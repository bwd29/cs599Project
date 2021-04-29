#include "actuator.h"
#include "world.h"
#include "node.h"
#include "agent.h"
#include <stdio.h>
#include "omp.h"

// mpic++ main.cpp -o test -lm -fopenmp -lssl -lcrypto -O3
// mpirun -np 2 -hostfile myhostfile.txt ./test

// run a brute force with actuators
int main(int argc, char * argv[]){

    int chain = atoi(argv[1]);


    int my_rank, nprocs;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

    if(my_rank == 0) printf("\nChain length of: %d\n", chain);


    int numActuators = 2;
    Vec destination = Vec(0,0,1);
    DTYPE actuatorStepSize = 0.01;
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


    MPI_Barrier(MPI_COMM_WORLD);

    // if(my_rank == 0) printf("\nCheck Packing");
    // char * packTest;
    // test.packWorld(&packTest);
    // World tmpTest = World(packTest,numActuators);

    // if(my_rank == 0){
    //     printf("\n\nPacked ->");
    //     test.print();
    //     printf("\n\nUnpacked ->");
    //     tmpTest.print();
    // }



    if(my_rank ==0) printf("\n\n\nRunnng agent code\n");
    double timeA1 = omp_get_wtime();

    Agent search = Agent(test, chain,  my_rank, nprocs);
    World solutionWorld = search.findPath();



    MPI_Barrier(MPI_COMM_WORLD);

    double timeA2 = omp_get_wtime();
    if(my_rank ==0)  printf("Solution found with time: %f", timeA2 - timeA1);
    if(my_rank ==0) printf("\nsyncTime: %f", search.syncTime);
    if(my_rank ==0) printf("\n Node search Time: %f", timeA2 - timeA1 - search.syncTime);

    if(my_rank ==0) solutionWorld.print();

    MPI_Barrier(MPI_COMM_WORLD);



    printf("\n");

    return 0;


}