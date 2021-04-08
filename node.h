#pragma once

#include <algorithm>
#include "world.h"


class Node{

    public:

        //variable to keep track of the world associatd with this node
        World worldState;

        //a vector of children nodes
        std::vector<Node*> children;

        // a pointer to the parent node
        Node * parent;

        DTYPE cost;

        //constructors
        Node(){}

        Node(Node * nodeA){
            worldState = World(&(nodeA->worldState));
            children = nodeA->children;
            parent = nodeA->parent;
            cost= nodeA->cost;
        }

        Node(World world){
            worldState = World(&world);
            cost = worldState.checkDist();
        }

        Node(World world, Node newParent){
            worldState = World(&world);
            parent = &newParent;
            cost = newParent.cost;
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
            for(int i = 0; i < numActuators; i++){
                //try to move actuator by stepsize up
                if(worldState.actuators[i].checkRotate(worldState.actuators[i].stepSize)){
                    //make new world
                    World newWorld = World(&worldState);

                    //advance to new state
                    newWorld.moveActuators(i, worldState.actuators[i].stepSize);

                    //make new Node
                    Node * newNode = new Node(newWorld, this);

                    //push back child on node vector
                    children.push_back(newNode); 


                }

                //try to move actuator by stepsize down
                if(worldState.actuators[i].checkRotate(-1*worldState.actuators[i].stepSize)){
                   
                    //make new world
                    World newWorld = World(&worldState);

                    //advance to new state
                    newWorld.moveActuators(i, -1*worldState.actuators[i].stepSize);

                    //make new Node
                    Node * newNode = new Node(newWorld, this);

                    //push back child on node vector
                    children.push_back(newNode); 
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



};