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

    MPI_Init( &argc, &argv );

    //read in file that has only 1 number
    fin.open( argv[ 1 ] );
    fin >> numValues;


    fin.close();
    
    //reopen the file
    fout.open( argv[ 1 ] );

    //input the number of values
    fout << numValues;
    fout << endl;

    //input numVales worth of ints into the file
    for( index = 0; index < numValues; index++ )
    {
        fout << rand() % 10000 << " ";

        //this if statement is used for spacing in the file
        if( index % 20 == 0 )
        {
            fout << endl;
        }

    } 
    
    fout.close();



}



