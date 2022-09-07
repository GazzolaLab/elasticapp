rm -rf data
mkdir data
../../../cmake-build-smp-release-clang-latest/bin/example_moving_wall_static_rod --final_sim_time=10.0 --dt=1e-5 --contact_stiffness=1e6 --damping=85 --contact_damping_normal=1e1 --contact_damping_tangential=1e1 --youngs_modulus=12e6 --shear_modulus=8e6
python plot_nest.py
