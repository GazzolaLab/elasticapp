import numpy as np
from sopht.simulator.immersed_body import (
    ImmersedBodyForcingGrid,
)
from sopht_mpi.simulator.immersed_body import (
    EmptyForcingGrid,
    ImmersedBodyFlowInteractionMPI,
)
from sopht_mpi.utils import (
    MPIConstruct2D,
    MPIConstruct3D,
    MPIGhostCommunicator2D,
    MPIGhostCommunicator3D,
)
from elasticapp.Systems.FishModel._PyFishModel import Fish as RodType
from typing import Type, Optional


class FishFlowInteractionMPI(ImmersedBodyFlowInteractionMPI):
    """Class for Elastica++ Cosserat rod flow interaction."""

    def __init__(
        self,
        mpi_construct: "MPIConstruct2D | MPIConstruct3D",
        mpi_ghost_exchange_communicator: "MPIGhostCommunicator2D | MPIGhostCommunicator3D",
        cosserat_rod: RodType | int,
        eul_grid_forcing_field: np.ndarray,
        eul_grid_velocity_field: np.ndarray,
        virtual_boundary_stiffness_coeff: float,
        virtual_boundary_damping_coeff: float,
        dx: float,
        grid_dim: int,
        forcing_grid_cls: Type[ImmersedBodyForcingGrid],
        eul_grid_coord_shift: Optional[float] = None,
        interp_kernel_width: Optional[float] = None,
        enable_eul_grid_forcing_reset: bool = False,
        start_time: float = 0.0,
        master_rank: int = 0,
        assume_data_locality: bool = False,
        auto_ghosting: bool = True,
        **forcing_grid_kwargs,
    ) -> None:
        self.master_rank = master_rank
        if mpi_construct.rank == self.master_rank:
            if forcing_grid_cls is None:
                raise ValueError(
                    "forcing_grid_cls must be provided when master_rank is 0"
                )
            self.forcing_grid = forcing_grid_cls(
                grid_dim=grid_dim,
                cosserat_rod=cosserat_rod,
                **forcing_grid_kwargs,
            )
            n_elems = int(cosserat_rod.n_elems[0])
            self.body_flow_forces = np.zeros((3, n_elems + 1))
            self.body_flow_torques = np.zeros((3, n_elems))
        else:
            self.forcing_grid = EmptyForcingGrid(grid_dim=grid_dim)
            self.body_flow_forces = np.zeros((3, cosserat_rod + 1))
            self.body_flow_torques = np.zeros((3, cosserat_rod))

        # initialising super class
        super().__init__(
            mpi_construct=mpi_construct,
            mpi_ghost_exchange_communicator=mpi_ghost_exchange_communicator,
            eul_grid_forcing_field=eul_grid_forcing_field,
            eul_grid_velocity_field=eul_grid_velocity_field,
            virtual_boundary_stiffness_coeff=virtual_boundary_stiffness_coeff,
            virtual_boundary_damping_coeff=virtual_boundary_damping_coeff,
            dx=dx,
            grid_dim=grid_dim,
            eul_grid_coord_shift=eul_grid_coord_shift,
            interp_kernel_width=interp_kernel_width,
            enable_eul_grid_forcing_reset=enable_eul_grid_forcing_reset,
            start_time=start_time,
            assume_data_locality=assume_data_locality,
            auto_ghosting=auto_ghosting,
        )
