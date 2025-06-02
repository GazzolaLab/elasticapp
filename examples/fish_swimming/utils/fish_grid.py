from elastica._linalg import _batch_cross, _batch_matvec, _batch_matrix_transpose
from sopht.simulator.immersed_body import ImmersedBodyForcingGrid
from elasticapp.Systems.FishModel._PyFishModel import Fish

import numpy as np
from numba import njit


class FishSurfaceForcingGrid(ImmersedBodyForcingGrid):
    def __init__(
        self,
        grid_dim: int,
        cosserat_rod: Fish,
        surface_grid_density_for_largest_element: int,
    ) -> None:
        if grid_dim != 3:
            raise ValueError(
                "Invalid grid dimensions. Cosserat rod surface forcing grid is only "
                "defined for grid_dim=3"
            )
        self.position_collection = np.asarray(cosserat_rod.position_collection)
        self.velocity_collection = np.asarray(cosserat_rod.velocity_collection)
        self.omega_collection = np.asarray(cosserat_rod.omega_collection)
        self.director_collection = np.asarray(cosserat_rod.director_collection)
        self.dilatation = np.asarray(cosserat_rod.dilatation)
        self.lengths = np.asarray(cosserat_rod.lengths)

        self.height = np.asarray(cosserat_rod.semi_major_axis)

        # Surface grid density at the arm maximum radius
        self.width = np.asarray(cosserat_rod.semi_minor_axis)
        self.surface_grid_density_for_largest_element = (
            surface_grid_density_for_largest_element
        )

        # area = 4*np.pi * self.width * self.height
        # self.surface_grid_points = np.rint(area / np.max(area) * surface_grid_density_for_largest_element).astype(int)
        # Srinivasa Ramanujan approximation for ellipse circumference
        # https://en.wikipedia.org/wiki/Ellipse
        self.circumference = np.pi * (
            3 * (self.width + self.height)
            - np.sqrt((3 * self.width + self.height) * (self.width + 3 * self.height))
        )
        self.surface_grid_points = np.rint(
            self.circumference
            / np.max(self.circumference)
            * surface_grid_density_for_largest_element
        ).astype(int)

        # If there are less than 1 point then set it equal to 1 since we will place
        # it on the element center.
        self.surface_grid_points[np.where(self.surface_grid_points < 3)[0]] = 1
        num_lag_nodes = self.surface_grid_points.sum()
        super().__init__(grid_dim, num_lag_nodes)
        self.n_elements = len(self.width)

        self.surface_point_rotation_angle_list = []
        for i in range(self.n_elements):
            if self.surface_grid_points[i] > 1:
                # If there are more than one point on the surface then compute
                # the angle of these points. Surface points are on the local frame
                self.surface_point_rotation_angle_list.append(
                    np.linspace(
                        0, 2 * np.pi, self.surface_grid_points[i], endpoint=False
                    )
                )
            else:
                # If there is only one point, then that point is on the element
                # center so pass empty array.
                self.surface_point_rotation_angle_list.append(np.array([]))

        # Since lag grid points are on the surface, for each node we need to compute moment arm.
        self.moment_arm = np.zeros_like(self.position_field)

        # Depending on the rod taper number of surface points can vary between elements,
        # so we need start_idx and end_idx to slice grid points according to element they belong.
        self.start_idx = np.zeros((self.n_elements), dtype=int)
        self.end_idx = np.zeros((self.n_elements), dtype=int)

        start_idx_temp = 0
        end_idx_temp = 0
        for i in range(self.n_elements):
            self.start_idx[i] = start_idx_temp
            start_idx_temp += self.surface_grid_points[i]
            end_idx_temp += self.surface_grid_points[i]
            self.end_idx[i] = end_idx_temp

        self.local_frame_surface_points = np.zeros_like(self.position_field)

        # Compute surface(grid) point positions on local frame for each element.
        # If there is one grid point then that point is on the element center
        # so just pass 0.0
        for i, surface_point_rotation_angle in enumerate(
            self.surface_point_rotation_angle_list
        ):
            if surface_point_rotation_angle.size == 0:
                # This is true if there is only one point for element, and it is on element center.
                self.local_frame_surface_points[
                    :, self.start_idx[i] : self.end_idx[i]
                ] = 0.0
            else:
                self.local_frame_surface_points[
                    0, self.start_idx[i] : self.end_idx[i]
                ] = self.height[i] * np.cos(surface_point_rotation_angle)
                self.local_frame_surface_points[
                    1, self.start_idx[i] : self.end_idx[i]
                ] = self.width[i] * np.sin(surface_point_rotation_angle)

        # some caching stuff
        self.rod_director_collection_transpose = np.zeros_like(self.director_collection)
        self.rod_element_position = np.zeros((self.grid_dim, self.n_elements))
        self.rod_element_velocity = np.zeros_like(self.rod_element_position)
        self.rod_element_global_frame_omega = np.zeros_like(self.rod_element_position)

        self.grid_point_director_transpose = np.zeros((3, 3, self.num_lag_nodes))
        self.grid_point_radius = np.zeros((self.num_lag_nodes))
        self.grid_point_dilatation = np.zeros((self.num_lag_nodes))
        self.grid_point_omega = np.zeros_like(self.position_field)
        self.lag_grid_torque_field = np.zeros_like(self.position_field)

        # to ensure position/velocity are consistent during initialisation
        self.compute_lag_grid_position_field()
        self.compute_lag_grid_velocity_field()

    def compute_lag_grid_position_field(self) -> None:
        """Computes location of forcing grid for the Cosserat rod"""

        self.rod_element_position[...] = 0.5 * (
            self.position_collection[..., 1:] + self.position_collection[..., :-1]
        )

        # Cache rod director collection transpose since it will be used to
        # compute velocity field.
        self.rod_director_collection_transpose[...] = _batch_matrix_transpose(
            self.director_collection
        )

        # Broadcast rod properties to the grid points.
        for i in range(self.n_elements):
            self.grid_point_director_transpose[
                :, :, self.start_idx[i] : self.end_idx[i]
            ] = self.rod_director_collection_transpose[:, :, i : i + 1]
            self.grid_point_dilatation[
                self.start_idx[i] : self.end_idx[i]
            ] = self.dilatation[i]
            self.position_field[
                :, self.start_idx[i] : self.end_idx[i]
            ] = self.rod_element_position[:, i : i + 1]

        # Compute the moment arm or distance from the element center
        # for each grid point.
        self.moment_arm[:] = _batch_matvec(
            self.grid_point_director_transpose, self.local_frame_surface_points
        )

        # Surface positions are moment_arm + element center position
        self.position_field += self.moment_arm

    def compute_lag_grid_velocity_field(self) -> None:
        """Computes velocity of forcing grid points for the Cosserat rod"""

        # Element velocity
        self.rod_element_velocity[...] = 0.5 * (
            self.velocity_collection[:, 1:] + self.velocity_collection[:, :-1]
        )
        # Element angular velocity
        self.rod_element_global_frame_omega[...] = _batch_matvec(
            self.rod_director_collection_transpose,
            self.omega_collection,
        )

        # Broadcast rod properties to the grid points.
        for i in range(self.n_elements):
            self.grid_point_omega[
                :, self.start_idx[i] : self.end_idx[i]
            ] = self.rod_element_global_frame_omega[:, i : i + 1]
            self.velocity_field[
                :, self.start_idx[i] : self.end_idx[i]
            ] = self.rod_element_velocity[:, i : i + 1]

        # v_elem + omega X moment_arm
        self.velocity_field += _batch_cross(self.grid_point_omega, self.moment_arm)

    def transfer_forcing_from_grid_to_body(
        self,
        body_flow_forces: np.ndarray,
        body_flow_torques: np.ndarray,
        lag_grid_forcing_field: np.ndarray,
    ) -> None:
        """Transfer forcing from lagrangian forcing grid to the cosserat rod"""
        body_flow_forces[...] = 0.0
        body_flow_torques[...] = 0.0

        # negative sign due to Newtons third law
        for i in range(self.n_elements):
            body_forces_on_elems = np.sum(
                lag_grid_forcing_field[:, self.start_idx[i] : self.end_idx[i]], axis=1
            )
            body_flow_forces[:, i] -= 0.5 * body_forces_on_elems
            body_flow_forces[:, i + 1] -= 0.5 * body_forces_on_elems

        # negative sign due to Newtons third law
        # torque generated by all lagrangian points are
        self.lag_grid_torque_field[...] = _batch_cross(
            self.moment_arm, -lag_grid_forcing_field
        )

        # Update body torques
        # convert global to local frame
        for i in range(self.n_elements):
            body_flow_torques[:, i] = self.director_collection[:, :, i] @ np.sum(
                self.lag_grid_torque_field[:, self.start_idx[i] : self.end_idx[i]],
                axis=1,
            )

    def get_maximum_lagrangian_grid_spacing(self) -> float:
        """Get the maximum Lagrangian grid spacing"""
        return np.amax(
            [
                self.lengths,
                self.circumference / self.surface_grid_density_for_largest_element,
            ]
        )


