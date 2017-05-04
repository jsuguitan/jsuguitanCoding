/*
Jervyn Suguitan
3-28
Bucket Sort Sequential

Set printValues to true if you want to print final matrix
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

//input matrix values
void getData( string fileOne, string fileTwo, int &numRows, int** &holder, int** &secHolder, int** &finalMatrix );

//zeroes out the complete matrix.  used for the sub matrixC
void zeroOut( int** &matrix, int workingRows );

//create space for 2D Matrix
void create2DMatrix( int** &holder, int value );

// multiples matrixA and matrixB and the result will be in matrixC
void multiply( int** matrix, int** otherM, int** finalM, int numRows );

//print given matirx
void print( int** finalM, int numRows );


int main( int argc, char* argv[] )
{
    // Initialize Program/Variables
    double timeBegin = 0;
    double timeEnd = 0;
    double timeFinal = 0;
    int **matrix = NULL;
    int **otherM = NULL;
    int **finalM = NULL;
    string fileOne = argv[ 1 ];
    string fileTwo = argv[ 2 ];

    bool printValues = true;

    int numRows;

    //initialize so wtime can work
    MPI_Init( &argc, &argv );
    
//readin File, create space for the main matricies and input data from file
    getData( fileOne, fileTwo, numRows, matrix, otherM, finalM );


    //start time
    timeBegin = MPI_Wtime();

    multiply( matrix, otherM, finalM, numRows );
        
    //end time
    timeEnd = MPI_Wtime();
    timeFinal = timeEnd - timeBegin;

    cout << "timeFinal: " << timeFinal << endl;
    if( printValues )
    {
        print( finalM, numRows );

        cout << endl << endl << "For more information about MatrixA and MatrixB Please Open up read in file" << endl;
    }

}



// matrix multiplication code
void multiply( int** matrix, int** otherM, int** finalM, int numRows )
{
    int indexOut, index, indexIn;

    for( indexOut = 0; indexOut < numRows; indexOut++ )
    {
        for( index = 0; index < numRows; index++ )
        {
            for( indexIn = 0; indexIn < numRows; indexIn++ )
            {
                finalM[ indexOut ][ index ] += matrix[ indexOut ][ indexIn ] * 
                                                    otherM[ indexIn ][ index ]; 
            }
        }
    }
}

void print( int** finalM, int numRows )
{
    int indexOut, index;

    for( indexOut = 0; indexOut < numRows; indexOut++ )
    {
        for( index = 0; index < numRows; index++ )
        {
            cout << finalM[ indexOut ][ index ] << " ";
        }
        cout << endl << endl << endl;
    }


}


/*
Function that reads in a file, create space for the main matricies, and input
    Data into those matricies.  The master will send these matricies to the 
    slaves using MPI_Bcast.  They will send a row at a time
*/

void getData( string fileOne, string fileTwo, int &numRows, int** &holder, 
              int** &secHolder, int** &finalMatrix
            )
{
    int buffer, index, indexIn;
    fstream fin;
    fstream finB;

    //open file
    fin.open( fileOne.c_str() );
    finB.open( fileTwo.c_str() );

    //input numRows
    fin >> numRows;
    finB >> numRows;

    //create space for main matricies
    create2DMatrix( holder, numRows );
    create2DMatrix( secHolder, numRows );
    create2DMatrix( finalMatrix, numRows );

    zeroOut( finalMatrix, numRows );

    for( index = 0; index < numRows; index++ )
    {
        for( indexIn = 0; indexIn < numRows; indexIn++ )
        {
            //but into a buffer and input into Main matrixA
            fin >> buffer;
            holder[ index ][ indexIn ] = buffer;
        }
    }

    for( index = 0; index < numRows; index++ )
    {
        for( indexIn = 0; indexIn < numRows; indexIn++ )
        {
            //but into a buffer and input into both Main matrixB
            finB >> buffer;
            secHolder[ index ][ indexIn ] = buffer;
        }
    }
     

}


/*
Creates space for a 2D Matrix with the first argument the double pointer
    which we are working on, and the value being how large the matrix side will be
*/
void create2DMatrix( int** &holder, int value )
{
    //create space for rows
    holder = new int*[ value ];

    //create space for columns
    for( int i = 0; i < value; i++ )
    {
        holder[ i ] = new int[ value ];
    }
    
}

/*
    put zeroes out the whole matrix
*/
void zeroOut( int** &matrix, int workingRows )
{
    int index, indexIn;

    for( index = 0; index < workingRows; index++ )
    {
        for( indexIn = 0; indexIn < workingRows; indexIn++ )
        {
            matrix[ index ][ indexIn ] = 0;
        }
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
//scp fileOne ip_address:~

//how to run
// srun -n2 pa01
// srun -<number of nodes> <executible
// lowercse n makes it within the same box

// srun -N2 pa01
   
   
   










