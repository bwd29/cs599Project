#pragma once

#include <algorithm>
#include "world.h"
#include <string>
#include <openssl/sha.h>
#include <cstring>

class Node{

    public:

        //variable to keep track of the world associatd with this node
        World worldState;

        //a vector of children nodes
        std::vector<Node*> children;

        omp_lock_t pushLock;

        // a pointer to the parent node
        Node * parent;

        // unsigned char hash[SHA256_DIGEST_LENGTH];

        DTYPE cost;

        //constructors
        Node(){}

        Node(char * pack, int numAct){
            unpackNode(pack, numAct);
        }

        Node(Node * nodeA){
            worldState = World(&(nodeA->worldState));
            children = nodeA->children;
            parent = nodeA->parent;
            cost= nodeA->cost;
            omp_init_lock(&pushLock);
        }

        Node(World world){
            worldState = World(&world);
            determineCost();
            omp_init_lock(&pushLock);
        }

        Node(World world, Node * newParent){
            worldState = World(&world);
            parent = newParent;
            cost = newParent->cost;
            omp_init_lock(&pushLock);
        }

        // overload the operator <
        bool operator<(const Node& r) const
        {

            return cost < r.cost;

        }
    
        // overload the operator >
        bool operator>(const Node& r) const
        {
            return cost > r.cost;
        }

        // methods

        //method to add children

        void addChildren(){
            //get the number of actuators
            int numActuators = worldState.actuators.size();

            // add children based on possible moves

            // int i;
            // #pragma omp parallel for shared(i) num_threads(numActuators)
            for(int i = 0; i < numActuators; i++){
                //try to move actuator by stepsize up
                if(worldState.actuators[i].checkRotate(worldState.actuators[i].stepSize)){
                    //make new world
                    World newWorld = World(&worldState);

                    //advance to new state
                    newWorld.moveActuators(i, worldState.actuators[i].stepSize);

                    //make new Node
                    Node * newNode = new Node(newWorld, this);

                    // bool beenThere = std::binary_search(visited.begin(), visited.end(), newNode->getHash());

                    //push back child on node vector
                    // if(!beenThere){
                        // omp_set_lock(&pushLock);
                        children.push_back(newNode); 
                        // omp_unset_lock(&pushLock);
                    // }

                }

                //try to move actuator by stepsize down
                if(worldState.actuators[i].checkRotate(-1*worldState.actuators[i].stepSize)){
                   
                    //make new world
                    World newWorld = World(&worldState);

                    //advance to new state
                    newWorld.moveActuators(i, -1*worldState.actuators[i].stepSize);

                    //make new Node
                    Node * newNode = new Node(newWorld, this);

                    // bool beenThere = std::binary_search(visited.begin(), visited.end(), newNode->getHash());
                    
                    //push back child on node vector
                    // if(!beenThere){
                        // omp_set_lock(&pushLock);
                        children.push_back(newNode); 
                        // omp_unset_lock(&pushLock);
                    // }
                }

            }

        }

        void determineCost(){
            //update based on distance
            cost = worldState.checkDist();

            //can further update for speed of actuators etc...
        }

        // method to reorder the children nodes by lowest cost to highest
        void rankChildren(){

            //update children costs
            for(int i = 0; i < children.size(); i++){
                children[i]->determineCost();
            }
            
            //sort by lowest cost to highes cost
            std::sort(children.begin(), children.end());

        }

        std::string getHash(){
            std::string val = "";
            for(int i = 0; i < worldState.actuators.size(); i++){
                val += std::to_string(worldState.actuators[i].currentAngle);
            }
            // char * pack;
            // packNode(&pack);
            // std::string val(pack);
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256_CTX sha256;
            SHA256_Init(&sha256);
            SHA256_Update(&sha256, val.c_str(),val.size());
            SHA256_Final(hash, &sha256);

            std::string hashString(hash, hash + SHA256_DIGEST_LENGTH);

            // unsigned long long * hashReturn = (unsigned long long *)(&hash[0]);
            // printf("\n%lld", *hashReturn);
            // return hash;

            // size_t result = 2166136261U ;
            // std::string::const_iterator end = val.end() ;
            // for ( std::string::const_iterator iter = val.begin() ;
            //     iter != end ;
            //     ++ iter ) {
            //     result = (16777619 * result)
            //             ^ static_cast< unsigned char >( *iter ) ;
            // }
            return hashString;
        }


        unsigned int packNode(char ** pack){

            char * worldPack;
            unsigned int worldPackSize = worldState.packWorld(&worldPack);

            unsigned int packSize = worldPackSize;
            
            *pack = (char*)malloc(packSize);

            for(int i = 0; i < worldPackSize; i++){
                (*pack)[i] = worldPack[i];
            }

            return packSize;
        }

        void unpackNode(char * pack, int numAct){

            worldState = World(pack, numAct);
            determineCost();

        }

};