#pragma once

#include "node.h"
#include <queue>
#include <string>
#include <unordered_map>


class Agent{

    public:

        Node * bestNode;
        std::deque<Node*> openSet;
        std::vector<unsigned int> visited;




        Agent(World startWorld){
            //make a starter node
            Node * start = new Node(startWorld);
            // start->cost = start->worldState.checkDist();sss
            openSet.push_back(start);
            bestNode = start;

        }

        World findPath(){
            unsigned int count = 0;
            while(!openSet.empty()){
                count++;
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