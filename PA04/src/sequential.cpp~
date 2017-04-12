/*
Jervyn Suguitan
3-28
Bucket Sort Sequential

Set 38 to true if you want to print
*/



// Header Files
#include <fstream>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "mpi.h"

using namespace std;

//function declarations
void multiply( int** matrix, int** otherM, int** finalM, int perfectSquare );
void print( int** finalM, int perfectSquare );
int main( int argc, char* argv[] )
{
    // Initialize Program/Variables
    double timeBegin = 0;
    double timeEnd = 0;
    double timeFinal = 0;
    ifstream fin;
    int index;
    int **matrix = NULL;
    int **otherM = NULL;
    int **finalM = NULL;

    bool printValues = false;

    int perfectSquare = 1800;

    //initialize so wtime can work
    MPI_Init( &argc, &argv );
    

    //create space for the buckets
    matrix = new int*[ perfectSquare ];
    otherM = new int*[ perfectSquare ];
    finalM = new int*[ perfectSquare ];
    for( index = 0; index < perfectSquare; index++ )
    {
        matrix[ index ] = new int[ perfectSquare ];
        otherM[ index ] = new int[ perfectSquare ];
        finalM[ index ] = new int[ perfectSquare ];
        for( int i = 0; i < perfectSquare; i++ )
        {
            matrix[ index ][ i ] = i + 2;
            otherM[ index ][ i ] = i + 2;
            finalM[ index ][ i ] = 0;
        }
    } 


    //start time
    timeBegin = MPI_Wtime();

    multiply( matrix, otherM, finalM, perfectSquare );
        
    //end time
    timeEnd = MPI_Wtime();
    timeFinal = timeEnd - timeBegin;

    cout << "timeFinal: " << timeFinal << endl;
    if( printValues )
    {
        print( finalM, perfectSquare );
    }

}




void multiply( int** matrix, int** otherM, int** finalM, int perfectSquare )
{
    int indexOut, index, indexIn;

    for( indexOut = 0; indexOut < perfectSquare; indexOut++ )
    {
        for( index = 0; index < perfectSquare; index++ )
        {
            for( indexIn = 0; indexIn < perfectSquare; indexIn++ )
            {
                finalM[ indexOut ][ index ] += matrix[ indexOut ][ indexIn ] * 
                                                    otherM[ indexIn ][ index ]; 
            }
        }
    }
}

void print( int** finalM, int perfectSquare )
{
    int indexOut, index;

    for( indexOut = 0; indexOut < perfectSquare; indexOut++ )
    {
        for( index = 0; index < perfectSquare; index++ )
        {
            cout << finalM[ indexOut ][ index ] << " ";
        }
        cout << endl;
    }


}


//how to put file into the headnode
// scp pa01.cpp jsugutian@h1.cse.unr.edu:pa01.cpp

//how to put folder into the headnode
// scp -r folder jsuguitan@h1.cse.unr.edu:folder

//how to compile
// mpic++ -lpmi -o pa01 pa01.cpp
// mpic++ -lpmi -o <executible> <cpp file>

//how to output file back to normal terminal
//scp filename ip_address:~

//how to run
// srun -n2 pa01
// srun -<number of nodes> <executible
// lowercse n makes it within the same box

// srun -N2 pa01
   
   
   










