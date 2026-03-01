#!/bin/bash
#SBATCH --job-name=bryon_catlin_mat_multOMP
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1  #(single node)
#SBATCH --ntasks=1  #(single process)
#SBATCH --cpus-per-task=2  #(32 total, class limit)
#SBATCH --mem-per-cpu=600mb
#SBATCH -t 00:02:00
#SBATCH -o Part1_Fig1_2threads_out
#SBATCH -e Part1_Fig1_2threads_err
export OMP_NUM_THREADS=2
./mat_multOMP 200 200 200

