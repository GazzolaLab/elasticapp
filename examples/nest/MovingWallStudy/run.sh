rm -rf data
mkdir data
../../../cmake-build-release/bin/example_moving_wall --final_sim_time=15.00 --dt=1e-6 --contact_stiffness=1e6 --damping=85 --contact_damping_normal=1e1 --contact_damping_tangential=1e1
python plot_nest.py
