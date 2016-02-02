# Gaussian Blur in parallel using MPI

    Input image is splitted into n slices and each of slice is processed by independent process.

#Requirements
    MPI installed

# Compilation:
    make

# Running:
    mpirun -n <n> ./gauss_mpi <input_image> <output_image>

    n - number of processes
    input_image - image to process
    output_image - result

# Example:

    run:
        mpirun -n 4 ./gauss_mpi clearImage.jpg bluredImage.jpg

    result:
        Time: 5850.651ms
