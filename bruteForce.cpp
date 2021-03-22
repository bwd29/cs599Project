#include "actuator.h"
#include "world.h"
#include <stdio.h>



// run a brute force with actuators
int main(int argc, char * argv[]){



    int numActuators = 2;
    Vec destination = Vec(0,0,3);
    DTYPE actuatorStepSize = 0.5;
    DTYPE minAngle = 0;
    DTYPE maxAngle = 180;
    Vec endPoint = Vec(2, 0, 0);

    // first build the world
    printf("Building the world...");
    World test(endPoint);
    for(int i = 0; i < numActuators; i++){
        Vec loc(i, 0, 0);
        Vec orient(0, -1, 0);
        Actuator act = Actuator(minAngle,minAngle,maxAngle,i+1);
        test.addActuator(act,loc,orient);
    }
    printf("Complete\n");

    //now to try all combinations of movements
    //total possible states
    long numStates = pow((maxAngle-minAngle)/actuatorStepSize, numActuators);

    printf("Itterating through %ld states...", numStates);

    DTYPE lowestScore = 1000;
    DTYPE bestAngles[numActuators];
    DTYPE motions = (maxAngle-minAngle)/actuatorStepSize;
    Vec actuatorLocs[numActuators];

    World bestWorld;
    for(DTYPE i = 0; i < motions; i+=actuatorStepSize){
        for(DTYPE j = 0; j < motions; j+=actuatorStepSize){
            World tempWorld = World(test);
            tempWorld.moveActuators(0,i);
            tempWorld.moveActuators(1,j);
            DTYPE score = tempWorld.checkDist(destination);
            // if(i == 30 && j == 120){
            //     printf("\nscore: %f", score);
            //     printf("\nsecond axis orient: (%f,%f,%f)", tempWorld.actuatorOrienations[1].x, tempWorld.actuatorOrienations[1].y, tempWorld.actuatorOrienations[1].z );
            // } 
            if(score < lowestScore){
                lowestScore = score;
                bestWorld = World(tempWorld);
                bestAngles[0] = i;
                bestAngles[1] = j;
                actuatorLocs[0] = tempWorld.actuatorLocations[0];
                actuatorLocs[1] = tempWorld.actuatorLocations[1];
                // for(int k = 0; k < numActuators; k++){
                //     bestAngles[k] = tempWorld.actuators[k].currentAngle;
                // }
            }
        }
    }

    printf("\nLowest score was %f with angles: ", lowestScore);
    for(int i = 0; i < numActuators; i++){
        printf("%f, ", bestAngles[i]);
    }

    printf("\nActuator Loations: ");
    for(int i = 0; i < numActuators; i++){
        printf("\n(%f, %f, %f)", actuatorLocs[i].x, actuatorLocs[i].y, actuatorLocs[i].z);
    }

    printf("\nEndPoint: (%f,%f,%f)", bestWorld.endPoint.x, bestWorld.endPoint.y, bestWorld.endPoint.z);

    printf("\n");


}