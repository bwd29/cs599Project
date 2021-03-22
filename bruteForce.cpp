#include "actuator.h"
#include "world.h"
#include <stdio.h>



// run a brute force with actuators
int main(int argc, char * argv[]){



    int numActuators = 2;
    Vec destination = Vec(0,0,1);
    DTYPE actuatorStepSize = 1;
    DTYPE minAngle = 0;
    DTYPE maxAngle = 180;
    Vec endPoint = Vec(3, 0, 0);

    // first build the world
    printf("Building the world...");
    World test(endPoint);
    for(int i = 0; i < numActuators; i++){
        Vec loc(i, 0, 0);
        Vec orient(0, 1, 0);
        Actuator act = Actuator(minAngle,minAngle,maxAngle,i+1);
        test.addActuator(act,loc,orient);
    }
    printf("Complete\n");

    //now to try all combinations of movements
    //total possible states
    long numStates = pow((maxAngle-minAngle)/actuatorStepSize, numActuators);

    printf("Itterating through %ld states...", numStates);

    DTYPE lowestScore = 100000000;
    DTYPE bestAngles[numActuators];
    int motions = (maxAngle-minAngle)/actuatorStepSize;
    // for(long i = 0; i < numStates; i++){
    //     
    //     
    //     World tempWorld = World(test);
    //     int numMovingActs = i/motions;
    //     for(int j = 0; j < numMovingActs; j++){
    //         tempWorld.moveActuators(j,i%motions);
    //     }
    World bestWorld;
    for(int i = 0; i < motions; i+=actuatorStepSize){
        for(int j = 0; j < motions; j+=actuatorStepSize){
            World tempWorld = World(test);
            tempWorld.moveActuators(0,i);
            tempWorld.moveActuators(1,j);
            DTYPE score = tempWorld.checkDist(destination);
            if(i == 30 && j == 120){
                printf("score: %f", score);
            } 
            if(score < lowestScore){
                lowestScore = score;
                bestWorld = World(tempWorld);
                bestAngles[0] = i;
                bestAngles[1] = j;
                // for(int k = 0; k < numActuators; k++){
                //     bestAngles[k] = tempWorld.actuators[k].currentAngle;
                // }
            }
        }
    }

    printf("...Complete\n");

    printf("Lowest score was %f with angles: ", lowestScore);
    for(int i = 0; i < numActuators; i++){
        printf("%f, ", bestAngles[i]);
    }
    printf("\n");


}