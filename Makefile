default: all

all:
	mpicxx -o gauss_mpi src/main.cpp src/gaussian.cpp `pkg-config --libs opencv`

clean:
	rm gauss_mpi
