// Bryon Catlin

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define CHUNKSIZE   10
#define N       100

/*   ttype: type to use for representing time */
typedef double ttype;
ttype tdiff(struct timespec a, struct timespec b)
/* Find the time difference. */
{
  ttype dt = (( b.tv_sec - a.tv_sec ) + ( b.tv_nsec - a.tv_nsec ) / 1E9);
  return dt;
}

struct timespec now()
/* Return the current time. */
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t;
}


// #define NRA 60                 /* number of rows in matrix A */
// #define NCA_RB 12                 /* number of columns in matrix A and rows in matrix */
// #define NCB 10                  /* number of columns in matrix B */

int main (int argc, char *argv[])
{

    int NRA = atof(argv[1]); 
    int NCA_RB = atof(argv[2]); 
    int NCB = atof(argv[3]); 

    int A[NRA][NCA_RB], B[NCA_RB][NCB], C[NRA][NCB]; 
    int i, j, k, numtasks;

    //clock_t begin, end;
    struct timespec begin, end;
    double time_spent;
     
    #pragma omp parallel shared(A, B, C, NRA, NCA_RB, NCB, begin, end) private(i,j,k)
    {
        int tid = omp_get_thread_num();
        
        if(tid == 0){

            printf("Initializing arrays...\n");
            for (i=0; i<NRA; i++)
                for (j=0; j<NCA_RB; j++)
                    A[i][j]= i+j;  

            printf (" Contents of matrix A\n");
                for (i=0; i<NRA; i++) {  
                    for (j=0; j<NCA_RB; j++)
                        printf("%d\t", A[i][j]);
                        printf("\n");
            } 

            for (i=0; i<NCA_RB; i++)   
                for (j=0; j<NCB; j++)
                    B[i][j]= i-j;

            printf (" Contents of matrix B\n");
            for (i=0; i<NCA_RB; i++) {  
                for (j=0; j<NCB; j++)
                    printf("%d\t", B[i][j]);
                    printf("\n");
                    printf("\n");        
            }     
        }
        #pragma omp barrier

        #pragma omp single
        begin = now();

        #pragma omp for collapse(2) schedule(static)
        for (i=0; i<NRA; i++) 
        {
            for (j=0; j<NCB; j++) 
            { 
                C[i][j] = 0; 
                for (k=0; k<NCA_RB; k++)  /* NCA = NRB = NCA_RB*/           
                    C[i][j] = C[i][j] + A[i][k] * B[k][j]; 
            }
        }

        #pragma omp barrier //add barrier to ensure all threads are completed before stopping the timer
        #pragma omp single
        end = now();

    }

    numtasks = omp_get_max_threads();
    printf("omp_mm has started with %d OpenMP threads.\n",numtasks);
    printf("Matrix A: #rows %d; #cols %d\n", NRA, NCA_RB);
    printf("Matrix B: #rows %d; #cols %d\n", NCA_RB, NCB);
    printf ("\n");

    time_spent = tdiff(begin, end);
    
    /* Print results */
    printf ("\n");
    printf("******************************************************\n");
    printf("Result Matrix:\n");
    for (i=0; i<NRA; i++)
    {
        printf("\n"); 
        for (j=0; j<NCB; j++) 
        printf("%d\t", C[i][j]);
    }
    
    printf("\n******************************************************\n");
    printf ("\n");
    printf("total time: %.8f sec\n", time_spent);
    printf ("\n");
        

}