#include <stdio.h>

#ifndef HANDLEERROR_H
#define HANDLEERROR_H

#include "mpi.h"

static void HandleError( int errorCode,
                         int taskId ) {

    if (errorCode != MPI_SUCCESS) {

        char errorString[BUFSIZ];
        int lengthErrorString, errorClass;

        MPI_Error_class(errorCode, &errorClass);
        MPI_Error_string(errorClass, errorString, &lengthErrorString);
        MPI_Error_string(errorCode, errorString, &lengthErrorString);

        fprintf(stderr, "Error! Task: %d\nClass: %s\nCode: %d\n", taskId, errorString, errorCode);
        MPI_Abort(MPI_COMM_WORLD, errorCode);
    }
}

#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))

#endif
