parta:
	[ -f partA/mat_mult_output ] && mv partA/mat_mult_output old_logs/mat_mult_output_`date +%Y%m%d_%H%M%S` || true
	[ -f partA/mat_mult_err ] && mv partA/mat_mult_err old_logs/mat_mult_err_`date +%Y%m%d_%H%M%S` || true
	mpicc -o partA/mat_mult partA/mat_mult.c
	sbatch partA/partA_batch.sh

partb:
	[ -f partB/monte_carlo_output ] && mv partB/monte_carlo_output old_logs/monte_carlo_output_`date +%Y%m%d_%H%M%S` || true
	[ -f partB/monte_carlo_err ] && mv partB/monte_carlo_err old_logs/monte_carlo_err_`date +%Y%m%d_%H%M%S` || true
	mpicc -o partB/monte_carlo partB/monte_carlo.c
	sbatch partB/partB_batch.sh

clean_logs:
	rm old_logs/*

clean_code:
	[ -f partA/mat_mult ] && rm partA/mat_mult || true
	[ -f partB/monte_carlo ] && rm partB/monte_carlo || true

clean_all:
	[ -f partA/mat_mult ] && rm partA/mat_mult || true
	[ -f partB/monte_carlo ] && rm partB/monte_carlo || true
	rm old_logs/*
