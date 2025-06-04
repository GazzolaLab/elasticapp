import os
from typing import Optional

import numpy as np
import sopht.utils as spu
from sopht_mpi.simulator import UnboundedFlowSimulator3D
from mpi4py import MPI
    
import vtkh5py as vh5
from utils import (
    MultiFishSurfaceForcingGrid,
    MultiFishFlowInteractionMPI,
    VTKHDFImageDataWriter,
    FishDataLogger,
)

from elasticapp.Systems import FishModel
from elasticapp.CaseStudies.PyFishCaseStudy import FishCaseStudy
from elasticapp.Arrays._PyArrays import Matrix


def get_data_path(path: str | None) -> str:
    script_path = os.path.abspath(__file__)
    parent_path = os.path.dirname(script_path)
    default_path = parent_path + "/data_mpi"

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
    precision: str = "double",
    rank_distribution: Optional[tuple[int, int, int]] = None,
    data_dir: str | None = None,
) -> None:
    # ================ Global parameters ================
    grid_dim = 3
    real_t = spu.get_real_t(precision)
    rho_f = 1.0
    base_length = 1.0
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
    origins = np.loadtxt("fish_head_positions.txt", delimiter=",")
    num_fish = origins.shape[1]

    # ================ Flow simulator ================
    kinematic_viscosity = base_length * vel_scale / actuation_reynolds_number
    flow_sim = UnboundedFlowSimulator3D(
        grid_size=(grid_size_z, grid_size_y, grid_size_x),
        x_range=x_range,
        kinematic_viscosity=kinematic_viscosity,
        flow_type="navier_stokes_with_forcing",
        real_t=real_t,
        rank_distribution=rank_distribution,
        filter_vorticity=True,
        filter_setting_dict={"order": 5, "type": "convolution"},
    )
    mpi_rank = flow_sim.mpi_construct.rank

    # Get the data directory and broadcast
    if mpi_rank == 0:
        data_dir = get_data_path(data_dir)
    else:
        data_dir = None

    data_dir = flow_sim.mpi_construct.grid.bcast(data_dir, root=0)

    # ================ Fish simulator ================
    if mpi_rank == 0:
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
    else:
        elastic_fish_list = []
        flow_forces = None
        flow_torques = None

    # ============== Immersed boundary ===============
    flow_interaction = MultiFishFlowInteractionMPI(
        mpi_construct=flow_sim.mpi_construct,
        mpi_ghost_exchange_communicator=flow_sim.mpi_ghost_exchange_communicator,
        fish_list=elastic_fish_list,
        num_fish=num_fish,
        n_elem_per_fish=n_elem,
        eul_grid_forcing_field=flow_sim.eul_grid_forcing_field,
        eul_grid_velocity_field=flow_sim.velocity_field,
        virtual_boundary_stiffness_coeff=coupling_stiffness,
        virtual_boundary_damping_coeff=coupling_damping,
        dx=flow_sim.dx,
        grid_dim=grid_dim,
        forcing_grid_cls=MultiFishSurfaceForcingGrid,
        surface_grid_density_for_largest_element=surface_grid_density_for_largest_element,
        master_rank=0,
    )

    # ================ IO ================
    if mpi_rank == 0:
        skip = flow_interaction.forcing_grid.num_lag_nodes // num_fish
        vh5_fish_data = vh5.VH5PartitionedDataset()
        for i in range(num_fish):
            forcing_grid = flow_interaction.forcing_grid
            grid_position = forcing_grid.position_field[
                :, i * skip : (i + 1) * skip
            ].view()
            grid_velocity = forcing_grid.velocity_field[
                :, i * skip : (i + 1) * skip
            ].view()

            grid_data = vh5.VH5PolyDataRod(
                block_name=f"fish_grid_{i}",
                number_of_points=grid_position.shape[1],
            )
            grid_data.set_points(grid_position.T)
            grid_data.add_point_data(grid_velocity=grid_velocity.T)
            vh5_fish_data.add_block(block=grid_data, parent="/Assembly")
        fish_writer = vh5.VH5BlockWriter(vh5_fish_data)

        fish_logger = FishDataLogger(
            rods=elastic_fish_list,
            n_elem=n_elem,
            flow_forces=flow_forces,
        )

    flow_writer = VTKHDFImageDataWriter(
        mpi_comm=flow_sim.mpi_construct.grid,
        dx=flow_sim.dx,
        global_grid_size=flow_sim.grid_size,
        local_grid_size=flow_sim.mpi_construct.local_grid_size,
        ghost_size=flow_sim.ghost_size,
    )

    # ================ Simulation ================
    final_time = non_dim_final_time * period
    data_timer_limit = period / 20.0
    data_timer = 0.0
    if mpi_rank == 0:
        rod_dt = cs.current_dt()
    else:
        rod_dt = None

    rod_dt = flow_sim.mpi_construct.grid.bcast(rod_dt, root=0)

    from time import time

    start_time = time()
    while flow_sim.time < final_time:
        if data_timer == 0.0 or data_timer > data_timer_limit:
            data_timer = 0.0

            # Print progress only on rank 0
            if mpi_rank == 0:
                print(f"{cs.current_time() / final_time * 100:.2f}%", flush=True)

                fish_writer.write_to_file(
                    os.path.join(
                        data_dir, f"fish_{int(cs.current_time() * 1000):04d}.vtkhdf"
                    )
                )
                fish_logger.log_data(flow_sim.time)

            # Log flow data for all ranks (HDF5-MPI)
            flow_writer.write_to_file(
                filename=os.path.join(
                    data_dir, f"flow_{int(flow_sim.time * 1000):04d}.vtkhdf"
                ),
                velocity=flow_sim.velocity_field,
                vorticity=flow_sim.vorticity_field,
            )

        # Compute time steps
        flow_dt = min(
            flow_sim.compute_stable_timestep(dt_prefac=0.25, precision=precision),
            rod_dt * 100,
        )
        rod_time_steps = max(int(flow_dt / rod_dt), 1)
        local_rod_dt = flow_dt / rod_time_steps

        # Rod + IBM steps
        for i in range(rod_time_steps):
            flow_interaction.compute_flow_forces_and_torques()
            if mpi_rank == 0:
                flow_forces[...] = flow_interaction.body_flow_forces
                flow_torques[...] = flow_interaction.body_flow_torques
                cs.step_with(updated_dt=local_rod_dt)

            flow_interaction.time_step(dt=local_rod_dt)

        # Update IBM grid
        flow_interaction()

        # Flow step
        flow_sim.time_step(dt=flow_dt, free_stream_velocity=None)

        # Update data timer
        data_timer += flow_dt

    # Finalize the simulation
    sim_time = time() - start_time

    sim_time = flow_sim.mpi_construct.grid.reduce(sim_time, op=MPI.MIN, root=0)
    if mpi_rank == 0:
        print(f"Simulation completed in {sim_time:.2f} seconds.")

    if mpi_rank == 0:
        fish_logger.write_to_file(
            os.path.join(data_dir, "fish_data.npz"),
        )
    MPI.Finalize()


if __name__ == "__main__":
    nx = 432
    n_elements = nx // 12
    n_max = n_elements // 4
    run_fish_swimming(
        non_dim_final_time=0.1,
        n_elem=n_elements,
        grid_size_x=nx,
        surface_grid_density_for_largest_element=n_max,
        slenderness_ratio=12.500535121608523,
        mass_ratio=1.0,
        non_dim_bending_stiffness=0.565414058870102,
        actuation_reynolds_number=7142.857142857143,
    )
