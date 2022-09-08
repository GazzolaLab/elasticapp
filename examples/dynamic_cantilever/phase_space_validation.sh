#!/bin/env bash

mode_array=("$(seq 0 1 4)")
RESULTS_DIR="phase_space_results"
mkdir -p "${RESULTS_DIR}"

for mode in "${mode_array[@]}"; do
	echo "Processing phase-space for mode ${mode}"
	ELASTICA_NO_SYSTEM_WARN=1 python3 phase_space_validation.py --mode "${mode}" --fig
	mv frequency_phase_space.png "${RESULTS_DIR}/frequency_phase_space_${mode}.png"
	mv amplitude_phase_space.png "${RESULTS_DIR}/amplitude_phase_space_${mode}.png"
done
unset RESULTS_DIR
unset mode_array
