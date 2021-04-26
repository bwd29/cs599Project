#pragma once

#include "vec.h"
#include "actuator.h"
#include <vector>
#include <stdio.h>



class World{

    public:

        std::vector<Actuator> actuators;
        std::vector<Vec> actuatorLocations;
        std::vector<Vec> actuatorOrientations;

        Vec endPoint;

        Vec destination;

        DTYPE cost = 0;


        //default constructor
        World(){}

        //constructor for known endpoint
        World(Vec endPoint){
            this->endPoint = endPoint;
        }

        World(char * pack, int numAct){
            unpackWorld(pack, numAct);
        }

        //copy constructor
        World(World * copy){
            actuators = copy->actuators;
            actuatorLocations = copy->actuatorLocations;
            actuatorOrientations = copy->actuatorOrientations;
            endPoint = copy->endPoint;
            destination = copy->destination;
        }

        //add an actuator
        void addActuator(Actuator act, Vec location, Vec orientation){
            actuators.push_back(act);
            actuatorLocations.push_back(location);
            actuatorOrientations.push_back(orientation);
        }

        //actutor at index actIndex will move all others by alpha degrees
        void moveActuators(int actIndex, DTYPE alpha){

            //update angle of actuator
            actuators[actIndex].rotate(alpha);
            
            //moveing all actators after actIndex (not includeing the actuator at actIndex)
            
            Vec axisOfRotation = actuatorOrientations[actIndex].norm();
            actuatorOrientations[actIndex]= actuatorOrientations[actIndex].rot(axisOfRotation, alpha);

            //itterate through all of the actators and update actuator locations in world
            for(int i = actIndex + 1; i < actuators.size(); i++){
                //move actuator at i

                //first we place actuator location relative to moving actuator
                Vec tempLocation = actuatorLocations[i].diff(actuatorLocations[actIndex]);

                //rotate our reduced location
                tempLocation = tempLocation.rot(axisOfRotation, alpha);

                // add the vecotr back in to get new location
                actuatorLocations[i] = tempLocation.add(actuatorLocations[actIndex]);

                actuatorOrientations[i] = actuatorOrientations[i].rot(axisOfRotation, alpha);
            }

            //move the end point
            // orient the endpoint
            Vec tmpEnd = endPoint.diff(actuatorLocations[actIndex]);
            tmpEnd = tmpEnd.rot(axisOfRotation,alpha);

            endPoint = tmpEnd.add(actuatorLocations[actIndex]);

            cost += alpha*actuators[actIndex].speed;
        }

        //check distance from the endpoint to a vec
        DTYPE checkDist(){
            return endPoint.dist(destination);
        }

        DTYPE calcScore(Vec dest){
            return cost + 10000000*endPoint.dist(dest);
        }

        void print(){
            printf("\nActuator loations:");
            for(int i = 0; i < actuators.size(); i++){
                printf("\n(%f, %f, %f)", actuatorLocations[i].x, actuatorLocations[i].y, actuatorLocations[i].z);
            }

            printf("\nActuator orientations:");
            for(int i = 0; i < actuators.size(); i++){
                printf("\n(%f, %f, %f)", actuatorOrientations[i].x, actuatorOrientations[i].y, actuatorOrientations[i].z);
            }

            printf("\nActuator angles:\n(");
            for(int i = 0; i < actuators.size(); i++){
                printf(" %f, ", actuators[i].currentAngle);
            }
            printf(")");
        }

        unsigned int packWorld(char ** pack){
            // printf("1x,");

            char * tmpActPack;
            char * tmpVecPack;
            unsigned int actuatorPackSize = actuators[0].packActuator(&tmpActPack);
            // printf("2x,");
            unsigned int vecPackSize = actuatorLocations[0].packVec(&tmpVecPack);
            //free tmps?

            int numVectors = actuatorLocations.size()+actuatorOrientations.size()+2;
            unsigned int packSize = sizeof(DTYPE)*1 +
                                    vecPackSize*(numVectors) +
                                    actuatorPackSize*actuators.size();
            
            *pack = (char*)malloc(packSize);
            for(int i = 0; i < actuators.size(); i++){
                char * actPack;
                actuators[i].packActuator(&actPack);
                for(int j = 0; j < actuatorPackSize; j++){
                    (*pack)[i*actuatorPackSize + j] = actPack[j];
                }
            }

            unsigned int packOffset = actuators.size()*actuatorPackSize;
            
            for(int i = 0; i < actuatorLocations.size(); i++){
                char * vecPack;
                actuatorLocations[i].packVec(&vecPack);
                for(int j = 0; j < vecPackSize; j++){
                    (*pack)[packOffset + i*vecPackSize + j] = vecPack[j];
                }
            }

            packOffset += actuatorLocations.size()*vecPackSize;
            for(int i = 0; i < actuatorOrientations.size(); i++){
                char * vecPack;
                actuatorOrientations[i].packVec(&vecPack);
                for(int j = 0; j < vecPackSize; j++){
                    (*pack)[packOffset + i*vecPackSize + j] = vecPack[j];
                }
            }

            packOffset += actuatorOrientations.size()*vecPackSize;

            char* ptr1;
            char* ptr2;
            endPoint.packVec(&ptr1);
            destination.packVec(&ptr2);
            for(int i = 0; i < vecPackSize; i++){
                (*pack)[packOffset + i] = ptr1[i];
                (*pack)[packOffset + vecPackSize + i] = ptr2[i];
            }

            packOffset += 2 * vecPackSize;

            char*ptr3 = (char*)(&cost);
            for(int i = 0; i < sizeof(DTYPE); i++){
                (*pack)[packOffset + i] = ptr3[i];
            }

            return packSize;


        }

        void unpackWorld(char * pack, int numActuators){
            char * tmpActPack;
            char * tmpVecPack;
            unsigned int actuatorPackSize = actuators[0].packActuator(&tmpActPack);
            unsigned int vecPackSize = actuatorLocations[0].packVec(&tmpVecPack);

            for(int i = 0; i < numActuators; i++){
                Actuator act = Actuator(&pack[i*actuatorPackSize]);
                Vec vec1 = Vec(&pack[actuatorPackSize*numActuators+i*vecPackSize]);
                Vec vec2 = Vec(&pack[actuatorPackSize*numActuators + numActuators*vecPackSize + i*vecPackSize]);
                addActuator(act, vec1, vec2);
            }

            unsigned int packOffset = actuatorPackSize*numActuators + numActuators*vecPackSize*2;
            endPoint = Vec(&pack[packOffset]);
            destination = Vec(&pack[packOffset + vecPackSize]);

            packOffset += 2*vecPackSize;

            char*ptr = (char*)(&cost);
            for(int i = 0; i < sizeof(DTYPE); i++){
                ptr[i] = pack[packOffset+i];
            }
        }


};