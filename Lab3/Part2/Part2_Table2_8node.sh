#!/bin/bash
#SBATCH --job-name=bryon_catlin_mat_multOMP
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=8  # Number of nodes 
#SBATCH --ntasks=8  # Number of MPI ranks
#SBATCH --ntasks-per-node=1 # Number of MPI ranks per node
#SBATCH --cpus-per-task=4 
#SBATCH --ntasks-per-socket=1  # Number of tasks per processor socket on the node
#SBATCH --mem-per-cpu=600mb
#SBATCH -t 00:02:00
#SBATCH -o Part2_Table2_8node_out
#SBATCH -e Part2_Table2_8node_err
export OMP_NUM_THREADS=8
srun --mpi=$HPC_PMIX ./mat_mult_hybridOMPMPI 1600 1600 1600

