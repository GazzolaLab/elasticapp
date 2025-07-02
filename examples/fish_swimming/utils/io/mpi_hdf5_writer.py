from warnings import warn

import numpy as np
import h5py
from mpi4py.MPI import Intracomm

from elasticapp.Systems.FishModel._PyFishModel import Fish


class MPIImageDataWriter:
    def __init__(
        self,
        mpi_comm: Intracomm,
        dx: float,
        global_grid_size: tuple[int, int, int],
        local_grid_size: tuple[int, int, int],
        ghost_size: int,
    ) -> None:
        self.mpi_comm = mpi_comm
        self.global_grid_size = global_grid_size
        self.origin = (0.5 * dx,) * 3
        self.spacing = (dx,) * 3

        start_idx = np.asarray(self.mpi_comm.coords) * local_grid_size
        end_idx = start_idx + local_grid_size
        self.slice = tuple(slice(start_idx[i], end_idx[i]) for i in range(3)) + (
            slice(None),
        )
        self.no_ghost_slice = (slice(None),) + (slice(ghost_size, -ghost_size),) * 3
        self.local_size_with_ghost = tuple(
            local_grid_size[i] + 2 * ghost_size for i in range(3)
        )

    def write_to_file(self, filename: str, **kwargs) -> None:
        with h5py.File(filename, "w", driver="mpio", comm=self.mpi_comm) as f:
            image_data_group = f.create_group(name="VTKHDF")
            image_data_group.attrs["Version"] = (2, 2)
            image_data_group.attrs["Type"] = np.string_("ImageData")
            image_data_group.attrs["WholeExtent"] = (
                0,
                self.global_grid_size[2] - 1,
                0,
                self.global_grid_size[1] - 1,
                0,
                self.global_grid_size[0] - 1,
            )
            image_data_group.attrs["Direction"] = (
                1.0,
                0.0,
                0.0,
                0.0,
                1.0,
                0.0,
                0.0,
                0.0,
                1.0,
            )
            image_data_group.attrs["Origin"] = self.origin
            image_data_group.attrs["Spacing"] = self.spacing

            point_data_group = image_data_group.create_group(name="PointData")
            # cell_data_group = image_data_group.create_group(name="CellData")
            # field_data_group = image_data_group.create_group(name="FieldData")

            for key, value in kwargs.items():
                if not isinstance(value, np.ndarray):
                    warn(f"Value for {key} is not a numpy array. Skipped.")
                    continue

                if len(value.shape) != 3 and len(value.shape) != 4:
                    warn(f"Value for {key} is not a 3D or 4D array. Skipped.")
                    continue

                if len(value.shape) == 3:
                    if value.shape != self.local_size_with_ghost:
                        warn(
                            f"Value for {key} has shape {value.shape} "
                            f"but expected {self.local_size_with_ghost}. Skipped."
                        )
                        continue

                    data_to_fill = point_data_group.create_dataset(
                        name=key,
                        shape=self.global_grid_size,
                        dtype=np.float64,
                    )
                    data_to_fill[self.slice[:-1]] = value[
                        self.no_ghost_slice[1:]
                    ].copy()

                if len(value.shape) == 4:
                    if value.shape != (3,) + self.local_size_with_ghost:
                        warn(
                            f"Value for {key} has shape {value.shape} "
                            f"but expected {(3,) + self.local_size_with_ghost}. Skipped."
                        )
                        continue

                    data_to_fill = point_data_group.create_dataset(
                        name=key,
                        shape=(*self.global_grid_size, 3),
                        dtype=np.float64,
                    )
                    data_to_fill[self.slice] = np.transpose(
                        value[self.no_ghost_slice], (1, 2, 3, 0)
                    ).copy()


class MPIFishWriter:
    def __init__(
        self,
        mpi_comm: Intracomm,
        fish_list: list[Fish],
        index_list: list[int],
        num_fish_element: int,
        global_num_fish: int,
    ) -> None:
        self.local_num_fish = len(fish_list)
        if self.local_num_fish != len(index_list):
            msg = "index_list must have the same length as fish_list."
            raise ValueError(msg)

        self.mpi_comm = mpi_comm
        self.local_num_fish_element = num_fish_element
        self.global_num_fish = global_num_fish

        self.fish_dicts = []
        for fish, idx in zip(fish_list, index_list):
            self.fish_dicts.append(
                {
                    "name": f"fish_{idx}",
                    "position": np.asarray(fish.position_collection),
                    "director": np.asarray(fish.director_collection),
                    "velocity": np.asarray(fish.velocity_collection),
                    "angular_velocity": np.asarray(fish.omega_collection),
                }
            )

    def write_to_file(self, filename: str) -> None:
        with h5py.File(filename, "w", driver="mpio", comm=self.mpi_comm) as f:
            root = f.create_group("root")
            for i in range(self.global_num_fish):
                group = root.create_group(f"fish_{i}")
                group.create_dataset(
                    name="position",
                    shape=(3, self.local_num_fish_element + 1),
                    dtype=np.float64,
                )
                group.create_dataset(
                    name="director",
                    shape=(3, 3, self.local_num_fish_element),
                    dtype=np.float64,
                )
                group.create_dataset(
                    name="velocity",
                    shape=(3, self.local_num_fish_element + 1),
                    dtype=np.float64,
                )
                group.create_dataset(
                    name="angular_velocity",
                    shape=(3, self.local_num_fish_element),
                    dtype=np.float64,
                )

            for fish_dict in self.fish_dicts:
                for key, value in fish_dict.items():
                    if key != "name":
                        path = "/root/" + fish_dict["name"] + "/" + key
                        root[path][...] = value
