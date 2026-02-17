#!/bin/bash
#SBATCH --job-name=bryon_catlin_matrix_mult_MPI
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=bryon.catlin@ufl.edu
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=8
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:01:00
#SBATCH -o partA/mat_mult_out
#SBATCH -e partA/mat_mult_err
srun --mpi=$HPC_PMIX ./partA/mat_mult
