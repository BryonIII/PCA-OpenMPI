#!/bin/bash
#SBATCH --job-name=bryon_catlin_sliding_window_MPI
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
#SBATCH -o sliding_window_out
#SBATCH -e sliding_window_err
srun --mpi=$HPC_PMIX ./sliding_window 15
