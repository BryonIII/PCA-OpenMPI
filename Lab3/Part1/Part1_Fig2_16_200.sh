#!/bin/bash
#SBATCH --job-name=bryon_catlin_mat_multOMP
#SBATCH --account=eel6763
#SBATCH --qos=eel6763
#SBATCH --nodes=1  #(single node)
#SBATCH --ntasks=1  #(single process)
#SBATCH --cpus-per-task=16  #(32 total, class limit)
#SBATCH --mem-per-cpu=600mb
#SBATCH -t 00:02:00
#SBATCH -o Part1_Fig2_16_200_out
#SBATCH -e Part1_Fig2_16_200_err
export OMP_NUM_THREADS=16
./mat_multOMP 200 200 200

