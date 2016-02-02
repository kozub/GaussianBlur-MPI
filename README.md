# Gaussian Blur in parallel using MPI

#Requirements
    MPI installed

# Compilation:
    make


# Running:
    mpirun -n <n> ./gauss_mpi <input_image> <output_image>

    n - number of processes
    input_image - image to process
    output_image - result