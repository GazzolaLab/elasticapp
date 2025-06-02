import numpy as np
from sopht.simulator.immersed_body import (
    ImmersedBodyForcingGrid,
    ImmersedBodyFlowInteraction,
)
from elasticapp.Systems.FishModel._PyFishModel import Fish as RodType
from typing import Type, Optional


class FishFlowInteraction(ImmersedBodyFlowInteraction):
    """Class for Elastica++ Cosserat rod flow interaction."""

    def __init__(
        self,
        cosserat_rod: RodType,
        eul_grid_forcing_field: np.ndarray,
        eul_grid_velocity_field: np.ndarray,
        virtual_boundary_stiffness_coeff: float,
        virtual_boundary_damping_coeff: float,
        dx: float,
        grid_dim: int,
        forcing_grid_cls: Type[ImmersedBodyForcingGrid],
        real_t: type = np.float64,
        eul_grid_coord_shift: Optional[float] = None,
        interp_kernel_width: Optional[float] = None,
        enable_eul_grid_forcing_reset: bool = False,
        num_threads: int | bool = False,
        start_time: float = 0.0,
        **forcing_grid_kwargs,
    ) -> None:
        """Class initialiser."""
        n_elems = int(cosserat_rod.n_elems[0])
        body_flow_forces = np.zeros(
            (3, n_elems + 1),
        )
        body_flow_torques = np.zeros(
            (3, n_elems),
        )

        forcing_grid_kwargs["cosserat_rod"] = cosserat_rod

        # initialising super class
        super().__init__(
            eul_grid_forcing_field,
            eul_grid_velocity_field,
            body_flow_forces,
            body_flow_torques,
            forcing_grid_cls,
            virtual_boundary_stiffness_coeff,
            virtual_boundary_damping_coeff,
            dx,
            grid_dim,
            real_t,
            eul_grid_coord_shift,
            interp_kernel_width,
            enable_eul_grid_forcing_reset,
            num_threads,
            start_time,
            **forcing_grid_kwargs,
        )


class MultiFishFlowInteraction(ImmersedBodyFlowInteraction):
    """Class for Elastica++ Cosserat rod flow interaction."""

    def __init__(
        self,
        fish_list: list[RodType],
        eul_grid_forcing_field: np.ndarray,
        eul_grid_velocity_field: np.ndarray,
        virtual_boundary_stiffness_coeff: float,
        virtual_boundary_damping_coeff: float,
        dx: float,
        grid_dim: int,
        forcing_grid_cls: Type[ImmersedBodyForcingGrid],
        real_t: type = np.float64,
        eul_grid_coord_shift: Optional[float] = None,
        interp_kernel_width: Optional[float] = None,
        enable_eul_grid_forcing_reset: bool = False,
        num_threads: int | bool = False,
        start_time: float = 0.0,
        **forcing_grid_kwargs,
    ) -> None:
        """Class initialiser."""
        n_elems = n_nodes = 0
        for fish in fish_list:
            n_elems += int(fish.n_elems[0])
            n_nodes += int(fish.n_elems[0]) + 1

        body_flow_forces = np.zeros((3, n_nodes), dtype=real_t)
        body_flow_torques = np.zeros((3, n_elems), dtype=real_t)

        forcing_grid_kwargs["fish_list"] = fish_list

        # initialising super class
        super().__init__(
            eul_grid_forcing_field,
            eul_grid_velocity_field,
            body_flow_forces,
            body_flow_torques,
            forcing_grid_cls,
            virtual_boundary_stiffness_coeff,
            virtual_boundary_damping_coeff,
            dx,
            grid_dim,
            real_t,
            eul_grid_coord_shift,
            interp_kernel_width,
            enable_eul_grid_forcing_reset,
            num_threads,
            start_time,
            **forcing_grid_kwargs,
        )
