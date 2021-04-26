#pragma once

#include <algorithm>
#include "world.h"
#include <string>


class Node{

    public:

        //variable to keep track of the world associatd with this node
        World worldState;

        //a vector of children nodes
        std::vector<Node*> children;

        omp_lock_t pushLock;

        // a pointer to the parent node
        Node * parent;

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

        void addChildren(std::vector<unsigned int> visited){
            //get the number of actuators
            int numActuators = worldState.actuators.size();

            // add children based on possible moves

            int i;
            // #pragma omp parallel for shared(i) num_threads(numActuators)
            for(i = 0; i < numActuators; i++){
                //try to move actuator by stepsize up
                if(worldState.actuators[i].checkRotate(worldState.actuators[i].stepSize)){
                    //make new world
                    World newWorld = World(&worldState);

                    //advance to new state
                    newWorld.moveActuators(i, worldState.actuators[i].stepSize);

                    //make new Node
                    Node * newNode = new Node(newWorld, this);

                    bool beenThere = std::binary_search(visited.begin(), visited.end(), newNode->getHash());

                    //push back child on node vector
                    if(!beenThere){
                        omp_set_lock(&pushLock);
                        children.push_back(newNode); 
                        omp_unset_lock(&pushLock);
                    }

                }

                //try to move actuator by stepsize down
                if(worldState.actuators[i].checkRotate(-1*worldState.actuators[i].stepSize)){
                   
                    //make new world
                    World newWorld = World(&worldState);

                    //advance to new state
                    newWorld.moveActuators(i, -1*worldState.actuators[i].stepSize);

                    //make new Node
                    Node * newNode = new Node(newWorld, this);

                    bool beenThere = std::binary_search(visited.begin(), visited.end(), newNode->getHash());
                    
                    //push back child on node vector
                    if(!beenThere){
                        omp_set_lock(&pushLock);
                        children.push_back(newNode); 
                        omp_unset_lock(&pushLock);
                    }
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

        unsigned int getHash(){
            std::string val = "";
            for(int i = 0; i < worldState.actuators.size(); i++){
                val += std::to_string(worldState.actuators[i].currentAngle);
                val += ",";
            }

            return std::hash<std::string>{}(val);
        }


        unsigned int packNode(char * pack){

            unsigned int worldPackSize = worldState.packWorld(pack);

            unsigned int packSize = worldPackSize;

            return packSize;
        }

        void unpackNode(char * pack, int numAct){

            worldState = World(pack, numAct);
            determineCost();

        }

};