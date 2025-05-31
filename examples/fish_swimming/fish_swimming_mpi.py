import os
from typing import Optional

import numpy as np
import sopht.utils as spu
from sopht_mpi.simulator import UnboundedFlowSimulator3D

import vtkh5py as vh5
from utils import (
    FishSurfaceForcingGrid,
    FishFlowInteractionMPI,
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
    grid_size: tuple[int, int, int],
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
    grid_size_z, grid_size_y, grid_size_x = grid_size
    real_t = spu.get_real_t(precision)
    rho_f = 1.0
    base_length = 1.0
    n_node = n_elem + 1
    x_range = 4 * base_length
    y_range = grid_size_y / grid_size_x * x_range
    z_range = grid_size_z / grid_size_x * x_range

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
    origins = np.array(
        [0.65 * x_range - 0.5 * base_length, 0.5 * y_range, 0.5 * z_range]
    ).reshape(3, -1)
    origins = np.tile(origins, (1, 6))

    # ================ Flow simulator ================
    kinematic_viscosity = base_length * vel_scale / actuation_reynolds_number
    flow_sim = UnboundedFlowSimulator3D(
        grid_size=grid_size,
        x_range=x_range,
        kinematic_viscosity=kinematic_viscosity,
        flow_type="navier_stokes_with_forcing",
        real_t=real_t,
        rank_distribution=rank_distribution,
        filter_vorticity=True,
        filter_setting_dict={"order": 5, "type": "convolution"},
    )
    mpi_rank = flow_sim.mpi_construct.rank
    mpi_size = flow_sim.mpi_construct.size

    # Get the data directory and broadcast
    if mpi_rank == 0:
        data_dir = get_data_path(data_dir)
    else:
        data_dir = None

    data_dir = flow_sim.mpi_construct.grid.bcast(data_dir, root=0)

    # ================ Fish simulator ================
    # Distribute fishes evenly across MPI ranks
    num_fish_total = origins.shape[1]
    num_fish_per_rank = np.ones((mpi_size,), np.int32) * (num_fish_total // mpi_size)
    rem = num_fish_total % mpi_size
    num_fish_per_rank[:rem] += 1
    index = np.insert(np.cumsum(num_fish_per_rank), 0, 0)
    if mpi_size == 1:
        origin = Matrix(origins)
    else:
        tmp = origins[:, index[mpi_rank] : index[mpi_rank + 1]].copy()
        origin = Matrix(tmp)

    has_fish = num_fish_per_rank[mpi_rank] > 0

    # Initialize the case study only if there are fishes on this rank
    if has_fish:
        cs = FishCaseStudy(
            num_fish=int(num_fish_per_rank[mpi_rank]),
            origins=origin,
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
    flow_interactions = []

    # Each rank has flow_interaction objects for all fishes
    # But the only functional ones are those that corresponds to the fish
    # on this rank.
    auto_ghosting = True
    for rank_idx in range(mpi_size):
        for fish_idx in range(num_fish_per_rank[rank_idx]):
            rod = elastic_fish_list[fish_idx] if mpi_rank == rank_idx else n_elem
            flow_interactions.append(
                FishFlowInteractionMPI(
                    mpi_construct=flow_sim.mpi_construct,
                    mpi_ghost_exchange_communicator=flow_sim.mpi_ghost_exchange_communicator,
                    cosserat_rod=rod,
                    eul_grid_forcing_field=flow_sim.eul_grid_forcing_field,
                    eul_grid_velocity_field=flow_sim.velocity_field,
                    virtual_boundary_stiffness_coeff=coupling_stiffness,
                    virtual_boundary_damping_coeff=coupling_damping,
                    dx=flow_sim.dx,
                    grid_dim=grid_dim,
                    forcing_grid_cls=FishSurfaceForcingGrid,
                    surface_grid_density_for_largest_element=surface_grid_density_for_largest_element,
                    master_rank=rank_idx,
                    auto_ghosting=auto_ghosting,
                )
            )
            auto_ghosting = False  # Velocity communication needs to be done only once

    print(f"Rank: {mpi_rank}, Number of fish: {num_fish_per_rank[mpi_rank]}")

    # ================ IO ================
    if has_fish:
        fish_writers = []
        for i in np.arange(index[mpi_rank], index[mpi_rank + 1]):
            forcing_grid = flow_interactions[i].forcing_grid
            grid_position = forcing_grid.position_field.view()
            grid_velocity = forcing_grid.velocity_field.view()

            grid_data = vh5.VH5PolyDataRod(
                number_of_points=grid_position.shape[1],
            )
            grid_data.set_points(grid_position.T)
            grid_data.add_point_data(grid_velocity=grid_velocity.T)
            fish_writers.append(vh5.VH5BlockWriter(grid_data))

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
    if has_fish:
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
                print(f"{flow_sim.time / final_time * 100:.2f}%", flush=True)

            # Log fish data only for ranks with fish
            if has_fish:
                for i, fish_writer in enumerate(fish_writers):
                    fish_writer.write_to_file(
                        os.path.join(
                            data_dir,
                            f"grid{i + index[mpi_rank]}_{int(cs.current_time() * 1000):04d}.vtkhdf",
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
            for j, flow_interaction in enumerate(flow_interactions):
                flow_interaction.compute_flow_forces_and_torques()
                if has_fish and j >= index[mpi_rank] and j < index[mpi_rank + 1]:
                    k = j - index[mpi_rank]
                    flow_forces[:, k * n_node : (k + 1) * n_node] = (
                        flow_interaction.body_flow_forces
                    )
                    flow_torques[:, k * n_elem : (k + 1) * n_elem] = (
                        flow_interaction.body_flow_torques
                    )

                flow_interaction.time_step(dt=local_rod_dt)

            if has_fish:
                cs.step_with(updated_dt=local_rod_dt)

        # Update IBM grid
        for flow_interaction in flow_interactions:
            flow_interaction()

        # Flow step
        flow_sim.time_step(dt=flow_dt, free_stream_velocity=None)

        # Update data timer
        data_timer += flow_dt

    # Finalize the simulation
    sim_time = time() - start_time
    from mpi4py import MPI

    sim_time = flow_sim.mpi_construct.grid.reduce(sim_time, op=MPI.MIN, root=0)
    if mpi_rank == 0:
        print(f"Simulation completed in {sim_time:.2f} seconds.")

    if has_fish:
        fish_logger.write_to_file(
            os.path.join(data_dir, "fish_data.npz"),
        )
    MPI.Finalize()


if __name__ == "__main__":
    nx = 128
    n_elements = nx // 4
    grid_size = (nx // 8, nx // 2, nx)
    n_max = n_elements // 4
    run_fish_swimming(
        non_dim_final_time=1.0,
        n_elem=n_elements,
        grid_size=grid_size,
        surface_grid_density_for_largest_element=n_max,
        slenderness_ratio=12.500535121608523,
        mass_ratio=1.0,
        non_dim_bending_stiffness=0.565414058870102,
        actuation_reynolds_number=7142.857142857143,
    )
