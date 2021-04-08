#pragma once

#include "node.h"
#include <queue>


class Agent{

    public:

        Node * bestNode;
        std::deque<Node*> openSet;



        Agent(World startWorld){
            //make a starter node
            Node * start = new Node(startWorld);
            // start->cost = start->worldState.checkDist();
            openSet.push_back(start);
            bestNode = start;

        }

        World findPath(){
            unsigned int count = 0;
            while(!openSet.empty()){
                count++;
                Node * currentNode = openSet.front();


                openSet.pop_front();

                currentNode->addChildren();
                currentNode->rankChildren();

                for(int i = currentNode->children.size() -1; i >= 0; i--){
                    if(currentNode->children[i]->cost < currentNode->cost){
                        openSet.push_front(currentNode->children[i]);
                    }
                    
                }
                // openSet.push_back(currentNode->children[0]);

                // std::sort(openSet.begin(), openSet.end());

                // currentNode->worldState.print();
                // printf("\nCost: %f\n\n", currentNode->cost);

                if(currentNode->cost > -0.000001 && currentNode->cost < 0.000001){
                    printf("\nVisited %u nodes\n", count);
                    return currentNode->worldState;
                } else {
                    if(currentNode->cost < bestNode->cost){
                        bestNode = currentNode;
                    }
                }

            }
            printf("\nVisited %u nodes\n", count);
            return bestNode->worldState;


        }
};