#!/bin/bash
#SBATCH --job-name=bryon_catlin_sliding_window_MPI
#SBATCH --mail-type=FAIL
#SBATCH --mail-user=bryon.catlin@ufl.edu
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1
#SBATCH --ntasks=32
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=1000mb
#SBATCH -t 00:01:00
#SBATCH -o OutputR32N1000_out
#SBATCH -e OutputR32N1000_err
srun --mpi=$HPC_PMIX ./sliding_window 1000
