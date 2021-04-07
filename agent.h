#pragma once

#include "node.h"
#include <queue>


class Agent{

    public:

        Node * bestNode;
        std::queue<Node*> openSet;



        Agent(World startWorld){
            //make a starter node
            Node * start = new Node(startWorld);
            openSet.push(start);
            bestNode = start;

        }

        World findPath(){
            
            while(!openSet.empty()){
                Node * currentNode = openSet.front();
                if(currentNode->cost == 0){
                    return currentNode->worldState;
                } else {
                    if(*currentNode < *bestNode){
                        bestNode = currentNode;
                    }
                }

                openSet.pop();

                currentNode->addChildren();
                currentNode->rankChildren();

                for(int i = 0; i < currentNode->children.size();i++){
                    if(*(currentNode->children)[i] < *currentNode){
                        openSet.push(currentNode->children[i]);
                    }
                    
                }

                std::sort(&openSet.front(), &openSet.back());

            }



        }
};