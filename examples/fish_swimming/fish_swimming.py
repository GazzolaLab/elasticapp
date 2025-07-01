import os

import numpy as np

import fish_utils as fu
import fish_utils.io as fuio
import vtkh5py as vh5
import sopht.simulator as sps
import sopht.utils as spu


def get_data_path(path: str | None) -> str:
    script_path = os.path.abspath(__file__)
    parent_path = os.path.dirname(script_path)
    default_path = parent_path + "/data"

    def create_path(path: str) -> str:
        if not os.path.exists(path):
            from warnings import warn

            warn(f"Path {path} does not exist. Trying to create it.")
            try:
                os.makedirs(path)
                return path
            except OSError as e:
                warn(f"Failed to create directory {path}: {e}")
                os.makedirs(default_path)
                return default_path

        return path

    if path is None:  # default path
        return create_path(default_path)

    while path[-1] == "/":  # strip trailing slashes
        path = path[:-1]

    if path[0] == "/":  # absolute path
        return create_path(path)

    return create_path(os.path.join(parent_path, path))


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
    num_threads: int = 4,
    precision: str = "double",
    data_dir: str | None = None,
    fish_head_positions_file="fish_init_data/fish_head_positions.txt",
) -> None:
    # ================ Global parameters ================
    grid_dim = 3
    real_t = spu.get_real_t(precision)
    rho_f = 1.0
    base_length = 1.0
    n_node = n_elem + 1
    x_range, y_range, z_range = (12.0, 8.0, 8.0)
    dx = x_range / grid_size_x
    grid_size_y = int(np.ceil(y_range / dx))
    grid_size_z = int(np.ceil(z_range / dx))

    # ================ Fish parameters ================
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
    origins = np.loadtxt(fish_head_positions_file, delimiter=",")
    num_fish = origins.shape[1]

    from elasticapp.Systems import FishModel
    from elasticapp.CaseStudies.PyFishCaseStudy import FishCaseStudy
    from elasticapp.Arrays._PyArrays import Matrix

    cs = FishCaseStudy(
        num_fish=num_fish,
        origins=Matrix(origins),
        n_elements=n_elem,
        density=rho_s,
        length=base_length,
        damping_rate=0.16,
        youngs_modulus=youngs_modulus,
        dt_scale=5e-4,
        period=period,
    )
    elastic_fish_list, _, flow_forces, flow_torques = cs.communicate()
    flow_forces = np.asarray(flow_forces)
    flow_torques = np.asarray(flow_torques)

    # ================ Flow simulator ================
    kinematic_viscosity = base_length * vel_scale / actuation_reynolds_number
    flow_sim = sps.UnboundedFlowSimulator3D(
        grid_size=(grid_size_z, grid_size_y, grid_size_x),
        x_range=x_range,
        kinematic_viscosity=kinematic_viscosity,
        flow_type="navier_stokes_with_forcing",
        real_t=real_t,
        num_threads=num_threads,
        filter_vorticity=True,
        filter_setting_dict={"order": 5, "type": "convolution"},
    )

    # ============== Immersed boundary ===============
    flow_interactions = []
    for i in range(num_fish):
        flow_interactions.append(
            fu.FishFlowInteraction(
                cosserat_rod=elastic_fish_list[i],
                eul_grid_forcing_field=flow_sim.eul_grid_forcing_field,
                eul_grid_velocity_field=flow_sim.velocity_field,
                virtual_boundary_stiffness_coeff=coupling_stiffness,
                virtual_boundary_damping_coeff=coupling_damping,
                dx=flow_sim.dx,
                grid_dim=grid_dim,
                real_t=real_t,
                num_threads=num_threads,
                forcing_grid_cls=fu.FishSurfaceForcingGrid,
                surface_grid_density_for_largest_element=surface_grid_density_for_largest_element,
            )
        )

    # ================ IO ================
    data_dir = get_data_path(data_dir)
    vh5_fish_data = vh5.VH5PartitionedDataset()
    for idx, flow_interaction in enumerate(flow_interactions):
        forcing_grid = flow_interaction.forcing_grid
        grid_position = forcing_grid.position_field.view()
        grid_velocity = forcing_grid.velocity_field.view()

        grid_data = vh5.VH5PolyDataRod(
            block_name=f"fish_grid_{idx}",
            number_of_points=grid_position.shape[1],
        )
        grid_data.set_points(grid_position.T)
        grid_data.add_point_data(grid_velocity=grid_velocity.T)
        vh5_fish_data.add_block(block=grid_data, parent="/Assembly")

    fish_writer = vh5.VH5BlockWriter(vh5_fish_data)

    flow_data = vh5.VH5ImageData(
        spacing=(flow_sim.dx,) * 3,
        whole_extent=(0, grid_size_x - 1, 0, grid_size_y - 1, 0, grid_size_z - 1),
        origin=(flow_sim.dx / 2,) * 3,
    )
    flow_data.add_point_data(
        velocity=np.transpose(flow_sim.velocity_field, (1, 2, 3, 0)),
        vorticity=np.transpose(flow_sim.vorticity_field, (1, 2, 3, 0)),
    )
    flow_writer = vh5.VH5BlockWriter(flow_data)

    fish_logger = fuio.FishDataLogger(
        rods=elastic_fish_list,
        n_elem=n_elem,
        flow_forces=flow_forces,
    )

    # ================ Simulation ================
    final_time = non_dim_final_time * period
    data_timer_limit = period / 20.0
    data_timer = 0.0
    rod_dt = cs.current_dt()

    from time import time

    start_time = time()
    while flow_sim.time < final_time:
        print(f"{flow_sim.time / final_time * 100:.2f}%", end="\r")
        # Save data
        if data_timer == 0.0 or data_timer > data_timer_limit:
            data_timer = 0.0
            fish_writer.write_to_file(
                os.path.join(
                    data_dir, f"fish_{int(cs.current_time() * 1000):04d}.vtkhdf"
                )
            )
            flow_writer.write_to_file(
                os.path.join(data_dir, f"flow_{int(flow_sim.time * 1000):04d}.vtkhdf")
            )
            fish_logger.log_data(flow_sim.time)

        # Compute time steps
        flow_dt = min(flow_sim.compute_stable_timestep(dt_prefac=0.25), rod_dt * 100)
        rod_time_steps = max(int(flow_dt / rod_dt), 1)
        local_rod_dt = flow_dt / rod_time_steps

        # Rod + IBM steps
        for i in range(rod_time_steps):
            for j, flow_interaction in enumerate(flow_interactions):
                flow_interaction.compute_flow_forces_and_torques()
                flow_interaction.time_step(dt=local_rod_dt)

                flow_forces[:, j * n_node : (j + 1) * n_node] = (
                    flow_interaction.body_flow_forces
                )
                flow_torques[:, j * n_elem : (j + 1) * n_elem] = (
                    flow_interaction.body_flow_torques
                )

            cs.step_with(updated_dt=local_rod_dt)

        # Update IBM grid
        for flow_interaction in flow_interactions:
            flow_interaction()

        # Flow step
        flow_sim.time_step(dt=flow_dt)

        # Update data timer
        data_timer += flow_dt

    print(f"Simulation completed in {time() - start_time:.2f} seconds.")
    fish_logger.write_to_file(os.path.join(data_dir, "fish_data.npz"))


if __name__ == "__main__":
    nx = 96
    n_elements = nx // 12
    n_max = n_elements // 4
    run_fish_swimming(
        non_dim_final_time=1.0,
        n_elem=n_elements,
        grid_size_x=nx,
        surface_grid_density_for_largest_element=n_max,
        slenderness_ratio=12.500535121608523,
        mass_ratio=1.0,
        non_dim_bending_stiffness=0.565414058870102,
        actuation_reynolds_number=7142.857142857143,
        data_dir="fish_data_no_mpi",
        fish_head_positions_file="fish_init_data/fish_head_positions.txt",
    )
