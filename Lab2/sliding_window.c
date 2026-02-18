#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0

int* initialize_data(int N); 
int* distribute_data(int* A, int N);        // returns recv_buff for that rank 
int* mask_operation(int *recv_buff, int N);      // returns updated_buff for that rank 
void collect_results(int *updated_buff, int N, int *Ap);   // *Ap is the processed matrix

// define globals for matrix size
int g_proc_rows = 0;
int g_start_proc = 0;
int g_recv_rows = 0;
double g_time_taken = 0;

int main(int argc, char **argv)
{  
	MPI_Init(&argc, &argv);
	int rank; 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int N = atof(argv[1]);
	int *A = NULL;
	int *Ap = NULL;
	if(rank == MASTER) {
		A = initialize_data(N);
		Ap = malloc((size_t)N*N*sizeof(int));
		for (int i = 0; i < N*N; i++) {
    			Ap[i] = A[i]; //copy vals of original into updated so borders are set to original
		}
	}
	int* temp1 = distribute_data(A, N);
	int* temp2 = mask_operation(temp1, N);
	collect_results(temp2, N, Ap);

	free(temp1);
	free(temp2);
	if (rank == MASTER) {free(A); free(Ap);}
	MPI_Finalize();
	return 0;
}

int* initialize_data(int N){
        srand(1);
        int *A = malloc((size_t)N*N*sizeof(int)); // dynamically allocate matrix for NxN
        puts("--------- INITIAL MATRIX ---------\n[");
	for(int i = 0; i<N; i++){
		for(int j = 0; j<N; j++){
                        A[i*N+j] = rand()%256;
			printf("%3d  ", A[i*N+j]);
                }
		puts("");
        }
	puts("]");
        return A;
}

int* mask_operation(int *recv_buff, int N){
	int* updated = (g_proc_rows > 0) ? (int*)malloc((size_t)g_proc_rows*N*sizeof(int)) : NULL;
	
	if (g_proc_rows == 0) return updated; //return if 0

	for(int row = 1; row<=g_proc_rows; row++){
		int row_index = row-1;
		
		// keep skipped first col as original value
		updated[row_index*N+0] = recv_buff[row*N+0];
		updated[row_index*N+(N-1)] = recv_buff[row*N+(N-1)];

		for(int col = 1; col<=N-2; col++){
			updated[row_index*N+col] = (
					recv_buff[(row-1)*N+(col-1)]+ //a
					recv_buff[(row-1)*N+(col)]+   //b
					recv_buff[(row-1)*N+(col+1)]+ //c
					recv_buff[(row)*N+(col-1)]+   //d
					2*recv_buff[(row)*N+(col)]+   //2e
					recv_buff[(row)*N+(col+1)]+   //f
					recv_buff[(row+1)*N+(col-1)]+ //g
					recv_buff[(row+1)*N+(col)]+   //h
					recv_buff[(row+1)*N+(col+1)]  //i
					)/10;
			
		}
	
	}

	return updated;

}

int* distribute_data(int* A, int N){
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);	
	
	// find row sizes
	int M = N-2;
	int base = M/size;
	int rem = M%size;

	g_proc_rows = (rank<rem ? base+1: base);
	g_start_proc = 1 + rank * base + (rank < rem ? rank : rem);
	g_recv_rows = (g_proc_rows > 0) ? (g_proc_rows + 2) : 0;

	int *send_counts = NULL;
    	int *displs      = NULL;

	if(rank == MASTER){
		send_counts = (int*)malloc((size_t)size*sizeof(int));
		displs = (int*)malloc((size_t)size*sizeof(int));

		for(int i = 0; i<size; i++){
			int rows_processed = (i < rem) ? (base + 1) : base;
			int start_row = 1 + i * base + (i < rem ? i : rem) - 1;
			int total_rows_sent = (rows_processed > 0) ? (rows_processed + 2) : 0;

			send_counts[i] = total_rows_sent*N;
			displs[i] = (total_rows_sent > 0) ? (start_row*N) : 0;

		}
	}

	int recv_count = g_recv_rows*N;
	int *recv_buffer = (recv_count>0) ? (int*)malloc((size_t)recv_count * sizeof(int)) : NULL;

	if(rank == MASTER) g_time_taken = MPI_Wtime();
	MPI_Scatterv(A, send_counts, displs, MPI_INT, recv_buffer, recv_count, MPI_INT, MASTER, MPI_COMM_WORLD);

	if(rank == MASTER) {free(send_counts); free(displs);}

	return recv_buffer;
}

void collect_results(int *updated_buff, int N, int *Ap){
	int rank, size;
 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
	
        // find row sizes
        int M = N-2;
        int base = M/size;
        int rem = M%size;

	int send_count = g_proc_rows*N;

        int *recv_counts = NULL;
        int *displs      = NULL;

	if(rank == MASTER){
		recv_counts = (int*)malloc((size_t)size*sizeof(int));
		displs = (int*)malloc((size_t)size*sizeof(int));

		for(int i = 0; i < size; i++){
			int rows_processed = (i<rem) ? base+1: base;
			int start_row = 1 + i * base + (i< rem ? i : rem);

			recv_counts[i] = rows_processed*N;
			displs[i] = start_row*N;
		
		}
	
	}

	MPI_Gatherv(updated_buff, send_count, MPI_INT, Ap, recv_counts, displs, MPI_INT, MASTER, MPI_COMM_WORLD);
	
	if(rank == MASTER) {
		double timer_stop = MPI_Wtime();
		g_time_taken = timer_stop-g_time_taken; // total time = timer_stop-timer_start

		puts("--------- FINAL MATRIX ---------\n[");
    		for (int i = 0; i < N; i++) {
		        for (int j = 0; j < N; j++) {
		            printf("%3d  ", Ap[i*N + j]);
        		}
		        puts("");
    		}
		puts("]");

		printf("Total time taken: %.6f seconds\n", g_time_taken);

		free(recv_counts); 
		free(displs);
	}

}

