#pragma once

#include "node.h"
#include <queue>
#include <string>
#include <unordered_map>
#include <mpi.h>

#define MAXQ 10
#define CHAINLENGTH 1000

 bool hashcmp(std::string a, std::string b){
     for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
         if(a[i] < b[i]){
             return true;
         }
     }
     return false;
 }

 bool hashpred(std::string a, std::string b){
     for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
         if(a[i] != b[i]){
             return true;
         }
     }
     return true;
 }

class Agent{

    public:

        Node * bestNode;
        std::deque<Node*> openSet;
        std::vector<std::string> visited;
        int my_rank;
        int nprocs;
        int numAct;

        double syncTime = 0;
        int chainlength;




        Agent(World startWorld, int chain, int rank = 0, int np = 1){
            //make a starter node
            Node * start = new Node(startWorld);
            // start->cost = start->worldState.checkDist();sss
            openSet.push_back(start);
            bestNode = start;
            numAct = startWorld.actuators.size();
            my_rank = rank;
            nprocs = np;
            chainlength = chain;
        }


        bool sync(){

            double syncTime1 = omp_get_wtime();

            MPI_Request request;
            MPI_Status status;
            // MPI_Barrier(MPI_COMM_WORLD);
            // sync the visited

            unsigned int recvCount[nprocs];
            unsigned int visitedCount = visited.size();
            // printf("\nrank: %d send size: %d",my_rank, visitedCount);
            // MPI_Igather( &visitedCount, 1 , MPI_UNSIGNED , recvCount, 1 , MPI_UNSIGNED , my_rank , MPI_COMM_WORLD, &request);
            // MPI_Bcast( &recvCount[my_rank] , 1 , MPI_UNSIGNED , my_rank , MPI_COMM_WORLD);

            for(int i = 0; i < nprocs; i++){
                if(i == my_rank){
                    recvCount[i] = visitedCount;
                } else {
                    MPI_Isend( &visitedCount , 1 , MPI_UNSIGNED , i , 0 , MPI_COMM_WORLD, &request);
                }
            }
            // MPI_Barrier(MPI_COMM_WORLD);

            for(int i = 0; i < nprocs; i++){
                if(i == my_rank){
                    // recvCount[i] = visitedCount;
                } else {
                    MPI_Recv( &recvCount[i] ,1 , MPI_UNSIGNED , i , 0 , MPI_COMM_WORLD , &status);
                }       
            }

            // MPI_Barrier(MPI_COMM_WORLD);

            // if(my_rank ==0) printf("\nRecive counts: ");
            unsigned int visitedSize = 0;
            for(int i = 0; i < nprocs; i ++){
                visitedSize += recvCount[i];
                // if(my_rank ==0) printf("%d,", recvCount[i]);
            }
            // if(my_rank == 0) printf("\ntotal recived: %d,", visitedSize);

            char * visitedBuffer = (char *)malloc(visitedSize *sizeof(char)*SHA256_DIGEST_LENGTH);

            char * visitedStream = (char*)malloc(visitedCount*SHA256_DIGEST_LENGTH);
            for(int i = 0; i < visitedCount; i++){
                for(int j = 0; j < SHA256_DIGEST_LENGTH; j++){
                    visitedStream[i*SHA256_DIGEST_LENGTH + j] = visited[i][j];
                }
            }

            for(int i = 0; i < nprocs; i++){
                MPI_Isend( visitedStream, visitedCount*SHA256_DIGEST_LENGTH , MPI_CHAR , i , 0, MPI_COMM_WORLD , &request);
            } 

            // MPI_Barrier(MPI_COMM_WORLD);


            unsigned int runningTotal = 0;
            for(int i = 0; i < nprocs; i++){
                MPI_Recv( &visitedBuffer[runningTotal*SHA256_DIGEST_LENGTH], recvCount[i]*SHA256_DIGEST_LENGTH, MPI_CHAR , i , 0 , MPI_COMM_WORLD , &status);
                runningTotal += recvCount[i];
            }

            
            // MPI_Barrier(MPI_COMM_WORLD);

            //make new vector
             
            visited.clear();
            // visited.resize(visitedSize);
            // printf("\nnew vistied size: %ld ", visited.size());
            // printf("\n hash length = %d", SHA256_DIGEST_LENGTH);
            unsigned int array_counter = 0;
            unsigned int counting = 0;
            for(int i = 0; i < visitedSize; i++){

                std::string hash(&visitedBuffer[i*SHA256_DIGEST_LENGTH],&visitedBuffer[i*SHA256_DIGEST_LENGTH] + SHA256_DIGEST_LENGTH );

                if(array_counter == 0){
                    visited.push_back(hash);
                    array_counter++;
                    
                }else {

                    bool inArray = std::binary_search(visited.begin(), visited.end(), hash);

                    if(inArray == false){
                        visited.insert(std::lower_bound(visited.begin(),visited.end(),hash),hash);
                        array_counter ++;
                    }else{
                        counting++;
                    }
                }
                
                // std::sort(visited.begin(), visited.end(), hashcmp);
            }
            // if(my_rank ==0) printf("\nnum duplicates: %d", counting);

            
            // std::vector<unsigned char[SHA256_DIGEST_LENGTH]>::iterator idx;
            // idx = std::unique(visited.begin(), visited.end(), hashpred);
            visited.erase(visited.begin() + array_counter, visited.end());




            // std::vector<unsigned long long> tmpVisited(visitedBuffer, visitedBuffer + visitedSize);
            // std::sort(tmpVisited.begin(), tmpVisited.end());
            // std::vector<unsigned long long>::iterator idx;
            // idx = std::unique(tmpVisited.begin(), tmpVisited.end());
            // tmpVisited.erase(idx, tmpVisited.end());

            // // visited.swap(tmpVisited);
            // visited.clear();
            // for(int i = 0; i<tmpVisited.size();i++){
            //     visited.push_back(tmpVisited[i]);
            // }

            // printf("\nrank: %d, visited size: %ld", my_rank, visited.size());

            //sync the opensets
            //  MPI_Barrier(MPI_COMM_WORLD);
            

            // sync the best node
            char * nodePack;
            unsigned int packSize = bestNode->packNode(&nodePack);
            for(int i  = 0; i < nprocs; i++){
                if(i != my_rank){
                    MPI_Isend(nodePack, packSize, MPI_CHAR , i , 0 , MPI_COMM_WORLD , &request);
                }
            }

            // MPI_Barrier(MPI_COMM_WORLD);
            
            // if(my_rank == 1) printf("\nSent data Packs");

            // MPI_Barrier(MPI_COMM_WORLD);

            char * recvPack = (char *)malloc(packSize);
            for(int i = 0; i < nprocs; i++){
                if(my_rank != i){
                    MPI_Recv( recvPack , packSize, MPI_CHAR , i , 0 , MPI_COMM_WORLD , &status);
                    // printf("\nrank %d recived pack from rank %d", my_rank, i);
                    Node * tmpNode = new Node(recvPack, numAct);
                    if(bestNode->cost > tmpNode->cost){
                        // printf(".");
                        bestNode = tmpNode;
                    } else {
                        // free(tmpNode);
                    }
                }

            }
            // printf("\nmy rank: %d, best cost: %f",my_rank, bestNode->cost);
            if(bestNode->cost > -0.000001 && bestNode->cost < 0.000001){
                // printf("\nFound Node");
                return false;
            }

            
            //assign the opensets


            unsigned int openSize = openSet.size();
            // printf("\nRank: %d, Openset size: %d", my_rank, openSize);        
            for(int i = 0; i < nprocs; i++){
                if(i == my_rank){
                    recvCount[i] = openSize;
                } else {
                    MPI_Isend( &openSize , 1 , MPI_UNSIGNED , i , 0 , MPI_COMM_WORLD, &request);
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

            unsigned int totalOpenSize = 0;
            for(int i = 0; i < nprocs; i ++){
                totalOpenSize += recvCount[i];
            }
            // printf("\nRank: %d, totalOpenSize: %d", my_rank, totalOpenSize);

            if(totalOpenSize == 0){
                // printf("\nopenset empty");
                return false;
            }

            char * openPack;
            char * openStream = (char *)malloc(packSize*openSize);

            for(int i =0; i < openSize; i++){
                openSet[i]->packNode(&openPack);
                for(int j = 0; j < packSize; j++){
                    openStream[i*packSize + j] = openPack[j]; 
                }
            }


            char * openBuffer = (char *)malloc(totalOpenSize*packSize);

            for(int i = 0; i < nprocs; i++){
                MPI_Isend( openStream, openSize*packSize , MPI_CHAR , i , 0, MPI_COMM_WORLD , &request);
            } 

            // MPI_Barrier(MPI_COMM_WORLD);


            runningTotal = 0;
            for(int i = 0; i < nprocs; i++){
                MPI_Recv( &openBuffer[runningTotal*packSize], recvCount[i]*packSize, MPI_CHAR , i , 0 , MPI_COMM_WORLD , &status);
                runningTotal += recvCount[i];
            }

            openSet.clear();

            std::vector<std::string> openHash;
            for(int i = 0; i < totalOpenSize; i++){
                
                Node * tmp = new Node(&openBuffer[i*packSize], numAct);
                
                std::string tmpHash = tmp->getHash();
                if(std::binary_search(openHash.begin(), openHash.end(), tmpHash)){
                    // dont add duplicates
                    // printf("\nduplicate");
                }else{
                    openHash.insert(std::lower_bound(openHash.begin(), openHash.end(), tmpHash),tmpHash);
                    openSet.push_back(tmp);
                }
                
                
            }

            // if(my_rank == 0) printf("\nTotal uniques openset: %ld", openSet.size());

            unsigned int nodesPerRank = openSet.size() / nprocs;
            
            std::vector<Node*> tmpSet;
            if(my_rank == nprocs - 1){
                for(int i = nodesPerRank*(nprocs-1); i < openSet.size();i++){
                    tmpSet.push_back(openSet[i]);
                }
                
            }else {
                for(int i = nodesPerRank*my_rank; i < nodesPerRank*my_rank + nodesPerRank;i++){
                    tmpSet.push_back(openSet[i]);
                }
            }

            openSet.clear();
            for(int i = 0; i < tmpSet.size();i++){
                openSet.push_back(tmpSet[i]);
            }
            
            // MPI_Barrier(MPI_COMM_WORLD);

            // printf("\nRank: %d, set size: %ld", my_rank, openSet.size());

            std::sort(openSet.begin(), openSet.end());

            // MPI_Barrier(MPI_COMM_WORLD);

            syncTime += omp_get_wtime() - syncTime1;

            return true;

        }


        World findPath(){
            unsigned int count = 0;
            unsigned int chainCount = 0;
            while(sync()){
                // if(my_rank ==0) printf("\nSYNCHED\n");
                // unsigned int visitedStart = visited.size();
                // visited.resize(visited.size()+CHAINLENGTH);

                // MPI_Barrier(MPI_COMM_WORLD);

                while(!openSet.empty() && chainCount < chainlength){
                    // if(my_rank == 0)  printf("\n%d,%da",my_rank,chainCount);
                    
                    
                    Node * currentNode = openSet.front();

                    // if(my_rank == 0)  printf("\n%d,%db",my_rank,chainCount);

                    openSet.pop_front();

                    // if(my_rank == 0)  printf("\n%d,%dc",my_rank,chainCount);
                    
                    bool check = std::binary_search(visited.begin(), visited.end(), currentNode->getHash());
                    
                    // if(my_rank == 0)  printf("\n%d,%dd",my_rank,chainCount);

                    chainCount++;
                    if(!check){
                        // if(my_rank == 0)  printf("\n%d,%dd1",my_rank,chainCount);

                        count++;
                       

                        
                    //    printf("a");
                        
                        std::string hash = currentNode->getHash();
                        // printf("\nhash len: %ld", hash.length());
                        // printf("%s", hash.c_str());
                        // std::string hashPush(hash, hash+SHA256_DIGEST_LENGTH);
                        // visited[visitedStart + chainCount] = hash;
                        // visited.push_back(hash);
                        // std::sort(visited.begin(),visited.end());
                        visited.insert(std::lower_bound(visited.begin(),visited.end(),currentNode->getHash()),currentNode->getHash());

                        // if(my_rank == 0)  printf("\n%d,%dd2",my_rank,chainCount);

                        // std::sort(visited.begin(),visited.end());
                        
                        // if(count != visited.size()){
                        //     printf("\nBroke af at count: %d\n", count);
                        //     return bestNode->worldState;
                        // }
                        
                        currentNode->addChildren();
                        // if(my_rank == 0)  printf("\n%d,%dd2a",my_rank,chainCount);

                        currentNode->rankChildren();
                        // printf("\nnum chldren: %ld", currentNode->children.size());
                        // if(my_rank == 0)  printf("\n%d,%dd3",my_rank,chainCount);

                        // printf("b");
                        for(int i = 0; i  < currentNode->children.size(); i++){
                            // currentNode->children[i]->getHash();
                            // unsigned char * hash = currentNode->children[i]->hash;
                            bool beenThere = std::binary_search(visited.begin(), visited.end(), currentNode->children[i]->getHash());
                            if(!beenThere){
                                if(currentNode->children[i]->cost < currentNode->cost){
                                    
                                    openSet.push_front(currentNode->children[i]);
                                }
                            }else{
                                // printf("\nDuplicate");
                            }                        
                        }
                        // openSet.push_back(currentNode->children[0]);

                        // printf("\nopen set size A: %ld", openSet.size());
                        // if(my_rank == 0)  printf("\n%d,%dd4",my_rank,chainCount);

                        std::sort(openSet.begin(), openSet.end());

                        while(openSet.size() > MAXQ){
                            openSet.pop_back();
                        }

                        // printf("\nopen set size B: %ld", openSet.size());
                        // if(my_rank == 0)  printf("\n%d,%dd5",my_rank,chainCount);

                        // currentNode->worldState.print();
                        // printf("\nCost: %f\n\n", currentNode->cost);

                        if(currentNode->cost < bestNode->cost){
                            bestNode = currentNode;
                            // bestNode = new Node(currentNode);
                        }
                        // if(my_rank == 0)  printf("\n%d,%dd5",my_rank,chainCount);

                    }

                    // if(my_rank == 0)  printf("\n%d,%de",my_rank,chainCount);


                }
                
                chainCount = 0;
                // printf("\nrank: %d, current visted: %d, visited size %ld", my_rank, count, visited.size());
            }

            MPI_Barrier(MPI_COMM_WORLD);

            if(my_rank ==0) printf("\nVisited %ld nodes\n", visited.size());
            return bestNode->worldState;


        }


};