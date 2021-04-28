#include "actuator.h"
#include "world.h"
#include "node.h"
#include "agent.h"
#include <stdio.h>
#include "omp.h"

// mpic++ bruteForce.cpp -o test -lm -fopenmp (-g or -o3)
// mpirun -np 2 -hostfile myhostfile.txt ./test

// run a brute force with actuators
int main(int argc, char * argv[]){


    int my_rank, nprocs;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);


    int numActuators = 2;
    Vec destination = Vec(0,0,1);
    DTYPE actuatorStepSize = 0.1;
    DTYPE minAngle = 0;
    DTYPE maxAngle = 180;
    Vec endPoint = Vec(2, 0, 0);

    // first build the world
    if(my_rank ==0)  printf("Building the world...");
    World test(endPoint);
    for(int i = 0; i < numActuators; i++){
        Vec loc(i, 0, 0);
        Vec orient(0, -1, 0);
        Actuator act = Actuator(minAngle,minAngle,maxAngle,i+1, actuatorStepSize);
        test.addActuator(act,loc,orient);
    }

    test.destination = destination;

    if(my_rank ==0)  printf("Complete\n");






    // printf("Performing test move:\n");
    // World tempTest = World(test);
    // tempTest.moveActuators(0,45);
    // printf("Actuator Loations: ");
    // for(int i = 0; i < numActuators; i++){
    //     printf("\n(%f, %f, %f)", tempTest.actuatorLocations[i].x, tempTest.actuatorLocations[i].y, tempTest.actuatorLocations[i].z);
    // }
    // printf("\nActuator angles: ");
    //   for(int i = 0; i < numActuators; i++){
    //     printf("%f, ", tempTest.actuators[i].currentAngle);
    // }
    // printf("\nEndPoint: (%f,%f,%f)", tempTest.endPoint.x, tempTest.endPoint.y, tempTest.endPoint.z);
    // tempTest.moveActuators(1,45);
    // printf("\nActuator Loations: ");
    // for(int i = 0; i < numActuators; i++){
    //     printf("\n(%f, %f, %f)", tempTest.actuatorLocations[i].x, tempTest.actuatorLocations[i].y, tempTest.actuatorLocations[i].z);
    // }
    // printf("\nActuator angles: ");
    //   for(int i = 0; i < numActuators; i++){
    //     printf("%f, ", tempTest.actuators[i].currentAngle);
    // }
    // printf("\nEndPoint: (%f,%f,%f)", tempTest.endPoint.x, tempTest.endPoint.y, tempTest.endPoint.z);


    // //now to try all combinations of movements
    // //total possible states
    // long numStates = pow((maxAngle-minAngle)/actuatorStepSize, numActuators);

    // double time1 = omp_get_wtime();
    // printf("\n\nItterating through %ld states...", numStates);

    // DTYPE lowestScore = 1000;
    // DTYPE bestAngles[numActuators];
    // DTYPE motions = (maxAngle-minAngle)/actuatorStepSize;
    // Vec actuatorLocs[numActuators];

    // World bestWorld;
    // for(DTYPE i = minAngle; i <= maxAngle; i+=actuatorStepSize){
    //     for(DTYPE j = minAngle; j <= maxAngle; j+=actuatorStepSize){
    //         World tempWorld = World(test);
    //         tempWorld.moveActuators(0,i);
    //         tempWorld.moveActuators(1,j);
    //         DTYPE score = tempWorld.checkDist();

    //         if(score < lowestScore){
    //             lowestScore = score;
    //             bestWorld = World(tempWorld);
    //             bestAngles[0] = i;
    //             bestAngles[1] = j;
    //             actuatorLocs[0] = tempWorld.actuatorLocations[0];
    //             actuatorLocs[1] = tempWorld.actuatorLocations[1];
    //             // for(int k = 0; k < numActuators; k++){
    //             //     bestAngles[k] = tempWorld.actuators[k].currentAngle;
    //             // }
    //         }
    //     }
    // }

    // double time2 = omp_get_wtime();

    // printf("Complete!\nTime to brute search: %f \n", time2 - time1);

    // printf("\nLowest score was %f with angles: ", lowestScore);
    // for(int i = 0; i < numActuators; i++){
    //     printf("%f, ", bestAngles[i]);
    // }

    // printf("\nActuator Loations: ");
    // for(int i = 0; i < numActuators; i++){
    //     printf("\n(%f, %f, %f)", actuatorLocs[i].x, actuatorLocs[i].y, actuatorLocs[i].z);
    // }

    // printf("\nEndPoint: (%f,%f,%f)", bestWorld.endPoint.x, bestWorld.endPoint.y, bestWorld.endPoint.z);


    MPI_Barrier(MPI_COMM_WORLD);

    if(my_rank == 0) printf("\nCheck Packing");
    char * packTest;
    test.packWorld(&packTest);
    World tmpTest = World(packTest,numActuators);

    if(my_rank == 0){
        printf("\n\nPacked ->");
        test.print();
        printf("\n\nUnpacked ->");
        tmpTest.print();
    }



    if(my_rank ==0) printf("\n\n\nRunnng agent code\n");
    double timeA1 = omp_get_wtime();

    Agent search = Agent(test, my_rank, nprocs);
    World solutionWorld = search.findPath();

   MPI_Barrier(MPI_COMM_WORLD);

    double timeA2 = omp_get_wtime();
    if(my_rank ==0)  printf("Solution found with time: %f", timeA2 - timeA1);

    if(my_rank ==0) solutionWorld.print();

   MPI_Barrier(MPI_COMM_WORLD);



    printf("\n");

    return 0;


}