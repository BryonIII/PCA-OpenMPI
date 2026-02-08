/*Bryon Catlin monte_carlo.c */

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

double estimate_g(double lower_bound, double upper_bound, long long int N);
void collect_results(double *result);
void init_rand_seed();

#define MASTER 0               /* taskid of first task */
#define FROM_MASTER 1          /* setting a message type */
#define FROM_WORKER 2          /* setting a message type */

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
	int rank, size;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
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
                for(int src = 1; src < size; src++){
                        double tmp = 0.0;
			MPI_Recv(&tmp, 1, MPI_DOUBLE,src, FROM_WORKER, MPI_COMM_WORLD, &status);
			total+=tmp;
                }
		*result = total;
		printf("The result is %.8f", *result);
        }
	/* ----------- worker nodes ----------- */
	else{
		MPI_Send(result, 1, MPI_DOUBLE, MASTER, FROM_WORKER, MPI_COMM_WORLD);
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

		for(int dest = 1; dest < size; dest++){
			localN = (dest < remN) ? aveN+1 : aveN;
			MPI_Send(&localN, 1, MPI_LONG_LONG, dest, FROM_MASTER, MPI_COMM_WORLD);
		}

		localN = (0 < remN) ? aveN+1 : aveN;

		for(long long i = 0; i<localN; i++){
		        u = (double)rand()/(double)RAND_MAX;
               		x = lower_bound + u * (upper_bound - lower_bound);
			local_sum+=8.0 * sqrt(2.0 * M_PI) / exp((2.0 * x) * (2.0 * x));
			if(i == 0) printf("this is %d and my rand is u=%.8f\n", rank, u);
		}
		
		partial = (upper_bound-lower_bound)*local_sum/ (double)N;
		return partial;

	}
	/* ----------- worker nodes ----------- */
	else{
		MPI_Recv(&localN, 1, MPI_LONG_LONG, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
                local_sum = 0.0;
		
		for(long long i = 0; i<localN; i++){
                        u = (double)rand()/(double)RAND_MAX;
                        x = lower_bound + u * (upper_bound - lower_bound);
                        local_sum+=8.0 * sqrt(2.0 * M_PI) / exp((2.0 * x) * (2.0 * x));
			if(i == 0) printf("this is %d and my rand is u=%.8f\n", rank, u);
                }

                partial = (upper_bound-lower_bound)*local_sum/ (double)N;
		return partial;
	}
}
