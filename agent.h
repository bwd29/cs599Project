#pragma once

#include "node.h"
#include <queue>
#include <string>
#include <unordered_map>
#include <mpi.h>

#define MAXQ 100
#define CHAINLENGTH 1000

class Agent{

    public:

        Node * bestNode;
        std::deque<Node*> openSet;
        std::vector<unsigned long long> visited;
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
            MPI_Request request;
            MPI_Status status;
            MPI_Barrier(MPI_COMM_WORLD);
            // sync the visited

            unsigned int recvCount[nprocs];
            unsigned int visitedCount = visited.size();
            printf("\nrank: %d send size: %d",my_rank, visitedCount);
            // MPI_Igather( &visitedCount, 1 , MPI_UNSIGNED , recvCount, 1 , MPI_UNSIGNED , my_rank , MPI_COMM_WORLD, &request);
            // MPI_Bcast( &recvCount[my_rank] , 1 , MPI_UNSIGNED , my_rank , MPI_COMM_WORLD);

            for(int i = 0; i < nprocs; i++){
                if(i == my_rank){
                    recvCount[i] = visitedCount;
                } else {
                    MPI_Isend( &visitedCount , 1 , MPI_UNSIGNED , i , 0 , MPI_COMM_WORLD, &request);
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);

            for(int i = 0; i < nprocs; i++){
                if(i == my_rank){
                    // recvCount[i] = visitedCount;
                } else {
                    MPI_Recv( &recvCount[i] ,1 , MPI_UNSIGNED , i , 0 , MPI_COMM_WORLD , &status);
                }       
            }

            MPI_Barrier(MPI_COMM_WORLD);

            if(my_rank ==1) printf("\nRecive counts: ");
            unsigned int visitedSize = 0;
            for(int i = 0; i < nprocs; i ++){
                visitedSize += recvCount[i];
                if(my_rank ==1) printf("%d,", recvCount[i]);
            }
            if(my_rank == 1) printf("\ntotal recived: %d,", visitedSize);

            unsigned long long * visitedBuffer = (unsigned long long *)malloc(visitedSize *sizeof(unsigned long long));

    
            for(int i = 0; i < nprocs; i++){
                MPI_Isend( &visited[0], visitedCount , MPI_UNSIGNED_LONG_LONG , i , 0, MPI_COMM_WORLD , &request);
            } 

            MPI_Barrier(MPI_COMM_WORLD);


            unsigned int runningTotal = 0;
            for(int i = 0; i < nprocs; i++){
                MPI_Recv( &visitedBuffer[runningTotal], recvCount[i], MPI_UNSIGNED_LONG_LONG , i , 0 , MPI_COMM_WORLD , &status);
                runningTotal += recvCount[i];
            }

            
            MPI_Barrier(MPI_COMM_WORLD);

            //make new vector

            std::vector<unsigned long long> tmpVisited(visitedBuffer, visitedBuffer + visitedSize);
            std::sort(tmpVisited.begin(), tmpVisited.end());
            std::vector<unsigned long long>::iterator idx;
            idx = std::unique(tmpVisited.begin(), tmpVisited.end());
            tmpVisited.erase(idx, tmpVisited.end());

            // visited.swap(tmpVisited);
            visited.clear();
            for(int i = 0; i<tmpVisited.size();i++){
                visited.push_back(tmpVisited[i]);
            }

            printf("\nrank: %d, visited size: %ld", my_rank, visited.size());

            //sync the opensets
             MPI_Barrier(MPI_COMM_WORLD);
            

            // sync the best node
            char * nodePack;
            unsigned int packSize = bestNode->packNode(&nodePack);
            for(int i  = 0; i < nprocs; i++){
                if(i != my_rank){
                    MPI_Isend(nodePack, packSize, MPI_CHAR , i , 0 , MPI_COMM_WORLD , &request);
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);
            
            if(my_rank == 1) printf("\nSent data Packs");

            MPI_Barrier(MPI_COMM_WORLD);

            char * recvPack = (char *)malloc(packSize);
            for(int i = 0; i < nprocs; i++){
                if(my_rank != i){
                    MPI_Recv( recvPack , packSize, MPI_CHAR , i , 0 , MPI_COMM_WORLD , &status);
                    printf("\nrank %d recived pack from rank %d", my_rank, i);
                    Node * tmpNode = new Node(recvPack, numAct);
                    if(bestNode->cost > tmpNode->cost){
                        printf(".");
                        bestNode = tmpNode;
                    } else {
                        // free(tmpNode);
                    }
                }

            }
            printf("\nmy rank: %d, best cost: %f",my_rank, bestNode->cost);
            if(bestNode->cost > -0.000001 && bestNode->cost < 0.000001){
                printf("\nFound Node");
                return false;
            }

            
            //assign the opensets











            if(openSet.empty()){
                printf("\nopenset empty");
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

                    openSet.pop_front();

                    visited.insert(std::lower_bound(visited.begin(),visited.end(),currentNode->getHash()),currentNode->getHash());
                    
                    currentNode->addChildren(visited);
                    currentNode->rankChildren();

                    for(int i = 0; i  < currentNode->children.size(); i++){
                        bool beenThere = std::binary_search(visited.begin(), visited.end(), currentNode->children[i]->getHash());
                        if(!beenThere){
                            if(currentNode->children[i]->cost < currentNode->cost){
                                openSet.push_front(currentNode->children[i]);
                            }
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
                        // bestNode = new Node(currentNode);
                    }

                }
                chainCount = 0;
            }

            MPI_Barrier(MPI_COMM_WORLD);

            if(my_rank ==0) printf("\nVisited %u nodes\n", count);
            return bestNode->worldState;


        }


};