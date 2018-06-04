/*
Jervyn Suguitan
4-25
Matrix Multiplication Parallel

//Main Function:
Master Reads in two matricies, then broadcasts the matricies into the slave processes
  The master tells each processor what section of the matrix they will be working on.
  The ith processor will deal with an ith part of the matrix.  All the processors
  shift submatricies initially then do matrix multiplication.  They continue to do this 
  a sqrt(processors) amount of times.  If the user wants prining change variable print to true.
  Also if printing occurs, the processors will send their resultant matrix back to the master 
  to print



Set print variable to true if you want to print
*/

// Header Files
#include <fstream>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <string>
#include "mpi.h"

using namespace std;

//Function Prototypes

//input matrix values
void getData( string fileOne, string fileTwo, int &numRows, int** &holder, int** &secHolder, int** &finalMatrix, int myRank );

//create space for 2D Matrix
void create2DMatrix( int** &holder, int value );


//figure out what each processor is working on within the two big matricies
void findWorkingRows( int workingValues[], int index, int workingRows, int proSquared );

//inputs a matrix properly into the correct place in a larger matrix
void inputArray( int** ( &matrix ), int indexBeginX, int indexBeginY, int indexEndX, 
                 int indexEndY, int** holder, bool isNormal );

//prints incoming matrix
void printMatrix( int **matrix, int workingRows );

//find which processor sends to the other processors.  THey each sub matrixA and sub matrixB
void findPart( int myRank, int& rowDest, int& rowSource, int& colDest, int& colSource, int numProcessors );

//this function does sub matrix movement between processors.  Acts as the cannons algorithm function 
//for rows
void move_MatrixRowOrCol( int** matrix, int workingRows, int proSquared, int myRank, int myDest, int mySource, bool init, bool isCol );

//acts as a helper for the move_MatrixRowOrCol function
void moveHelper( int** &matrix, int** &temp, int workingRows, int myDest, int mySource, int currentRow );

//zeroes out the complete matrix.  used for the sub matrixC
void zeroOut( int** &matrix, int workingRows );

//basic matrix multiplication using 3 matricies
void matrixMultiplication( int** &matrixA, int** &matrixB, int** &matrixC, int workingRows );

//moves all resulat sub matrixC's into the final Matrix. This is after all the main multiplication is done
void createResultMatrix( int** &finalMatrix, int** &matrixC, int workingRows, int numProcessors );

