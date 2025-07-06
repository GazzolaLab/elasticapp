import os
import click
from time import time
from typing import Optional

import numpy as np
from mpi4py import MPI
from sopht.utils import get_real_t
from sopht_mpi.simulator import UnboundedFlowSimulator3D
from sopht_mpi.simulator.immersed_body import ImmersedBodyFlowInteractionMPI

import fish_utils as fu
import fish_utils.io as fuio

from elasticapp.Systems import FishModel  # noqa: F401
from elasticapp.CaseStudies.PyFishCaseStudy import FishCaseStudy
from elasticapp.Arrays._PyArrays import Matrix


def reload_flow_interaction(
    flow_interaction: ImmersedBodyFlowInteractionMPI, time: float
) -> None:
    flow_interaction.time = time
    flow_interaction.forcing_grid.compute_lag_grid_position_field()
    flow_interaction.forcing_grid.compute_lag_grid_velocity_field()
    flow_interaction.mpi_lagrangian_field_communicator.map_lagrangian_nodes_based_on_position(
        flow_interaction.forcing_grid.position_field
    )
    flow_interaction.local_num_lag_nodes = (
        flow_interaction.mpi_lagrangian_field_communicator.local_num_lag_nodes
    )
    flow_interaction._init_local_buffers(flow_interaction.local_num_lag_nodes)
    flow_interaction.mpi_lagrangian_field_communicator.scatter_global_field(
        local_lag_field=flow_interaction.local_lag_grid_position_mismatch_field,
        global_lag_field=flow_interaction.global_lag_grid_position_mismatch_field,
    )
    flow_interaction.mpi_lagrangian_field_communicator.scatter_global_field(
        local_lag_field=flow_interaction.local_lag_grid_velocity_mismatch_field,
        global_lag_field=flow_interaction.global_lag_grid_velocity_mismatch_field,
    )