class MultiFishSurfaceForcingGrid(ImmersedBodyForcingGrid):
    def __init__(
        self,
        grid_dim: int,
        fish_list: list[Fish],
        surface_grid_density_for_largest_element: int,
    ) -> None:
        self.num_fish = len(fish_list)
        if self.num_fish == 0:
            raise ValueError("No fish provided to the MultiFishSurfaceForcingGrid.")

        self.sub_grids = [
            FishSurfaceForcingGrid(
                grid_dim=grid_dim,
                cosserat_rod=fish,
                surface_grid_density_for_largest_element=surface_grid_density_for_largest_element,
            )
            for fish in fish_list
        ]

        num_lag_nodes = 0
        for sub_grid in self.sub_grids:
            num_lag_nodes += sub_grid.num_lag_nodes

        super().__init__(grid_dim, num_lag_nodes)

        start = 0

        for i, sub_grid in enumerate(self.sub_grids):
            end = start + sub_grid.num_lag_nodes

            self.position_field[:, start:end] = sub_grid.position_field
            self.velocity_field[:, start:end] = sub_grid.velocity_field

            del sub_grid.position_field
            del sub_grid.velocity_field

            sub_grid.position_field = self.position_field[:, start:end].view()
            sub_grid.velocity_field = self.velocity_field[:, start:end].view()

            start = end

    def compute_lag_grid_position_field(self) -> None:
        for sub_grid in self.sub_grids:
            sub_grid.compute_lag_grid_position_field()

    def compute_lag_grid_velocity_field(self) -> None:
        for sub_grid in self.sub_grids:
            sub_grid.compute_lag_grid_velocity_field()

    def transfer_forcing_from_grid_to_body(
        self,
        body_flow_forces: np.ndarray,
        body_flow_torques: np.ndarray,
        lag_grid_forcing_field: np.ndarray,
    ) -> None:
        start_elem = start_node = start_lag = 0
        for sub_grid in self.sub_grids:
            end_elem = start_elem + sub_grid.n_elements
            end_node = start_node + sub_grid.n_elements + 1
            end_lag = start_lag + sub_grid.num_lag_nodes
            sub_grid.transfer_forcing_from_grid_to_body(
                body_flow_forces=body_flow_forces[:, start_node:end_node].view(),
                body_flow_torques=body_flow_torques[:, start_elem:end_elem].view(),
                lag_grid_forcing_field=lag_grid_forcing_field[
                    :, start_lag:end_lag
                ].view(),
            )
            start_elem = end_elem
            start_node = end_node
            start_lag = end_lag

    def get_maximum_lagrangian_grid_spacing(self) -> float:
        max_spacing = 0.0
        for sub_grid in self.sub_grids:
            max_spacing = max(
                max_spacing, sub_grid.get_maximum_lagrangian_grid_spacing()
            )

        return max_spacing
