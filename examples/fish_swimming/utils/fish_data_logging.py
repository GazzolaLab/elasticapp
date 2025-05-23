import numpy as np
from elasticapp.Systems.FishModel._PyFishModel import Fish as RodType


class FishDataLogger:
    def __init__(self, rods: list[RodType], n_elem: int, flow_forces: np.ndarray):
        self.rods = rods
        self.num_rods = len(rods)
        self.num_nodes = n_elem + 1
        self.flow_forces = flow_forces.view()
        self.num_steps = 0

        self.time = []
        self.com_positions = [[]] * self.num_rods
        self.com_velocities = [[]] * self.num_rods
        self.grid_forces = [[]] * self.num_rods

    def log_data(self, time: float):
        self.time.append(time)

        for idx, rod in enumerate(self.rods):
            self.com_positions[idx].append(self._get_com_position(rod))
            self.com_velocities[idx].append(self._get_com_velocity(rod))
            self.grid_forces[idx].append(self._get_grid_force(idx))

        self.num_steps += 1

    def write_to_file(self, file_name: str):
        to_save = (np.empty((self.num_steps, 10), dtype=np.float64),) * self.num_rods

        for i in range(self.num_steps):
            for j in range(self.num_rods):
                to_save[j][i, 0] = self.time[i]
                to_save[j][i, 1:4] = self.com_positions[j][i]
                to_save[j][i, 4:7] = self.com_velocities[j][i]
                to_save[j][i, 7:10] = self.grid_forces[j][i]

        np.savez(
            file_name,
            *to_save,
            allow_pickle=True,
        )

    def _get_com_position(self, rod: RodType) -> np.ndarray:
        return np.sum(
            np.asarray(rod.position_collection) * np.asarray(rod.mass),
            axis=1,
        ) / np.sum(rod.mass)

    def _get_com_velocity(self, rod: RodType) -> np.ndarray:
        return np.sum(
            np.asarray(rod.velocity_collection) * np.asarray(rod.mass),
            axis=1,
        ) / np.sum(rod.mass)

    def _get_grid_force(self, index: int) -> np.ndarray:
        return np.mean(
            self.flow_forces[:, index * self.num_nodes : (index + 1) * self.num_nodes],
            axis=1,
        )
