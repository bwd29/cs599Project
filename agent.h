#pragma once

#include "node.h"
#include <queue>
#include <string>
#include <unordered_map>
#include <mpi.h>

#define MAXQ 10
#define CHAINLENGTH 1000

class Agent{

    public:

        Node * bestNode;
        std::deque<Node*> openSet;
        std::vector<unsigned int> visited;
        int my_rank;
        int nprocs;
        int numAct;




        Agent(World startWorld, int rank = 0, int np = 1){
            //make a starter node
            Node * start = new Node(startWorld);
            // start->cost = start->worldState.checkDist();sss
            openSet.push_back(start);
            bestNode = start;
            numAct = startWorld.actuators.size();
            my_rank = rank;
            nprocs = np;
        }


        bool sync(){
            // sync the visited

            unsigned int recvCount[nprocs];
            unsigned int visitedCount = visited.size();
            MPI_Gather( &visitedCount, 1 , MPI_UNSIGNED , recvCount, 1 , MPI_UNSIGNED , my_rank , MPI_COMM_WORLD);

            unsigned int visitedSize = 0;
            for(int i = 0; i < nprocs; i ++){
                visitedSize += recvCount[i];
                printf("%d,", recvCount[i]);
            }
            unsigned int * visitedBuffer = (unsigned int *)malloc(visitedSize *sizeof(unsigned int));

            MPI_Request request;
            for(int i = 0; i < nprocs; i++){
                MPI_Isend( &visited[0], visitedCount , MPI_UNSIGNED , i , 0, MPI_COMM_WORLD , &request);
            } 

            MPI_Status status;
            unsigned int runningTotal = 0;
            for(int i = 0; i < nprocs; i++){
                MPI_Recv( &visitedBuffer[runningTotal], recvCount[i], MPI_UNSIGNED , i , 0 , MPI_COMM_WORLD , &status);
                runningTotal += recvCount[i];
            }

            //make new vector

            std::vector<unsigned int> tmpVisited(visitedBuffer, visitedBuffer + visitedSize);
            std::sort(tmpVisited.begin(), tmpVisited.end());
            std::vector<unsigned int>::iterator idx;
            idx = std::unique(tmpVisited.begin(), tmpVisited.end());
            tmpVisited.resize(std::distance(tmpVisited.begin(), idx));

            visited = tmpVisited;

            //sync the opensets

            

            //sync the best node
            char * nodePack;
            unsigned int packSize = bestNode->packNode(nodePack);
            for(int i  = 0; i < nprocs; i++){
                if(i != my_rank){
                    MPI_Isend(nodePack, packSize, MPI_CHAR , i , 0 , MPI_COMM_WORLD , &request);
                }
            }

            char * recvPack = (char *)malloc(packSize);
            for(int i = 0; i < nprocs; i++){
                if(my_rank != 0){
                    MPI_Recv( recvPack , packSize, MPI_CHAR , i , 0 , MPI_COMM_WORLD , &status);
                    Node * tmpNode = new Node(recvPack, numAct);
                    if(bestNode->cost > tmpNode->cost){
                        bestNode = tmpNode;
                    } else {
                        // free(tmpNode);
                    }
                }

            }

            if(bestNode->cost > -0.000001 && bestNode->cost < 0.000001){
                return false;
            }

            
            //assign the opensets











            if(openSet.empty()){
                return false;
            }

            return true;

        }

        World findPath(){
            unsigned int count = 0;
            unsigned int chainCount = 0;
            while(sync()){
                while(!openSet.empty() && chainCount < CHAINLENGTH){

                    count++;
                    chainCount++;
                    Node * currentNode = openSet.front();

                    //hash the current node, and add to visited
                    
                    visited.insert(std::lower_bound(visited.begin(),visited.end(),currentNode->getHash()),currentNode->getHash());
                    

                    openSet.pop_front();

                    currentNode->addChildren(visited);
                    currentNode->rankChildren();

                    for(int i = currentNode->children.size() -1; i >= 0; i--){
                        if(currentNode->children[i]->cost < currentNode->cost){
                            openSet.push_front(currentNode->children[i]);
                        }
                        
                    }
                    // openSet.push_back(currentNode->children[0]);

                    std::sort(openSet.begin(), openSet.end());

                    while(openSet.size() > MAXQ){
                        openSet.pop_back();
                    }

                    // currentNode->worldState.print();
                    // printf("\nCost: %f\n\n", currentNode->cost);

                    if(currentNode->cost < bestNode->cost){
                        bestNode = currentNode;
                    }

                }
                chainCount = 0;
            }

            printf("\nVisited %u nodes\n", count);
            return bestNode->worldState;


        }


};