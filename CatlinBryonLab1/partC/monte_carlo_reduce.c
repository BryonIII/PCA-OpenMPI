/*Bryon Catlin monte_carlo.c */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */
#define TIME_FROM_WORKER 3     /* setting a message type */

double estimate_g(double lower_bound, double upper_bound, long long int N);
void collect_results(double *result);
void init_rand_seed();

static double g_worker_start_time = 0.0;

int main(int argc, char **argv) { 
	double result = 0.0; 
	MPI_Init(&argc, &argv); 
	
	float lower_bound  = atof(argv[1]); 
	float upper_bound  = atof(argv[2]); 
	long long int N = atof(argv[3]); 
	
	init_rand_seed(); // using srand() 
	result = estimate_g(lower_bound, upper_bound, N); 
	collect_results(&result); 
	
	MPI_Finalize(); 
	return 0; 
}

void init_rand_seed(){
	int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// add rank to ensure each worker uses a different seed	
	srand(time(0)+rank);
}

void collect_results(double *result){
        int rank, size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;

	double total, start_time, end_time;

	// receive and reduce all results into one sum
	MPI_Reduce(result, &total, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
	if(rank == MASTER) end_time = MPI_Wtime(); // stop timer immediately after last receive
	
	// receive all worker times
	MPI_Reduce(&g_worker_start_time, &start_time, 1, MPI_DOUBLE, MPI_MIN, MASTER, MPI_COMM_WORLD);

	printf("Worker %d's result is: %.8f\n", rank, *result);
	printf("Worker %d's start_time is: %.8f\n", rank, start_time);
	/* ----------- master node ----------- */
        if(rank == MASTER){
		*result = total;

		// print final results and time spent
		printf("The end time is: %.8f\n", end_time);
		printf("The result is %.8f\n", *result);
		printf("Time spent: %.8f seconds\n", end_time-start_time);
        }
}

double estimate_g(double lower_bound, double upper_bound, long long int N){
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	long long localN, aveN, remN;
	double partial, local_sum, u, x;
	/* ----------- master node ----------- */
	if(rank == MASTER){
        	local_sum = 0.0;
		
		// split data
		aveN = N/size;
		remN = N%size;
		localN = aveN + (rank < remN ? 1 : 0);

		g_worker_start_time = MPI_Wtime(); // start master worker timer

		// perform monte_carlo calculations
		for(long long i = 0; i<localN; i++){
		        u = (double)rand()/(double)RAND_MAX;
               		x = lower_bound + u * (upper_bound - lower_bound);
			local_sum+=8.0 * sqrt(2.0 * M_PI) / exp((2.0 * x) * (2.0 * x));
		}
		
		partial = (upper_bound-lower_bound)*local_sum/ (double)N;
		return partial;

	}
	/* ----------- worker nodes ----------- */
	else{
                // split data
		aveN  = N / size;
		remN  = N % size;
		localN = aveN + (rank < remN ? 1 : 0);
		
		local_sum = 0.0;
		g_worker_start_time = MPI_Wtime(); // start worker timer
		
		// perform monte_carlo calculations
		for(long long i = 0; i<localN; i++){
                        u = (double)rand()/(double)RAND_MAX;
                        x = lower_bound + u * (upper_bound - lower_bound);
                        local_sum+=8.0 * sqrt(2.0 * M_PI) / exp((2.0 * x) * (2.0 * x));
                }

                partial = (upper_bound-lower_bound)*local_sum/ (double)N;
		return partial;
	}
}
