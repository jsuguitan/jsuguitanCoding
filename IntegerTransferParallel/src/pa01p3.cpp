
#include "mpi.h"
#include <iostream>
#include <stdlib.h>


using namespace std;

int main( int argc, char* argv[] )
{
    MPI_Init( &argc, &argv);
    int myrank;
    int index;
    int indexCount = 0;
    int msgtag = 5;
    int indexEnd = 100;
    double timeBegin = 0;
    double timeHolder = 0;
    double timeEnd = 0;
    MPI_Status status;
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
       
    if( myrank == 0 )
    {
        int x[10000];
        for( index = 0; index < 10000; index++ )
        {
            x[index] = rand() % 100;
        }

        for( indexCount = 1; indexCount < 10002; indexCount += 5 )
        {    
            timeHolder = 0;
            for( index = 0; index < indexEnd; index++ )  //indexEnd = 100
            {
                
                timeBegin = MPI_Wtime();

                MPI_Send( &x, indexCount, MPI_INT, 1, msgtag, MPI_COMM_WORLD ); 
                                                                       //4th arguement is that 
                                                                       // process myRank
                MPI_Recv( &x, indexCount, MPI_INT, 1, msgtag, MPI_COMM_WORLD, &status );
                
                timeEnd = MPI_Wtime();
                timeHolder = timeHolder + ( timeEnd - timeBegin );
            }
            timeHolder = timeHolder / indexEnd; //average time for pingpong
            cout << timeHolder << endl;
        }
    }
    else if( myrank == 1 ) 
    {        
        int x[10000];
        int index2 = 0;
        int indexOuter2 = 1;
        for( indexOuter2 = 1; indexOuter2 < 10002; indexOuter2 += 5 )
        {
            for( index2 = 0; index2 < indexEnd; index2++ )
            {
                MPI_Recv( &x, indexOuter2, MPI_INT, 0, msgtag, MPI_COMM_WORLD, &status );
                MPI_Send( &x, indexOuter2, MPI_INT, 0, msgtag, MPI_COMM_WORLD );
            }
        }
    }
    else
    {
        cout << "You don't know what you are doing" << endl;
    }

    MPI_Finalize();
}

//how to put file into the headnode
// scp pa01.cpp jsugutian@h1.cse.unr.edu:pa01.cpp

//how to compile
// mpic++ -lpmi -o pa01 pa01.cpp
// mpic++ -lpmi -o <executible> <cpp file>



//how to run
// srun -n2 pa01
// srun -<number of nodes> <executible
// lowercse n makes it within the same box

// srun -N2 pa01





