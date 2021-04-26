#pragma once

#include "vec.h"

class Actuator{

    public:
        DTYPE currentAngle;
        DTYPE minAngle;
        DTYPE maxAngle;
        DTYPE stepSize;

        DTYPE speed;


        //default constructor
        Actuator(){}

        Actuator(char * pack){
            unpackActuator(pack);
        }

        //custom constructor for known settings
        Actuator(DTYPE currentAngle, DTYPE minAngle, DTYPE maxAngle, DTYPE speed, DTYPE stepSize){
            this->currentAngle = currentAngle;
            this->minAngle = minAngle;
            this->maxAngle = maxAngle;
            this->speed = speed;
            this->stepSize = stepSize; 
        }

        //sets current angle and returns true if legal move, otherwise returns false
        bool rotate(DTYPE alpha){
            if(currentAngle + alpha > minAngle && currentAngle + alpha < maxAngle){
                currentAngle = currentAngle + alpha;
                return true;
            }else{
                return false;
            }
        }

        bool checkRotate(DTYPE alpha){
            if(currentAngle + alpha > minAngle && currentAngle + alpha < maxAngle){
                return true;
            }else{
                return false;
            }
        }

        unsigned int packActuator(char ** pack){
            unsigned int packSize = sizeof(DTYPE)*5;
            *pack = (char *)malloc(packSize);
            char*ptr1 = (char*)(&currentAngle);
            char*ptr2 = (char*)(&minAngle);
            char*ptr3 = (char*)(&maxAngle);
            char*ptr4 = (char*)(&stepSize);
            char*ptr5 = (char*)(&speed);
            for(int i = 0; i < sizeof(DTYPE); i++){
                *pack[0*sizeof(DTYPE) + i] = ptr1[i];
                *pack[1*sizeof(DTYPE) + i] = ptr2[i];
                *pack[2*sizeof(DTYPE) + i] = ptr3[i];
                *pack[3*sizeof(DTYPE) + i] = ptr4[i];
                *pack[4*sizeof(DTYPE) + i] = ptr5[i];
            }

            return packSize;
        }

        void unpackActuator(char * pack){
            char*ptr1 = (char*)(&currentAngle);
            char*ptr2 = (char*)(&minAngle);
            char*ptr3 = (char*)(&maxAngle);
            char*ptr4 = (char*)(&stepSize);
            char*ptr5 = (char*)(&speed);
            for(int i = 0; i < sizeof(DTYPE); i++){
                ptr1[i] = pack[0*sizeof(DTYPE) + i];
                ptr2[i] = pack[1*sizeof(DTYPE) + i];
                ptr3[i] = pack[2*sizeof(DTYPE) + i];
                ptr4[i] = pack[3*sizeof(DTYPE) + i];
                ptr5[i] = pack[4*sizeof(DTYPE) + i];
            }
        }

};