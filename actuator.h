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

};