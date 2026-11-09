from pathlib import Path
from time import time

import click
import numpy as np

import sopht.simulator as sps

from elasticapp.Systems import FishModel
from elasticapp.CaseStudies.PyFishCaseStudy import FishCaseStudy
from elasticapp.Arrays._PyArrays import Matrix

import fish_utils as fu


def run_fish_swimming(
    non_dim_final_time: float,
    n_elem: int,
    grid_size_x: int,
    surface_grid_density_for_largest_element: int,
    slenderness_ratio: float,
    mass_ratio: float,
    non_dim_bending_stiffness: float,
    actuation_reynolds_number: float,
    coupling_stiffness: float = -3.2e4,
    coupling_damping: float = -32.0,
    flow_num_threads: int = 4,
    save_data: bool = False,
    data_dir: str | None = None,
) -> None:
    # ================ Global parameters ================
    grid_dim = 3
    rho_f = 1.0
    base_length = 1.0
    x_range, y_range, z_range = (4.0, 1.0, 0.5)
    dx = x_range / grid_size_x
    grid_size_y = int(np.ceil(y_range / dx))
    grid_size_z = int(np.ceil(z_range / dx))

    # ================ Fish parameters ================
    head_position = Matrix(np.array([[2.6, 0.5, 0.25]], dtype=np.float64).T)
    period = 1.0
    vel_scale = base_length / period
    rho_s = mass_ratio * rho_f
    base_diameter = base_length / slenderness_ratio
    base_radius = base_diameter / 2
    moment_of_inertia = np.pi / 4 * base_radius**4
    youngs_modulus = (
        non_dim_bending_stiffness
        * rho_f
        * vel_scale**2
        * base_length**3
        * base_diameter
        / moment_of_inertia
    )

    # ================ Fish simulator ================
    cs = FishCaseStudy(
        num_fish=1,
        origins=head_position,
        n_elements=n_elem,
        density=rho_s,
        length=base_length,
        damping_rate=0.16,
        youngs_modulus=youngs_modulus,
        dt_scale=5e-4,
        period=period,
    )
    elastic_fish_list, virtual_fish_list, flow_forces, flow_torques = cs.communicate()
    flow_forces = np.asarray(flow_forces)
    flow_torques = np.asarray(flow_torques)

    # ================ Flow simulator ================
    kinematic_viscosity = base_length * vel_scale / actuation_reynolds_number
    flow_sim = sps.create_unbounded_flow_simulator_3d(
        grid_size=(grid_size_z, grid_size_y, grid_size_x),
        x_range=x_range,
        kinematic_viscosity=kinematic_viscosity,
        flow_type="navier_stokes_with_forcing",
        real_t=np.float64,
        num_threads=flow_num_threads,
        filter_vorticity=True,
        filter_setting_dict={"order": 5, "type": "convolution"},
    )

    # ============== Immersed boundary ===============
    flow_interaction = fu.FishFlowInteraction(
        cosserat_rod=elastic_fish_list[0],
        eul_grid_forcing_field=flow_sim.eul_grid_forcing_field,
        eul_grid_velocity_field=flow_sim.velocity_field,
        virtual_boundary_stiffness_coeff=coupling_stiffness,
        virtual_boundary_damping_coeff=coupling_damping,
        dx=flow_sim.dx,
        grid_dim=grid_dim,
        real_t=np.float64,
        num_threads=flow_num_threads,
        forcing_grid_cls=fu.FishSurfaceForcingGrid,
        surface_grid_density_for_largest_element=surface_grid_density_for_largest_element,
    )

    # ================ IO ================
    if save_data:
        data_dir = fu.get_data_path(data_dir)
        flow_data_manager = fu.ImageDataIO(dx=flow_sim.dx, grid_size=flow_sim.grid_size)
        fish_data_manager = fu.FishIO(elastic_fish_list, virtual_fish_list)
        grid_data_manager = fu.GridIO(
            num_fish=1, num_lag_points_per_fish=flow_interaction.forcing_grid.num_lag_nodes
        )

    # ================ Simulation ================
    final_time = non_dim_final_time * period
    data_timer_limit = period / 20.0
    data_timer = 0.0
    rod_dt = cs.current_dt()

    flow_time = 0.0
    io_time = 0.0
    fsi_time = 0.0
    rod_time = 0.0


    print(
        f"{'Progress (time/final_time)':<26}  {'flow_time':>10}  {'fsi_time':>10}  "
        f"{'rod_time':>10}  {'io_time':>10}  {'grid error':>12}"
    )

    while flow_sim.time < final_time:
        # Save data

        a = time()
        if save_data and (data_timer == 0.0 or data_timer > data_timer_limit):
            data_timer = 0.0
            flow_data_manager.write_to_file(
                filename=data_dir / f"flow_{int(flow_sim.time * 1000):04d}.vtkhdf",
                time=flow_sim.time,
                velocity=flow_sim.velocity_field,
                vorticity=flow_sim.vorticity_field,
            )
            fish_data_manager.write_to_file(
                filename=data_dir / f"fish_{int(cs.current_time() * 1000):04d}.h5",
                time=np.float64(cs.current_time()),
            )
            grid_data_manager.write_to_file(
                filename=data_dir / f"grid_{int(cs.current_time() * 1000):04d}.vtkhdf",
                time=np.float64(cs.current_time()),
                position=flow_interaction.forcing_grid.position_field,
                velocity=flow_interaction.forcing_grid.velocity_field,
            )
            grid_error = flow_interaction.get_grid_deviation_error_l2_norm()
            progress = f"{flow_sim.time:.4f}/{final_time:.4f}"
            print(
                f"{progress:<26}  {flow_time:>9.2f}s  {fsi_time:>9.2f}s  "
                f"{rod_time:>9.2f}s  {io_time:>9.2f}s  {grid_error:>12.4e}"
            )
        io_time += time() - a

        # Compute time steps
        flow_dt = min(flow_sim.compute_stable_timestep(dt_prefac=0.25), rod_dt * 20)
        rod_time_steps = max(int(flow_dt / rod_dt), 1)
        local_rod_dt = flow_dt / rod_time_steps

        # Rod + IBM steps
        for _ in range(rod_time_steps):
            a = time()
            flow_interaction.compute_flow_forces_and_torques()
            flow_interaction.time_step(dt=local_rod_dt)

            # Copy flow forces and torques into C++ buffer
            flow_forces[...] = flow_interaction.body_flow_forces
            flow_torques[...] = flow_interaction.body_flow_torques

            fsi_time += time() - a

            a = time()
            cs.step_with(dt=local_rod_dt)
            rod_time += time() - a

        # Update IBM grid
        a = time()
        flow_interaction()
        fsi_time += time() - a

        # Flow step
        a = time()
        flow_sim.time_step(dt=flow_dt)
        flow_time += time() - a

        # Update data timer
        data_timer += flow_dt


@click.command()
@click.option("--final-time", "-t", type=float, default=1.0)
@click.option("--grid-size", "-n", type=int, default=96)
@click.option("--num-threads", "-j", type=int, default=1)
@click.option("--out-dir", "-o", type=str, default="fish_data")
def main(final_time: float, grid_size: int, num_threads: int, out_dir: str):
    out_dir = Path(out_dir).resolve()
    out_dir.mkdir(parents=True, exist_ok="True")

    n_elements = grid_size // 4
    n_max = n_elements // 3
    run_fish_swimming(
        non_dim_final_time=final_time,
        n_elem=n_elements,
        grid_size_x=grid_size,
        surface_grid_density_for_largest_element=n_max,
        flow_num_threads=num_threads,
        slenderness_ratio=12.500535121608523,
        mass_ratio=1.0,
        non_dim_bending_stiffness=0.565414058870102,
        actuation_reynolds_number=7142.857142857143,
        save_data=True,
        data_dir=out_dir,
    )

if __name__ == "__main__":
    import logging
    logging.basicConfig(level=logging.INFO)

    main()
