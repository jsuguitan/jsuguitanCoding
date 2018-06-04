/*
Jervyn Suguitan
Mandelbrot Set
Note: To hardcode the size of the image change the ints width and height
width and height are found lines 54 and 55
To Change processor number you must also change dynamic.sh and change variable
workingProcessorAmount to # of specified in dynamic.sh
*/
// Header Files
#include "mpi.h"
#include <fstream>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include "PIMFuncs.h"

using namespace std;

// Global Constants
const double real_min = -2.0;
const double real_max = 2.0;
const double imag_min = -2.0;
const double imag_max = 2.0;

const int tag = 5;
const int FINISH_PROCESSORS = 100;

// Structs
struct complex
{
    float real;
    float imag;
};

// Function Prototypes
int cal_pixel( complex c );

// Main program
int main( int argc, char* argv[] )
   {
    // Initialize variables
    int myRank;
    int  index, rowNum;
    double scale_real, scale_imag;
    MPI_Status status;
    double totalTime, startTime, endTime;
    int workingRows[ 2 ];
    int rowStart, rowEnd;

    //change these hardcoded variables to change essense of code
    int numberOfProcessors = 24; // if this number changes from 24
                                 // you must change the sbatch -n value
    int workingProcessorAmount = 30;
    int width = 540;
    int height = 540;
    
    int numSlaveProcessors = numberOfProcessors - 1; 
    // Iniitialize MPI
    MPI_Init( &argc, &argv );
    
    // Get rank
    MPI_Comm_rank( MPI_COMM_WORLD, &myRank );
    

    // scale values
    scale_real = ( real_max - real_min ) / width;
    scale_imag = ( imag_max - imag_min ) / height;

    
    // Initialize row (used in slave and master)
    int row[ width + 1 ];
    
    // Master
    if( myRank == 0 )
    {
        // Variables
        unsigned char ** pixels;
        char * file;
        ofstream fout;
        int count;
        
        // Create the file
        file = new char[20];
    
        // create .ppm file
        strcpy( file, "dynamic.ppm" );
        fout.open( file );
        fout.close();
        
        // Creating an array of pixel pointers
        pixels = new unsigned char *[ height ];

        // Creating an array of pixels from the pixel points
        for( index = 0; index < height; index++ )
        {
            pixels[ index ] = new unsigned char[ width ];
        }
           
        // starting time
        startTime = MPI_Wtime();
        
        // the following code states how many rows is given to a single processor
        // rowStart is the beginning row in which the processor will start working
        // rowEnd is the last row the proessor will be working on
        rowStart = 0;
        rowEnd = workingProcessorAmount - 1;
    
        //these values will be put in a workingRow array of the size 2
        //the workingRows state which rows the processor will work on
        workingRows[ 0 ] = rowStart;
        workingRows[ 1 ] = rowEnd;

        count = 0;
        rowNum = rowStart;
       
        // Loop first distribution to all processors
        //if rowStart still in bounds it may fail
        for( index = 0; ( index < numSlaveProcessors ) && ( rowStart < height ); index++ )
        {
            // send workingRows which states which rows the processor will be 
                //working on
            //
            MPI_Send( &workingRows, 2, MPI_INT, index + 1, tag, MPI_COMM_WORLD ); 
            
            // incremement row num and count
            rowStart = rowEnd + 1;
            rowEnd = rowEnd + workingProcessorAmount;
            workingRows[ 0 ] = rowStart;
            workingRows[ 1 ] = rowEnd;
            rowNum = rowStart;
            count++;
        }

        // check for idle processors
        while( index < numSlaveProcessors )
        {
            // send the death tag to those processors
            MPI_Send( &workingRows, 2, MPI_INT, index + 1, FINISH_PROCESSORS, MPI_COMM_WORLD );
            index++;
        }
           
        // loop load balacning the work
        do
        {
            // receive a row from any processes
            MPI_Recv( &row, width + 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
            
            // decrement count 
            if( status.MPI_TAG == 7 )
            {
                count--;
            }
            //look here

            
            int holder = row [ 0 ];
            for( int specialIndex = 0; specialIndex < width; specialIndex++ )
            {   
                pixels[ holder ][ specialIndex ] = row[ specialIndex + 1 ];
            }
            
       
            // store the row in the 2d array
            
            
            // check that you havent finishied
            if( ( rowNum < height ) && ( status.MPI_TAG == 7 ) )
            {
                // send a row
                MPI_Send( &workingRows, 2, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD );
                
                // increment row and count
                rowStart = rowEnd + 1;
                rowEnd = rowEnd + workingProcessorAmount;
                workingRows[0] = rowStart;
                workingRows[1] = rowEnd;
                rowNum = rowStart;
                count++;
            }
               

        } while( count > 0 );

        // loop sending death to all
        for( index = 0; index < numSlaveProcessors; index++ )
        {
            MPI_Send( &workingRows, 2, MPI_INT, index + 1, FINISH_PROCESSORS, MPI_COMM_WORLD ); 
        }

           
        // get end time
        endTime = MPI_Wtime();
        
        // get total time
        totalTime = endTime - startTime;
        
        // display time
        cout << totalTime << endl;
        
        // display the image
        pim_write_black_and_white( file, width, height, (const unsigned char**) pixels );
    }
       
    // Slave function
    else
    {
        int x,y, color;
        complex c;
        
        // recieve the workingRow array
        MPI_Recv( &workingRows, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
        
        rowStart = workingRows[ 0 ];
        rowEnd = workingRows[ 1 ];
        
        // loop doing work
        
        while( ( status.MPI_TAG != FINISH_PROCESSORS ) && ( rowStart < height ) )
        {
            // set start and end
            rowStart = workingRows[ 0 ];
            rowEnd = workingRows[ 1 ];

            // loop across all rows to do
            for( y = rowStart; ( y < rowEnd + 1 ) && ( y < height ); y++ )
            {
                // store row number at first element
                row[ 0 ] = y;
            
                // loop doing the row
                for( x = 0, index = 1; x < width; x++, index++ )
                {

                    // calculate real
                    c.real = real_min + ( (float) x * scale_real );
                            
                    // calculate imag
                    c.imag = imag_min + ( (float) y * scale_imag );
                    
                  
                    // get the color to store
                    color = cal_pixel( c );
 
                    // store the color
                    row[index] = color;
                }
                  
                // send the row back
                if( y == rowEnd || y == height - 1 )
                {
                    MPI_Send( &row, width + 1, MPI_INT, 0, 7, MPI_COMM_WORLD );
                }
                   
                else
                {
                    MPI_Send( &row, width + 1, MPI_INT, 0, tag, MPI_COMM_WORLD );
                }
            } 
            
            // keep recieving new rows to work on once first initial rows are done
            MPI_Recv( &workingRows, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
        }
    }

    MPI_Finalize();
}
   
/**
  * @name calPixel
  *
  * @brief Caluclates a pixel
  *
  * @details Texbook calculation of a pixel
  *
  * @pre arguemnts must exist
  *
  * @post The iterations is returned
  *
  * @par Algorithm
  *      Use Textbook Equation
  *
  * @exception None
  *
  * @param[in] c
  *            Imaginary and real portions of c
  *
  * @return # of iterations
  *
  * @note This function is defined in the textbook
  */ 
int cal_pixel( complex c )
{
    int count, max_iter;
    complex z;
    float temp, lengthsq;
    
    // Set variables
    max_iter = 256;
    z.real = 0;
    z.imag = 0;
    count = 0;
    
    // Loop Iteration
    do
    {
        temp = z.real * z.real - z.imag * z.imag + c.real;
        
        z.imag = 2 * z.real * z.imag + c.imag;
        
        z.real = temp;
        
        lengthsq = z.real * z.real + z.imag * z.imag;
        
        count++;
    } while( ( lengthsq < 4.0 ) &&  ( count < max_iter ) );
       
    return count;
}


   



















   
   
