#!/bin/bash
#SBATCH --job-name=bryon_catlin_mat_multOMP
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1  #(single node)
#SBATCH --ntasks=1  #(single process)
#SBATCH --cpus-per-task=1  #(32 total, class limit)
#SBATCH --mem-per-cpu=600mb
#SBATCH -t 00:02:00
#SBATCH -o Part1_Fig2_1_400_out
#SBATCH -e Part1_Fig2_1_400_err
export OMP_NUM_THREADS=1
./mat_multOMP 400 400 400

