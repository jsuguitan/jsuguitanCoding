/*
Jervyn Suguitan
3-28
Bucket Sort Helper: Creates Random Integers


*/
// Header Files
#include <fstream>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "mpi.h"

using namespace std;


int main( int argc, char* argv[] )
{
    ofstream fout;
    ifstream fin;
    int numValues;
    int index;
    int maxIndex;
    MPI_Init( &argc, &argv );
    
    srand( 0 );

    //read in file that has only 1 number
    fin.open( argv[ 1 ] );
    fin >> numValues;


    fin.close();
    
    //reopen the file
    fout.open( argv[ 1 ] );

    //input the number of values
    fout << numValues;
    fout << endl;


    maxIndex = numValues * numValues * 2;
    //input numVales worth of ints into the file
    for( index = 0; index < maxIndex ; index++ )
    {
        if( index == maxIndex / 2 )
        {
            fout << endl << endl << endl;
        }
        fout << ( rand() % 50 ) + 1 << " ";

        //fout << 2 << " ";

        //this if statement is used for spacing in the file
        if( index % 100 == 0 && index > 0 )
        {
            fout << endl;
        }

    } 
    
    fout.close();



}



