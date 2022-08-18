rm -rf data
mkdir data
./example_nest_with_wall --n_rods=450 --final_sim_time=300.00 --dt=5e-7 --contact_stiffness=2e6 --packing_initial_vertical_span=300.0 --damping=65 --contact_damping_normal=1e1 --contact_damping_tangential=1e1
python plot_nest.py