@click.command()
@click.option(
    "--non-dim-final-time",
    "-t",
    type=float,
    default=1.0,
    help="Non-dimensional final time of the simulation. Scaled by the period.",
)
@click.option(
    "--n-elem", "-n", type=int, default=16, help="Number of elements per fish."
)
@click.option(
    "--grid-size-x",
    "-g",
    type=int,
    default=192,
    help="Number of flow grid points in the x-direction.",
)
@click.option(
    "--surface-grid-density-for-largest-element",
    "-s",
    type=int,
    default=4,
    help="Surface grid density for the largest element.",
)
@click.option(
    "--slenderness-ratio",
    "-r",
    type=float,
    default=12.5,
    help="Slenderness ratio of the fish.",
)
@click.option(
    "--mass_ratio",
    "-m",
    type=float,
    default=1.0,
    help="Mass ratio of the fish to the fluid.",
)
@click.option(
    "--non-dim-bending-stiffness",
    "-b",
    type=float,
    default=0.565414058870102,
    help="Non-dimensional bending stiffness of the fish.",
)
@click.option(
    "--actuation-reynolds-number",
    "-a",
    type=float,
    default=7142.857142857143,
    help="Actuation Reynolds number for the fish.",
)
@click.option(
    "--coupling-stiffness",
    "-k",
    type=float,
    default=-32000.0,
    help="Coupling stiffness for the immersed boundary method.",
)
@click.option(
    "--coupling-damping",
    "-d",
    type=float,
    default=-32.0,
    help="Coupling damping for the immersed boundary method.",
)
@click.option(
    "--precision",
    "-p",
    type=str,
    default="double",
    help="Precision of the simulation (single or double).",
)
@click.option(
    "--rank-distribution",
    "-R",
    nargs=3,
    type=(int, int, int),
    default=None,
    help="Rank distribution for MPI.",
)
@click.option(
    "--restart/--no-restart",
    default=False,
    help="Flag to restart the simulation from the last saved data in data_dir.",
)
@click.option(
    "--data-dir",
    "-d",
    type=str,
    default=None,
    help="Directory to save (and load) the simulation data.",
)
@click.option(
    "--fish-head-positions-file",
    "-h",
    type=str,
    default="assets/fish_head_positions.txt",
    help="File containing the fish head positions.",
)
def run_fish_swimming_mpi(
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
    restart: bool = False,
    data_dir: str | None = None,
    fish_head_positions_file: str = "assets/fish_head_positions.txt",
) -> None:
    # ================ Global parameters ================
    grid_dim = 3
    real_t = get_real_t(precision)
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
    origins = np.loadtxt(fish_head_positions_file, delimiter=",")

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
    mpi_size = flow_sim.mpi_construct.size

    # Get the data directory and broadcast
    if mpi_rank == 0:
        data_dir = fu.get_data_path(data_dir)
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
    num_proc_with_fish = np.count_nonzero(num_fish_per_rank)

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
        elastic_fish_list, virtual_fish_list, flow_forces, flow_torques = (
            cs.communicate()
        )
        flow_forces = np.asarray(flow_forces)
        flow_torques = np.asarray(flow_torques)
    else:
        elastic_fish_list = []
        virtual_fish_list = []
        flow_forces = None
        flow_torques = None

    # ============== Immersed boundary ===============
    flow_interactions = []

    # Each rank has flow_interaction objects for all fishes
    # But the only functional ones are those that corresponds to the fish
    # on this rank.
    auto_ghosting = True
    for rank_idx in range(num_proc_with_fish):
        flow_interactions.append(
            fu.MultiFishFlowInteractionMPI(
                mpi_construct=flow_sim.mpi_construct,
                mpi_ghost_exchange_communicator=flow_sim.mpi_ghost_exchange_communicator,
                fish_list=elastic_fish_list,
                num_fish=num_fish_per_rank[rank_idx],
                n_elem_per_fish=n_elem,
                eul_grid_forcing_field=flow_sim.eul_grid_forcing_field,
                eul_grid_velocity_field=flow_sim.velocity_field,
                virtual_boundary_stiffness_coeff=coupling_stiffness,
                virtual_boundary_damping_coeff=coupling_damping,
                dx=flow_sim.dx,
                grid_dim=grid_dim,
                forcing_grid_cls=fu.MultiFishForcingGrid,
                surface_grid_density_for_largest_element=surface_grid_density_for_largest_element,
                master_rank=rank_idx,
                auto_ghosting=auto_ghosting,
            )
        )
        auto_ghosting = False
    for idx, flow_interaction in enumerate(flow_interactions):
        if idx == mpi_rank:
            assert not flow_interaction.is_empty()
        else:
            assert flow_interaction.is_empty()

    print(f"Rank: {mpi_rank}, Number of fish: {num_fish_per_rank[mpi_rank]}")

    # ================ IO ================
    if has_fish:
        num_lag_node_per_fish = (
            flow_interactions[mpi_rank].forcing_grid.num_lag_nodes
            // num_fish_per_rank[mpi_rank]
        )
        lag_fields = {
            "position_mismatch": flow_interactions[
                mpi_rank
            ].global_lag_grid_position_mismatch_field,
            "velocity_mismatch": flow_interactions[
                mpi_rank
            ].global_lag_grid_velocity_mismatch_field,
        }
    else:
        num_lag_node_per_fish = 0
        lag_fields = {
            "position_mismatch": None,
            "velocity_mismatch": None,
        }

    num_lag_node_per_fish = flow_sim.mpi_construct.grid.bcast(
        num_lag_node_per_fish, root=0
    )
    fish_writer = fuio.MPIFishIO(
        mpi_comm=flow_sim.mpi_construct.grid,
        fish_list=elastic_fish_list,
        virtual_fish_list=virtual_fish_list,
        index_list=list(range(index[mpi_rank], index[mpi_rank + 1])),
        num_fish_element=n_elem,
        num_lag_nodes=num_lag_node_per_fish,
        global_num_fish=num_fish_total,
        **lag_fields,
    )

    flow_writer = fuio.MPIImageDataIO(
        mpi_comm=flow_sim.mpi_construct.grid,
        dx=flow_sim.dx,
        global_grid_size=flow_sim.grid_size,
        local_grid_size=flow_sim.mpi_construct.local_grid_size,
        ghost_size=flow_sim.ghost_size,
    )

    if restart:
        fish_file = fu.get_most_recent_data_file(data_dir, "fish_*.h5")
        flow_file = fu.get_most_recent_data_file(data_dir, "flow_*.vtkhdf")
        fish_time = fish_writer.read_from_file(fish_file)
        flow_sim.time = flow_writer.read_from_file(
            flow_file,
            velocity=flow_sim.velocity_field,
            vorticity=flow_sim.vorticity_field,
        )
        if has_fish:
            cs.set_current_time(fish_time)
        if mpi_rank == 0:
            print("Loaded fish data from ", fish_file)
            print("Loaded flow data from ", flow_file)
        
        for flow_interaction in flow_interactions:
            reload_flow_interaction(flow_interaction, fish_time)
            
    # ================ Simulation ================
    final_time = non_dim_final_time * period
    data_timer_limit = period / 20.0
    data_timer = 0.0
    if has_fish:
        rod_dt = cs.current_dt()
    else:
        rod_dt = None

    rod_dt = flow_sim.mpi_construct.grid.bcast(rod_dt, root=0)

    flow_time = 0.0
    rod_time = 0.0
    fsi_time = 0.0
    io_time = 0.0

    start_time = time()
    while flow_sim.time < final_time:
        if data_timer == 0.0 or data_timer > data_timer_limit:
            data_timer = 0.0

            # Print progress only on rank 0
            if mpi_rank == 0:
                # print(f"{flow_sim.time / final_time * 100:.2f}%", flush=True)
                print(flow_sim.time, io_time, flow_time, rod_time, fsi_time, flush=True)

            a = time()

            # Log fish data (Not VTKHDF. Post-processing required)
            fish_time = cs.current_time() if has_fish else 0.0
            fish_time = flow_sim.mpi_construct.grid.bcast(fish_time, root=0)
            fish_writer.write_to_file(
                filename=os.path.join(
                    data_dir, f"fish_{int(flow_sim.time * 1000):04d}.h5"
                ),
                time=fish_time,
            )

            # Log flow data for all ranks (HDF5-MPI)
            flow_writer.write_to_file(
                filename=os.path.join(
                    data_dir, f"flow_{int(flow_sim.time * 1000):04d}.vtkhdf"
                ),
                time=flow_sim.time,
                velocity=flow_sim.velocity_field,
                vorticity=flow_sim.vorticity_field,
            )
            io_time += time() - a

        # Compute time steps
        flow_dt = min(
            flow_sim.compute_stable_timestep(dt_prefac=0.25, precision=precision),
            rod_dt * 100,
        )
        rod_time_steps = max(int(flow_dt / rod_dt), 1)
        local_rod_dt = flow_dt / rod_time_steps

        # Rod + IBM steps
        for _ in range(rod_time_steps):
            a = time()
            for flow_interaction in flow_interactions:
                flow_interaction.compute_flow_forces_and_torques()
            if has_fish:
                flow_forces[...] = flow_interactions[mpi_rank].body_flow_forces
                flow_torques[...] = flow_interactions[mpi_rank].body_flow_torques
            flow_interaction.time_step(dt=local_rod_dt)
            fsi_time += time() - a

            if has_fish:
                a = time()
                cs.step_with(updated_dt=local_rod_dt)
                rod_time += time() - a

        # Update IBM grid
        a = time()
        for flow_interaction in flow_interactions:
            flow_interaction()
            flow_interaction.update_buffers(
                flow_interaction.forcing_grid.position_field
            )
        fsi_time += time() - a

        # Flow step
        a = time()
        # flow_sim.time_step(dt=flow_dt, free_stream_velocity=None)
        flow_sim.time += flow_dt
        flow_time += time() - a

        # Update data timer
        data_timer += flow_dt

        flow_time = flow_sim.mpi_construct.grid.allreduce(flow_time, op=MPI.MAX)
        rod_time = flow_sim.mpi_construct.grid.allreduce(rod_time, op=MPI.MAX)
        fsi_time = flow_sim.mpi_construct.grid.allreduce(fsi_time, op=MPI.MAX)
        io_time = flow_sim.mpi_construct.grid.allreduce(io_time, op=MPI.MAX)

    # Finalize the simulation
    sim_time = time() - start_time

    sim_time = flow_sim.mpi_construct.grid.reduce(sim_time, op=MPI.MAX, root=0)
    if mpi_rank == 0:
        print(f"Simulation completed in {sim_time:.2f} seconds.")

    MPI.Finalize()


if __name__ == "__main__":
    run_fish_swimming_mpi()
