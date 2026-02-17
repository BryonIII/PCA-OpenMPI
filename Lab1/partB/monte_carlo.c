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
	/* ----------- master node ----------- */
        if(rank==0){
		double total = *result;
		// receive and sum all results
                for(int src = 1; src < size; src++){
                        double tmp = 0.0;
			MPI_Recv(&tmp, 1, MPI_DOUBLE,src, FROM_WORKER, MPI_COMM_WORLD, &status);
			total+=tmp;
                }
		double end_time = MPI_Wtime(); // measure end time immediately after last result received
		*result = total;

		// get time spent and final result
		double start_time = g_worker_start_time;
		printf("Master worker's time was: %.8f\nEnd time was: %.8f\n", g_worker_start_time, end_time);
		for(int src = 1; src < size; src++){
			double tmp;
			MPI_Recv(&tmp, 1, MPI_DOUBLE,src, TIME_FROM_WORKER, MPI_COMM_WORLD, &status);
			if(tmp<start_time) start_time=tmp;
		}
		
		// print final results and time spend
		printf("The result is %.8f\n", *result);
		printf("Time spent: %.8f seconds\n", end_time-start_time);
        }
	/* ----------- worker nodes ----------- */
	else{
		// send result inside of timer
		MPI_Send(result, 1, MPI_DOUBLE, MASTER, FROM_WORKER, MPI_COMM_WORLD);
		
		// send time taken outside of timer
		MPI_Send(&g_worker_start_time, 1, MPI_DOUBLE, MASTER, TIME_FROM_WORKER, MPI_COMM_WORLD);
		
		// print result outside of timer for verification/partial credit
		printf("Worker %d's start time was: %.8f\n", rank, g_worker_start_time);
                printf("Worker %d's result was: %.8f\n", rank, *result);
	}
}

double estimate_g(double lower_bound, double upper_bound, long long int N){
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	long long localN;
	double partial, local_sum, u, x;
	/* ----------- master node ----------- */
	if(rank==0){
        	local_sum = 0.0;
		
		long long aveN = N/size;
		long long remN = N%size;

		// split data and send to workers
		for(int dest = 1; dest < size; dest++){
			localN = (dest < remN) ? aveN+1 : aveN;
			MPI_Send(&localN, 1, MPI_LONG_LONG, dest, FROM_MASTER, MPI_COMM_WORLD);
		}

		localN = (0 < remN) ? aveN+1 : aveN;
		
		g_worker_start_time = MPI_Wtime(); // start master worker timer

		// perform monte_carlo calculation
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
		// receive data
		MPI_Recv(&localN, 1, MPI_LONG_LONG, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
                local_sum = 0.0;
		g_worker_start_time = MPI_Wtime(); // start worker timer
		
		// perform monte_carlo calculation
		for(long long i = 0; i<localN; i++){
                        u = (double)rand()/(double)RAND_MAX;
                        x = lower_bound + u * (upper_bound - lower_bound);
                        local_sum+=8.0 * sqrt(2.0 * M_PI) / exp((2.0 * x) * (2.0 * x));
                }

                partial = (upper_bound-lower_bound)*local_sum/ (double)N;
		return partial;
	}
}
