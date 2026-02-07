#!/bin/bash
#SBATCH --job-name=matrix_mult_MPI
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=bryon.catlin@ufl.edu
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=8
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:05:00
#SBATCH -o mat_mult_output
#SBATCH -e mat_mult_err
srun --mpi=$HPC_PMIX ./mat_mult
