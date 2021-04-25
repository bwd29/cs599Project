#include "actuator.h"
#include "world.h"
#include "node.h"
#include "agent.h"
#include <stdio.h>
#include "omp.h"
#include <mpi.h>

// mpic++ bruteForce.cpp -o test -lm -fopenmp (-g or -o3)

// mpirun -np 4 -hostfile myhostfile.txt ./test

// run a brute force with actuators
int main(int argc, char ** argv){
    int myrank, nprocs;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);


    int numActuators = 2;
    Vec destination = Vec(0,0,1);
    DTYPE actuatorStepSize = 0.1;
    DTYPE minAngle = 0;
    DTYPE maxAngle = 180;
    Vec endPoint = Vec(2, 0, 0);

    // first build the world
    printf("Building the world...");
    World test(endPoint);
    for(int i = 0; i < numActuators; i++){
        Vec loc(i, 0, 0);
        Vec orient(0, -1, 0);
        Actuator act = Actuator(minAngle,minAngle,maxAngle,i+1, actuatorStepSize);
        test.addActuator(act,loc,orient);
    }

    test.destination = destination;
    
    printf("Complete\n");




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    printf("Performing test move:\n");
    World tempTest = World(test);
    tempTest.moveActuators(0,45);
    printf("Actuator Loations: ");
    for(int i = 0; i < numActuators; i++){
        printf("\n(%f, %f, %f)", tempTest.actuatorLocations[i].x, tempTest.actuatorLocations[i].y, tempTest.actuatorLocations[i].z);
    }
    printf("\nActuator angles: ");
      for(int i = 0; i < numActuators; i++){
        printf("%f, ", tempTest.actuators[i].currentAngle);
    }
    printf("\nEndPoint: (%f,%f,%f)", tempTest.endPoint.x, tempTest.endPoint.y, tempTest.endPoint.z);
    tempTest.moveActuators(1,45);
    printf("\nActuator Loations: ");
    for(int i = 0; i < numActuators; i++){
        printf("\n(%f, %f, %f)", tempTest.actuatorLocations[i].x, tempTest.actuatorLocations[i].y, tempTest.actuatorLocations[i].z);
    }
    printf("\nActuator angles: ");
      for(int i = 0; i < numActuators; i++){
        printf("%f, ", tempTest.actuators[i].currentAngle);
    }
    printf("\nEndPoint: (%f,%f,%f)", tempTest.endPoint.x, tempTest.endPoint.y, tempTest.endPoint.z);


    //now to try all combinations of movements
    //total possible states
    long numStates = pow((maxAngle-minAngle)/actuatorStepSize, numActuators);

    double time1 = omp_get_wtime();
    printf("\n\nItterating through %ld states...", numStates);

    DTYPE lowestScore = 1000;
    DTYPE bestAngles[numActuators];
    DTYPE motions = (maxAngle-minAngle)/actuatorStepSize;
    Vec actuatorLocs[numActuators];
    
    // omp_lock_t scoreLock;
    // omp_init_lock(&scoreLock);

    World bestWorld;
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DTYPE N = (maxAngle-minAngle);
    DTYPE localStep = N/nprocs;
    DTYPE mystart = myrank*localStep;
    if(myrank == nprocs-1){
        localStep += (int)N%nprocs;
    }

    // DTYPE i;
    // #pragma omp parallel for shared(i) num_threads(numActuators)
    for(DTYPE i = mystart; i <= (mystart+localStep); i+=actuatorStepSize){
        for(DTYPE j = minAngle; j <= maxAngle; j+=actuatorStepSize){
            World tempWorld = World(test);
            tempWorld.moveActuators(0,i);
            tempWorld.moveActuators(1,j);
            DTYPE score = tempWorld.checkDist();

            // omp_set_lock(&scoreLock);
            if(score < lowestScore){
                // omp_unset_lock(&scoreLock);
                lowestScore = score;
                bestWorld = World(tempWorld);
                bestAngles[0] = i;
                bestAngles[1] = j;
                actuatorLocs[0] = tempWorld.actuatorLocations[0];
                actuatorLocs[1] = tempWorld.actuatorLocations[1];
                // for(int k = 0; k < numActuators; k++){
                //     bestAngles[k] = tempWorld.actuators[k].currentAngle;
                // }
            }else{
                // omp_unset_lock(&scoreLock);
            }

        }
    }
    DTYPE globalLowScore;
    MPI_Allreduce(&lowestScore, &globalLowScore, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

    if(lowestScore == globalLowScore){
        double time2 = omp_get_wtime();

        printf("Complete!\nTime to brute search: %f \n", time2 - time1);

        printf("\nLowest score was %f with angles: ", lowestScore);
        
        for(int i = 0; i < numActuators; i++){
            printf("%f, ", bestAngles[i]);
        }

        printf("\nActuator Loations: ");
        for(int i = 0; i < numActuators; i++){
            printf("\n(%f, %f, %f)", actuatorLocs[i].x, actuatorLocs[i].y, actuatorLocs[i].z);
        }

        printf("\nEndPoint: (%f,%f,%f)\n", bestWorld.endPoint.x, bestWorld.endPoint.y, bestWorld.endPoint.z);
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // printf("\n\n\nRunnng agent code\n");
    // double timeA1 = omp_get_wtime();

    // Agent search = Agent(test);
    // World solutionWorld = search.findPath();

    // double timeA2 = omp_get_wtime();
    // printf("Solution found with time: %f", timeA2 - timeA1);

    // solutionWorld.print();


    // printf("\n");


    MPI_Finalize();
    return 0;
}