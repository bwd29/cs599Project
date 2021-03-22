#pragma once

#include "vec.h"
#include "actuator.h"
#include <vector>



class World{

    public:

        std::vector<Actuator> actuators;
        std::vector<Vec> actuatorLocations;
        std::vector<Vec> actuatorOrienations;

        Vec endPoint;

        DTYPE cost = 0;


        //default constructor
        World(){}

        //constructor for known endpoint
        World(Vec endPoint){
            this->endPoint = endPoint;
        }

        //copy constructor
        World(World * copy){
            actuators = copy->actuators;
            actuatorLocations = copy->actuatorLocations;
            actuatorOrienations = copy->actuatorOrienations;
            endPoint = copy->endPoint;
        }

        //add an actuator
        void addActuator(Actuator act, Vec location, Vec orientation){
            actuators.push_back(act);
            actuatorLocations.push_back(location);
            actuatorOrienations.push_back(orientation);
        }

        //actutor at index actIndex will move all others by alpha degrees
        void moveActuators(int actIndex, DTYPE alpha){

            //update angle of actuator
            actuators[actIndex].rotate(alpha);
            
            //moveing all actators after actIndex (not includeing the actuator at actIndex)
            
            Vec axisOfRotation = actuatorOrienations[actIndex].norm();
            actuatorOrienations[actIndex]= actuatorOrienations[actIndex].rot(axisOfRotation, alpha);

            //itterate through all of the actators and update actuator locations in world
            for(int i = actIndex + 1; i < actuators.size(); i++){
                //move actuator at i

                //first we place actuator location relative to moving actuator
                Vec tempLocation = actuatorLocations[i].diff(actuatorLocations[actIndex]);

                //rotate our reduced location
                tempLocation = tempLocation.rot(axisOfRotation, alpha);

                // add the vecotr back in to get new location
                actuatorLocations[i] = tempLocation.add(actuatorLocations[actIndex]);

                actuatorOrienations[i] = actuatorOrienations[i].rot(axisOfRotation, alpha);
            }

            //move the end point
            // orient the endpoint
            Vec tmpEnd = endPoint.diff(actuatorLocations[actIndex]);
            tmpEnd = tmpEnd.rot(axisOfRotation,alpha);

            endPoint = tmpEnd.add(actuatorLocations[actIndex]);

            cost += alpha*actuators[actIndex].speed;
        }

        //check distance from the endpoint to a vec
        DTYPE checkDist(Vec destination){
            return endPoint.dist(destination);
        }

        DTYPE calcScore(Vec dest){
            return cost + 10000000*endPoint.dist(dest);
        }


};