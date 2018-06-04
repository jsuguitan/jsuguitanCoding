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
void numToBucket( int*, int**, int*, int, int, int );
void sortBuckets( int*, int  );
void bubblesort( int[], int );
int main( int argc, char* argv[] )
{
    // Initialize Program/Variables
    double timeBegin = 0;
    double timeEnd = 0;
    double timeFinal = 0;
    int inputIndex = 0;
    ifstream fin;
    int numValues, numBuckets, index, biggestVal;
    int *holder;
    int **buckets = NULL;
    int *bucketIndex;
    int *finalHolder;
    bool printValues = true;

    //initialize so wtime can work
    MPI_Init( &argc, &argv );
    
    //biggest number in the sort
    biggestVal = 1000000;

    //read in file
    fin.open( argv[ 1 ] );
    
    //read in number which shows the number of ints 
    fin >> numValues;

    numBuckets = 10;    

    //declare dynamic space
    holder = new int[ numValues ];
    finalHolder = new int[ numValues ];
    bucketIndex = new int[ numBuckets ];


    //set how many ints per bucket to -1 which really represents 0 ints in a bucket
    for( index = 0; index < numBuckets; index++ )
    {
        bucketIndex[ index ] = -1;
    }

    //input all numbers into a holder/buffer
    for( index = 0; index < numValues; index++ )
    {
        fin >> holder[ index ];
    }

    //create space for the buckets
    buckets = new int*[ numBuckets ];
    for( index = 0; index < numBuckets; index++ )
    {
        buckets[ index ] = new int[ numValues ] ;
    }
    
    //start time
    timeBegin = MPI_Wtime();

    //bucketsort function
    numToBucket( holder, buckets, bucketIndex, numValues, biggestVal, numBuckets );
        
    //end time
    timeEnd = MPI_Wtime();
    timeFinal = timeEnd - timeBegin;

    fin.close();

    cout << "timeFinal: " << timeFinal << endl;
    



    //to see numbers sorted correctly uncomment the below cout statements
    for( int i = 0; i < numBuckets; i++ )
    {
        //+ 1 because bucketIndex holds where the last one is inputted
        for( index = 0; index < bucketIndex[ i ] + 1; index ++ )
        {
            finalHolder[ inputIndex ] = buckets[ i ][ index ];
            
            
            if( printValues )
            {
                //this if statement is just for readibilty and spacing
                cout << finalHolder[ inputIndex ] << " ";
                if( inputIndex % 50 == 0 )
                {
                    cout << endl;
                }
            }
            inputIndex++;
        }
        if( printValues )
        {
            cout << endl << endl;
        }    
    }

}




//the biggest value is 1000000
//holder contains all the numbers from the file
//buckets is the buckets
//bucketIndex has the number of ints in a bucket
//numValues has the number of ints overall
//biggestVal is the biggest possible int 
//numBuckets is the number of buckets
void numToBucket( int* holder, int** buckets, int* bucketIndex, int numValues, int biggestVal, int numBuckets )
{
    int index = 0;
    int currentVal;
    int currentBucket;
    int currentBucketIndex;

    //input each value into its correct bucket
    for( index = 0; index < numValues; index++ )
    {
        currentVal = holder[ index ];
        
        //determine working/currentBucket by using the currentVal
        currentBucket = currentVal / ( biggestVal / numBuckets );
        
        //based on the currentBucket state that there will be + 1 int in the bucketbucket
        bucketIndex[ currentBucket ] = bucketIndex[ currentBucket ] + 1;
        currentBucketIndex = bucketIndex[ currentBucket ];
    
        //input the current value into the correct bucket
        buckets[ currentBucket ][ currentBucketIndex ] = currentVal;
        
        //cout << currentBucket  << "CBI: " << currentBucketIndex << endl;
        
    }

    //sort the buckets
    for( index = 0; index < numBuckets; index++ )
    {
        sortBuckets( buckets[ index ], bucketIndex[ index ] );
    }
}


//sorts one bucket at a time
void sortBuckets( int* buckets, int bucketIndex )
{
    //if bucketIndex == -1 that means there are no ints in the bucket thus 
        //no need to sort anything
    if( bucketIndex == -1 )
    {
        return;
    }

    //sorting algorithm from the algorithm library
    //quicksort( buckets, index, bucketIndex - 1 );
    bubblesort( buckets, bucketIndex );
}

void bubblesort( int input[], int n )
{
    int index = 0;
    int holder;
    bool hasSwitched = true;
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
   
   
   










