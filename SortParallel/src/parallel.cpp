/*
Jervyn Suguitan
4-5
Bucket Sort Parallel

Set line 39 to true if you want to print
*/

// Header Files
#include <fstream>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "mpi.h"

using namespace std;

//Function Prototypes
void findRange( int rangeHolder[], int numBuckets, int numValues );
void sendWork( int numBuckets, int numValues, int &valueBegin, int &valueEnd );
void numToSingleBucket( int* holder, int** bucketHolder, int* rangeHolder, int valueBegin,  int valueEnd, int numBuckets );
void numToSingleBucketHelper( int workingVal, int** bucketHolder, int currentRangeIndex, int numBuckets );
void bubblesort( int input[], int n );

//main function
int main( int argc, char* argv[] )
{
    // Initialize Program/Variables
    double timeBegin = 0;
    double timeEnd = 0;
    double timeFinal = 0;

    ifstream fin;
    int numValues, index, sizeOfBucket, myRank;
    int valueBegin, valueEnd; 
    int numBuckets, remainder, totalValues;
    int first, second = 0;
    bool print = true;
 
    int *holder;
    int workingValues[ 2 ];
    int *rangeHolder;
    int tag = 5;
    int biggestVal = 1000000;
        
    int *bigArray;
    int *extraIndexArray;
    int **bucketHolder;
    int **extraBucket;      
    MPI_Status status;
    MPI_Request request;

    //initialize so wtime can work
    MPI_Init( &argc, &argv );
    
    // Get rank
    MPI_Comm_rank( MPI_COMM_WORLD, &myRank );

    fin.open( argv[ 1 ] );
    fin >> numValues;
    numBuckets = atoi( argv[ 2 ] );

    //create space for dynamic variables
    extraIndexArray = new int[ numBuckets ];
    holder = new int[ numValues ];
    rangeHolder = new int[ numBuckets * 2 ];

    //there will be 1 more buckets than needed as the extra bucket will hold 
    //how many ints are in each bucket
    bucketHolder = new int*[ numBuckets + 1];
    extraBucket = new int*[ numBuckets + 1 ];
    bigArray = new int[ numValues + 1 ];

    for( int ind = 0; ind < numBuckets + 1; ind++ )
    {
        bucketHolder[ ind ] = new int[ numValues ];
        extraBucket[ ind ]  = new int[ numValues ];
        
    }    

    //prime the extra bucket to hold the amount of numbers in a bucket
    for( index = 0; index < numBuckets; index++ )
    {
        bucketHolder[ numBuckets ][ index ] = 0;

    }
///////////////////////

    //input all data into a holder/buffer
    for( index = 0; index < numValues; index++ )
    {
        fin >> holder[ index ];
    }


    totalValues = numValues / numBuckets;
    remainder = numValues % numBuckets;
    valueBegin = 0;
    valueEnd = totalValues;
    remainder = numValues % numBuckets;

    //master 1 function
    if( myRank == 0 )
    {
        // find the range and put it in the array
        findRange( rangeHolder, numBuckets, biggestVal );
        
        //broadcast range
        MPI_Bcast( rangeHolder, numBuckets * 2, MPI_INT, 0, MPI_COMM_WORLD );

        //algorithm that sends what each processor will work with
        for( index = 0; index < numBuckets; index++ )
        {
            if( remainder > 0 )
            {
                valueEnd++;
                remainder--;
            }
    
            workingValues[ 0 ] = valueBegin;
            workingValues[ 1 ] = valueEnd;            

            if( index == 0 )
            {
                first = workingValues[ 0 ];
                second = workingValues[ 1 ];
            }
            else
            {
                //extra function that figures out how much data goes to each processor
                sendWork( numBuckets, numValues, valueBegin, valueEnd );
                workingValues[ 0 ] = valueBegin;
                workingValues[ 1 ] = valueEnd;
                
                //send what part of the buffer each index works with
                MPI_Send( &workingValues, 2, MPI_INT, index, tag, MPI_COMM_WORLD );  
            }
        }

        //what the master is working with
        workingValues[ 0 ] = first;
        workingValues[ 1 ] = second;
        
        
        MPI_Barrier( MPI_COMM_WORLD );
    }
    else
    {

        // recieve ranges
        MPI_Bcast( rangeHolder, numBuckets * 2, MPI_INT, 0, MPI_COMM_WORLD );
        
        // recieve what will the slave work on
        MPI_Recv( &workingValues, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
        
        // these values state from where the slave will work on
        valueBegin = workingValues[ 0 ];
        valueEnd = workingValues[ 1 ];

        MPI_Barrier( MPI_COMM_WORLD );
        
    }
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////

    //master 2
    if( myRank == 0 )
    {
        sizeOfBucket = 0;

        // start Time
        timeBegin = MPI_Wtime();

        //input values into sub buckets
        numToSingleBucket( holder, bucketHolder, rangeHolder, valueBegin, valueEnd, numBuckets  );

        // Send the sub buckets to the correct slave
        for( index = 0; index < numBuckets; index++ )
        {
            tag = bucketHolder[ numBuckets ][ index ];
            MPI_Isend( bucketHolder[ index ], tag, MPI_INT, index, tag, MPI_COMM_WORLD, &request );          
        }

        //recieve the correct bucket and input how many numbers each bucket recieved into extraIndexArray
        for( index = 0; index < numBuckets; index++ )
        {
            MPI_Recv( extraBucket[ index ], numValues + 1, MPI_INT, index, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
            extraIndexArray[ index ] = status.MPI_TAG; 
        } 

        //input all the sub buckets into a large "bigArray" which will hold all the numbers from the correct range into a single bucket
        int specialIndex = 0;
        for( index = 0; index < numBuckets; index++ )
        {
            sizeOfBucket += extraIndexArray[ index ];
            for( int i = 0; i < extraIndexArray[ index ]; i++ )
            {
                bigArray[ specialIndex ] = extraBucket[ index ][ i ];
                specialIndex++;
            }
        }


        //sort
        bubblesort( bigArray, sizeOfBucket );
        MPI_Barrier( MPI_COMM_WORLD );

        //once all buckets are sorted find time
        timeEnd = MPI_Wtime();
        timeFinal = timeEnd - timeBegin;

        cout << "Final Time: " << timeFinal << endl;

        if( print )
        {
            int **extraHolder;
            extraHolder = new int*[ numBuckets ];
            for( int i = 0; i < numBuckets; i++ )
            {
                extraHolder[ i ] = new int[ numValues ];
            }

            //cout << "master about to send " << endl;
            MPI_Isend( bigArray, sizeOfBucket, MPI_INT, 0, sizeOfBucket, MPI_COMM_WORLD, &request ); 
            for( int i = 0; i < numBuckets; i++ )
            {
                MPI_Recv( extraHolder[ i ], numValues + 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
                extraIndexArray[ index ] = status.MPI_TAG; 
            }
        
            for( int i = 0; i < numBuckets; i++ )
            {
                for( int j = 0; j < extraIndexArray[ i ]; j++ )
                {
                    cout << extraHolder[ i ][ j ] << " ";
                    if( j % 50 == 0 )
                    {
                        cout << endl;
                    }
                }
                cout << endl;
            }

            
        }
        

    }
    //slave 2
    else
    {
        sizeOfBucket = 0;
        numToSingleBucket( holder, bucketHolder, rangeHolder, valueBegin, valueEnd, numBuckets  );

        for( index = 0; index < numBuckets; index++ )
        {
            tag = bucketHolder[ numBuckets ][ index ];
            MPI_Isend( bucketHolder[ index ], tag, MPI_INT, index, tag, MPI_COMM_WORLD, &request );          
        }

        for( index = 0; index < numBuckets; index++ )
        {
            
            MPI_Recv( extraBucket[ index ], numValues + 1, MPI_INT, index, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

            extraIndexArray[ index ] = status.MPI_TAG; 
        }


        int specialIndex = 0;
        for( index = 0; index < numBuckets; index++ )
        {
            sizeOfBucket += extraIndexArray[ index ];
            for( int i = 0; i < extraIndexArray[ index ]; i++ )
            {
                bigArray[ specialIndex ] = extraBucket[ index ][ i ];
                specialIndex++;
            }
        }    

        bubblesort( bigArray, sizeOfBucket );
    
        MPI_Barrier( MPI_COMM_WORLD );

        if( print )
        {
            MPI_Isend( bigArray, sizeOfBucket, MPI_INT, 0, sizeOfBucket, MPI_COMM_WORLD, &request );          
        }

    }

    MPI_Finalize();

}

void sendWork( int numBuckets, int numValues, int &valueBegin, int &valueEnd )
{
    
    int totalValWork = numValues / numBuckets;

    valueBegin = valueEnd + 1;
    valueEnd = valueEnd + totalValWork;    
}

void findRange( int rangeHolder[], int numBuckets, int biggestVal )
{
    int index;
    int rangeBegin;
    int rangeEnd;
    int work;

    work = biggestVal / numBuckets;

    rangeBegin = 0;
    rangeEnd = work;

    for( index = 0; index < numBuckets * 2; index += 2 )
    {
        rangeHolder[ index ] = rangeBegin;
        rangeHolder[ index + 1 ] = rangeEnd;

        rangeBegin = rangeEnd + 1;
        rangeEnd = rangeEnd + work;

    }
    
    rangeHolder[ index + 1 ] = biggestVal;

}
 




void numToSingleBucket
    ( 
        int* holder, 
        int** bucketHolder, 
        int* rangeHolder, 
        int valueBegin, 
        int valueEnd, 
        int numBuckets
    )
{
    
    int currentRangeIndex = ( numBuckets - 1 ) * 2;
    int workingVal;
    
    int findBucket = false;
    int index;



    //from the first section of the Holder to 
    //ast part of the data the processor works on
    for( index = valueBegin; index < valueEnd + 1; index++ )
    {
        //input the holder value into a workingValue
        workingVal = holder[ index ];
            
        while( !findBucket )
        {
            //if the workingVal is between the given range
            if( workingVal >= rangeHolder[ currentRangeIndex ] )
            {
                //input the workingValue into the proper sub bucket
                numToSingleBucketHelper( workingVal, bucketHolder, currentRangeIndex, numBuckets );              

                //reset currentRangeIndex after inputting the number into the correct sub bucket
                currentRangeIndex = ( numBuckets - 1 ) * 2;
                findBucket = true; 
            }
            else
            {
                currentRangeIndex -= 2;
                //testing
                if( currentRangeIndex < 0 )
                {
                    cout << "ERROR" << endl;
                }
            }
                
        }
        findBucket = false;   
    }
}

void numToSingleBucketHelper
    ( 
        int workingVal, 
        int** bucketHolder,  
        int currentRangeIndex,
        int numBuckets
    )
{
    int workingBucket = currentRangeIndex / 2;
    int workingIndex;
    
    

    workingIndex = bucketHolder[ numBuckets ][ workingBucket ];
    bucketHolder[ workingBucket ][ workingIndex ] = workingVal;
    bucketHolder[ numBuckets ][ workingBucket ]++;
   
}




/*
Function: Sort number

Arguments:

*/
void bubblesort( int input[], int n )
{
    int index = 0;
    int holder;
    bool hasSwitched = true;

    //if hasSwitched is true that means keep sorting
    //else it is sorted
    while( hasSwitched )
    {
        hasSwitched = false;
        for( index = 0; index < n - 1; index++ )
        {
            if( input[ index ] > input[ index + 1 ] )
            {
                holder = input[ index + 1 ];
                input[ index + 1 ] = input[ index ];
                input[ index ] = holder;
                hasSwitched = true;
            }
        }
    }
}

