//main function
int main( int argc, char* argv[] )
{
    // Initialize Program/Variables
    double timeBegin = 0;
    double timeEnd = 0;
    double timeFinal = 0;

    ifstream fin;
    int numRows, index, myRank, numProcessors, workingRows;
    int indexBeginX, indexEndX, indexBeginY, indexEndY = 0; 
    int rowDest, colDest, rowSource, colSource, proSquared;
    string fileOne, fileTwo;
    bool print = true;
 
    //main Matricies and sub Matricies
    int** holder;
    int** secHolder;
    int** matrixA;
    int** matrixB;
    int** matrixC;
    int** finalMatrix;

    int workingValues[ 4 ];
    int tag = 5;
             
    MPI_Status status;

    //initialize so wtime can work
    MPI_Init( &argc, &argv );
    
    // Get rank
    MPI_Comm_rank( MPI_COMM_WORLD, &myRank );

    //totalProcessors
    MPI_Comm_size( MPI_COMM_WORLD, &numProcessors );

    
    //set fileOne
    fileOne = argv[ 1 ];
    fileTwo = argv[ 2 ];
    
    //readin File, create space for the main matricies and input data from file
    getData( fileOne, fileTwo, numRows, holder, secHolder, finalMatrix, myRank );

    //read in how many processors squared there are
    proSquared = sqrt( numProcessors );

    //how many columns/rows each processors is working On
    //acts as the column and rows
    workingRows = numRows / proSquared;

    //space creation
    create2DMatrix( matrixA, workingRows );
    create2DMatrix( matrixB, workingRows );
    create2DMatrix( matrixC, workingRows );

    
    //zero out all sub matricies that are the resultant, matrix A * matrixB = matrixC    
    zeroOut( matrixC, workingRows );
    
    //splits how many rows/columns each processor is doing.

    workingValues[ 0 ] = 0;                 //what is the starting column
    workingValues[ 1 ] = workingRows;       //ending column
    workingValues[ 2 ] = 0;                 //starting row
    workingValues[ 3 ] = workingRows;       //ending row


    //figure out where to send sub matricies function
    findPart( myRank, rowDest, rowSource, colDest, colSource, numProcessors );

    //master 1
    if( myRank == 0 )
    {
        //function that sends out which processor will do what
        for( index = 1; index < numProcessors; index++ )
        {
            findWorkingRows( workingValues, index, workingRows, proSquared );
            MPI_Send( &workingValues, 4, MPI_INT, index, tag, MPI_COMM_WORLD );  
        }

        //state what the master matrix will work on
        indexBeginX = 0;
        indexEndX = workingRows;
        indexBeginY = 0;
        indexEndY = workingRows;                

        //input the data into the sub matricies using the two main matricies - holder and secHolder
        //matrixA and matrixB are the sub matricies
        inputArray( matrixA, indexBeginX, indexBeginY, indexEndX, indexEndY, holder, true );
        inputArray( matrixB, indexBeginX, indexBeginY, indexEndX, indexEndY, secHolder, true );

        //wait until all processors put in the correct data
        MPI_Barrier( MPI_COMM_WORLD );
    
        //start time
        timeBegin = MPI_Wtime(); 
    }

    //slave 1
    else
    {
        //recieve the two main matricies
        for( index = 0; index < numRows; index++ )
        {
            MPI_Bcast( holder[ index ], numRows, MPI_INT, 0, MPI_COMM_WORLD );
            MPI_Bcast( secHolder[ index ], numRows, MPI_INT, 0, MPI_COMM_WORLD );
        }

        //recieve what part of the main matrix that this specific processor will work from
        //this data comes from master
        MPI_Recv( &workingValues, 4, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
        
        indexBeginX = workingValues[ 0 ]; 
        indexEndX = workingValues[ 1 ]; 
        indexBeginY = workingValues[ 2 ];
        indexEndY = workingValues[ 3 ]; 

        //input the data into the sub matricies using the two main matricies - holder and secHolder
        //matrixA and matrixB are the sub matricies
        inputArray( matrixA, indexBeginX, indexBeginY, indexEndX, indexEndY, holder, true );
        inputArray( matrixB, indexBeginX, indexBeginY, indexEndX, indexEndY, secHolder, true );

        MPI_Barrier( MPI_COMM_WORLD );
        
    }

    //post barrier work    
    //initialize to do multiplication correctly
    //this function uses true since it is actually initializing
    move_MatrixRowOrCol( matrixA, workingRows, proSquared, myRank, rowDest, rowSource, true, false );
    move_MatrixRowOrCol( matrixB, workingRows, proSquared, myRank, colDest, colSource, true, true );


    //loop through multiplying matricies and moving the sub matricies
    for( index = 0; index < proSquared; index++ )
    {
        matrixMultiplication( matrixA, matrixB, matrixC, workingRows );
        
        //the false makes it slightly different from the initializing move
        move_MatrixRowOrCol( matrixA, workingRows, proSquared, myRank, rowDest, rowSource, false, false );
        move_MatrixRowOrCol( matrixB, workingRows, proSquared, myRank, colDest, colSource, false, true );
    }

    //wait until all processors are here
    //all processors are doing the multiplication
    MPI_Barrier( MPI_COMM_WORLD );
    
    //finish timing
    if( myRank == 0 )
    {
        timeEnd = MPI_Wtime();
        timeFinal = timeEnd - timeBegin;
        cout << "timeFinal: " << timeFinal << endl;
        
        //put all sub matricies into the final Matrix
        createResultMatrix( finalMatrix, matrixC, workingRows, numProcessors );             
        
    }
    else
    {
        //send the matrixC (resulatant matrix) to the main
        for( index = 0 ; index < workingRows; index++ )
        {
            MPI_Send( matrixC[ index ], workingRows, MPI_INT, 0, tag, MPI_COMM_WORLD );
        }
    }
    
    //prints final matrix
    if( print && myRank == 0 )
    {
        cout << "Final Result" << endl;
        printMatrix( finalMatrix, numRows );

        cout << endl << endl << "For more information about MatrixA and MatrixB Please Open up read in file" << endl;
    }

    MPI_Finalize();

} //end of main

//////////////////////////////////////////////////////////////////////////////////////////////////////







/*
Function that reads in a file, create space for the main matricies, and input
    Data into those matricies.  The master will send these matricies to the 
    slaves using MPI_Bcast.  They will send a row at a time
*/

void getData( string fileOne, string fileTwo, int &numRows, int** &holder, 
              int** &secHolder, int** &finalMatrix, int myRank 
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

    // only do this work if it is master
    if( myRank == 0 )
    {
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
     
        //broadcast the two main matricies a row at a time to all processors
        for( index = 0; index < numRows; index++ )
        {
            MPI_Bcast( holder[ index ], numRows, MPI_INT, 0, MPI_COMM_WORLD );
            MPI_Bcast( secHolder[ index ], numRows, MPI_INT, 0, MPI_COMM_WORLD );
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
figure out where each processor is working on within the two matricies
*/
void findWorkingRows( int workingValues[], int index, int workingRows, int proSqaured )
{
    if( index % proSqaured == 0 )
    {   
        // columns 
        workingValues[ 0 ] = 0;
        workingValues[ 1 ] = workingRows;
        
        // rows
        workingValues[ 2 ] = workingValues[ 3 ];
        workingValues[ 3 ] = workingValues[ 3 ] + workingRows;
        return;
    }
    workingValues[ 0 ] = workingValues[ 1 ];
    workingValues[ 1 ] = workingValues[ 1 ] + workingRows;    
    

}


/*
inputs holder and secHolder ( the two main matricies ) properly into the correct
the correct sub matricies
*/

void inputArray( int** ( &matrix ), int indexBeginX, int indexBeginY, int indexEndX, 
                 int indexEndY, int** holder, bool isNormal )
{
    int index, indexIn, xVal, yVal = 0;

    //case where breaking large matrix up and putting in small matrix
    if( isNormal )
    {
        for( index = indexBeginY, yVal = 0; index < indexEndY; index++, yVal++ )
        {
                
            for( indexIn = indexBeginX, xVal = 0; indexIn < indexEndX; indexIn++, xVal++ )
            {
                matrix[ yVal ][ xVal ] = holder[ index ][ indexIn ];

            } 
        }
    }

    //case occurs when printing.  Putting result sub matricies into main matrixC
    else 
    {
        for( index = indexBeginY, yVal = 0; index < indexEndY; index++, yVal++ )
        {
                
            for( indexIn = indexBeginX, xVal = 0; indexIn < indexEndX; indexIn++, xVal++ )
            {
                matrix[ index ][ indexIn ] = holder[ yVal ][ xVal ];
            } 
        }        
    }

}






/*
print the matricies with the workingRows being the number of rows and columns
*/
void printMatrix( int **matrix, int workingRows )
{
    cout << "In printMatrix" << endl;
    int index, indexIn = 0;
    for( index = 0; index < workingRows; index++ )
    {
        for( indexIn = 0; indexIn < workingRows; indexIn++ )
        {
            cout << " " << matrix[ index ][ indexIn ];
        }
        cout << endl << endl << endl;
    }
}


/*
    figures out where each processors moves the sub matricies
*/
void findPart( int myRank, int& rowDest, int& rowSource, int& colDest, int& colSource, int numProcessors )
{
    //initialize how many parts are for each row
    int proSquared = sqrt( numProcessors );
    int currentRow = myRank / sqrt( numProcessors );

    rowDest = myRank - 1;
    rowSource = myRank + 1;
    
    if( rowDest < 0 || ( rowDest / proSquared ) != currentRow )
    {
        rowDest += proSquared;
    }
       
    if( rowSource / proSquared != currentRow )
    {
        rowSource -= proSquared;
    }
    
    colDest = myRank - proSquared;
    colSource = myRank + proSquared;

    if( colDest < 0 )
    {
        colDest += numProcessors;
    }
       
    if( colSource >= numProcessors )
    {
        colSource -= numProcessors;
    }
}

/*
    move the row to the correct processors, the moves are differents if it init is true or false
    if it is true that means those are the initital cannon algorthm steps
    false means it is after the cannon algorithm steps
*/
void move_MatrixRowOrCol( int** matrix, int workingRows, int proSquared, int myRank, int myDest, int mySource, bool init, bool isCol )
{

    int** temp;
    int index, currentRow;

    //create space for buffer matrix
    create2DMatrix( temp, workingRows );

    //determine if working with columns or rows
    if( isCol )
    {
        currentRow = myRank % proSquared;
    }
    else
    {
        currentRow = myRank / proSquared;
    }        

    //determing if this is the initialization move or not
    if( init )
    {
        for( index = 0; index < currentRow; index++ )
        {
            //function that actual moves data from one processor to another
            moveHelper( matrix, temp, workingRows, myDest, mySource, currentRow );
        }
    }
    else //normal move
    {
        //function that actual moves data from one processor to another
        moveHelper( matrix, temp, workingRows, myDest, mySource, currentRow );

    }

}


void moveHelper( int** &matrix, int** &temp, int workingRows, int myDest, int mySource, int currentRow )
{
    int indexIn, inputIndex, innerIndex;
    MPI_Request request;
    MPI_Status status, waitStatus;   

    //sending 1 Row at a time
    for( indexIn = 0; indexIn < workingRows; indexIn++ )
    {
        MPI_Isend( matrix[ indexIn ], workingRows, MPI_INT, myDest, currentRow, MPI_COMM_WORLD, &request );
    }

    //recieveing 1 row at a time
    for( indexIn = 0; indexIn < workingRows; indexIn++ )
    {
                MPI_Recv( temp[ indexIn ], workingRows, MPI_INT, mySource, currentRow, MPI_COMM_WORLD, &status );
    }

    MPI_Wait( &request, &waitStatus );
        
    //rows
    for( inputIndex = 0; inputIndex < workingRows; inputIndex++ )
    {   
        //columns
        for( innerIndex = 0; innerIndex < workingRows; innerIndex++ )
        {
            matrix[ inputIndex ][ innerIndex ] = temp[ inputIndex ][ innerIndex ];
        }
    }
            


}



/*
    multiply the matricies
*/
void matrixMultiplication( int** &matrixA, int** &matrixB, int** &matrixC, int workingRows )
{
    int indexOut, index, indexIn = 0;

    for( indexOut = 0; indexOut < workingRows; indexOut++ )
    {
        for( index = 0; index < workingRows; index++ )
        {
            for( indexIn = 0; indexIn < workingRows; indexIn++ )
            {
                matrixC[ indexOut ][ index ] += matrixA[ indexOut ][ indexIn ] * 
                                                matrixB[ indexIn ][ index ];
            }
        }
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
/*
    put the submatricies from all the processors into the finalMatrix
*/
void createResultMatrix( int** &finalMatrix, int** &matrixC, int workingRows, int numProcessors )
{
    int index, indexBeginY = 0, indexIn, indexRecv = 0;
    int indexBeginX = 0;    
    int proSqaured = sqrt( numProcessors );
    int indexEndY = workingRows;
    int indexEndX = workingRows;
    int** holder;
    MPI_Status status;

    //create space for holder
    create2DMatrix( holder, workingRows );

    //input master subMatrix to finalMatrix
    for( index = 0; index < workingRows; index++ )
    {
        for( indexIn = 0 ; indexIn < workingRows; indexIn++ )
        {
            finalMatrix[ index ][ indexIn ] = matrixC[ index ][ indexIn ];
        }
    }

    //loops through all processors except the master
    for( indexRecv = 1; indexRecv < numProcessors; indexRecv++ )
    {
        //recieve a processors resultant sub matrix row at a time
        for( indexIn = 0; indexIn < workingRows; indexIn++ )
        {
            //master processor recieves a result matrix one row a time
            MPI_Recv( holder[ indexIn ], workingRows, MPI_INT, indexRecv, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
        }

//algorithm to figure out where to input into the finalMatrix
////////////////
        indexBeginX += workingRows;
        indexEndX += workingRows;

        if( indexRecv % proSqaured == 0 )
        {
            indexBeginY += workingRows; 
            indexEndY += workingRows;
            indexBeginX = 0;
            indexEndX = workingRows;
        }   
////////////////

        //input into the finalMatrix correctly
        // it is set to false because sub matrix is put into the final matrix       
        inputArray( finalMatrix, indexBeginX, indexBeginY, indexEndX, indexEndY, holder, false );

        
    }

}











