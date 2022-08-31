rm -rf data
mkdir data
export RUNSCRIPT="../../../cmake-release-gcc/bin/example_nest_with_wall"
${RUNSCRIPT} --n_rods=450 --final_sim_time=10.00 --dt=1e-6 --contact_stiffness=2e6 --packing_initial_vertical_span=200.0 --damping=80 --contact_damping_normal=1e1 --contact_damping_tangential=1e1
python plot_nest.py
