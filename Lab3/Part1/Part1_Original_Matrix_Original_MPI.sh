#!/bin/bash
#SBATCH --job-name=bryon_catlin_mat_multOMP
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1  #(single node)
#SBATCH --ntasks=4  #(single process)
#SBATCH --cpus-per-task=4  #(32 total, class limit)
#SBATCH --mem-per-cpu=600mb
#SBATCH -t 00:02:00
#SBATCH -o Part1_Original_Matrix_Original_MPI_out
#SBATCH -e Part1_Original_Matrix_Original_MPI_err
export OMP_NUM_THREADS=4
srun --mpi=$HPC_PMIX ./mat_multMPI 60 12 10

