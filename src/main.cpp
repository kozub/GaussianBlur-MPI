#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "mpi.h"
#include "handleError.h"
#include "gaussian.hpp"

using namespace std;
using namespace cv;

bool validateArgs(int argc, char *argv[]);
void saveResultToFile(char* fileName);

int main(int argc, char *argv[])
{
    int rank, numberOfTasks, imgRows, imgCols;
    int *send_counts, *rows_per_process, *displacements;
    double startTime, totalTime;
    Mat source;
    Mat* result = new Mat();

    HandleError(MPI_Init(&argc, &argv), 0);
    HandleError(MPI_Comm_rank(MPI_COMM_WORLD,&rank), 0);
    HandleError(MPI_Comm_size(MPI_COMM_WORLD,&numberOfTasks), rank);
    HandleError(MPI_Errhandler_set(MPI_COMM_WORLD,MPI_ERRORS_RETURN), rank);

    int numOfSlices = numberOfTasks -1;
    int colsPerProcess;

    // Before processing
    if (rank == 0) {

        if(!validateArgs(argc, argv)) {
            exit(EXIT_FAILURE);
        }

        if(numberOfTasks == 1) {
            fprintf(stderr, "%s should be running with at least 2 processes.\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        source = imread(argv[1], CV_LOAD_IMAGE_COLOR);
        imgCols = source.cols;
        imgRows = source.rows;
        result = new Mat(imgRows, imgCols, CV_8UC3);

        send_counts = new int[numOfSlices];
        displacements = new int[numOfSlices];
        rows_per_process = new int[numberOfTasks];

        for (int i = 0; i < numberOfTasks; i++) {
            send_counts[i] = displacements[i] = 0;
        }

        rows_per_process[0] = 0;
        int colSize = imgRows / numOfSlices;

        for (int i = 1; i < numberOfTasks; i++) {
            rows_per_process[i] = colSize;
            displacements[i] = displacements[i-1] + send_counts[i-1];
            send_counts[i] = colSize * imgCols * 3;
        }


        if(numOfSlices > 1) {
            int lastSliceSize = imgRows % (colSize * (numOfSlices - 1));

            if (lastSliceSize == 0) {
                lastSliceSize = colSize;
            }
            rows_per_process[numberOfTasks - 1] = lastSliceSize;
            displacements[numberOfTasks - 1] = displacements[numberOfTasks - 2] + send_counts[numberOfTasks - 2];
            send_counts[numberOfTasks - 1] = lastSliceSize * imgCols * 3;
        }
    }


    HandleError(MPI_Scatter(rows_per_process, 1, MPI_INT, &imgRows, 1, MPI_INT, 0, MPI_COMM_WORLD), rank);
    HandleError(MPI_Bcast(&imgCols, 1, MPI_INT, 0, MPI_COMM_WORLD), rank);
    HandleError(MPI_Barrier(MPI_COMM_WORLD), rank);

    Mat* slice = new Mat(imgRows, imgCols, CV_8UC3);
    HandleError(MPI_Scatterv(source.data, send_counts, displacements, MPI_BYTE,
                 slice->data, imgRows * imgCols * 3, MPI_BYTE, 0, MPI_COMM_WORLD), rank);

    // Processing
    startTime = MPI_Wtime();
    if (1 <= rank && rank < numberOfTasks) {
        GaussianBlur* gB = new GaussianBlur(slice);
        gB->process();

        slice = gB->getResult();
        free(gB);
    }

    HandleError(MPI_Gatherv(slice->data, imgRows * imgCols * 3, MPI_BYTE,
                result->data, send_counts, displacements, MPI_BYTE, 0, MPI_COMM_WORLD), rank);
    totalTime = MPI_Wtime() - startTime;
    HandleError(MPI_Finalize(), rank);

    //After processing
    if (rank == 0) {

        printf("Time: %fms\n", (totalTime*1000) );
        imwrite(argv[2], *result);

        delete [] send_counts;
        delete [] displacements;
    }
    delete slice;
    delete result;

    return 0;
}


bool validateArgs(int argc, char *argv[]    ){
    if(argc!=3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file> \n", argv[0]);
        return false;
    }

    struct stat st;
    int result = stat(argv[1], &st);
    if (result != 0) {
        fprintf(stderr, "Program can not open input file \n");
        return false;
    }
    return true;
}
void saveResultToFile(char* fileName, Mat result) {
    imwrite(fileName, result);
}
