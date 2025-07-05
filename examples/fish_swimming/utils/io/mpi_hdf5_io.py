from warnings import warn

import numpy as np
import h5py
from mpi4py.MPI import Intracomm

from elasticapp.Systems.FishModel._PyFishModel import Fish


class MPIImageDataIO:
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

    def write_to_file(self, filename: str, time: float | np.float64, **kwargs) -> None:
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
            field_data_group = image_data_group.create_group(name="FieldData")

            time_data = field_data_group.create_dataset(
                name="Time",
                shape=(1,),
                dtype=np.float64,
            )
            if self.mpi_comm.Get_rank() == 0:
                time_data[0] = np.float64(time)

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

    def read_from_file(self, filename: str, **kwargs) -> np.float64:
        time = np.float64(-1.0)
        with h5py.File(filename, "r", driver="mpio", comm=self.mpi_comm) as f:
            try:
                time = f["/VTKHDF/FieldData/Time"][0]
            except KeyError:
                msg = (
                    f"Process {self.mpi_comm.Get_rank()}: "
                    f"Time not found in FiledData of file {filename}"
                )
                raise KeyError(msg)

            for field_name, field_view in kwargs.items():
                path = "/VTKHDF/PointData/" + field_name
                try:
                    dataset = f[path]
                except KeyError:
                    msg = (
                        f"Process {self.mpi_comm.Get_rank()}: "
                        f"Dataset '{field_name}' not found in PointData of file {filename}"
                    )
                    raise KeyError(msg)

                if dataset.shape != self.global_grid_size and dataset.shape != (
                    *self.global_grid_size,
                    3,
                ):
                    msg = (
                        f"Process {self.mpi_comm.Get_rank()}: "
                        f"Dataset '{field_name}' has shape {dataset.shape} "
                        f"but expected {self.global_grid_size} or (*{self.global_grid_size}, 3)."
                    )
                    raise ValueError(msg)

                if len(dataset.shape) == 3:
                    field_view[self.no_ghost_slice[1:]] = dataset[self.slice[:-1]]

                elif len(dataset.shape) == 4:
                    field_view[self.no_ghost_slice] = np.transpose(
                        dataset[self.slice], (3, 0, 1, 2)
                    )

        return time


class MPIFishIO:
    def __init__(
        self,
        mpi_comm: Intracomm,
        fish_list: list[Fish],
        virtual_fish_list: list[Fish],
        index_list: list[int],
        num_fish_element: int,
        num_lag_nodes: int,
        global_num_fish: int,
        **lag_fields: dict[str, np.ndarray],
    ) -> None:
        self.local_num_fish = len(fish_list)
        if self.local_num_fish != len(index_list):
            msg = "index_list must have the same length as fish_list."
            raise ValueError(msg)
        if self.local_num_fish != len(virtual_fish_list):
            msg = "virtual_fish_list must have the same length as fish_list."
            raise ValueError(msg)

        for _, lag_field in lag_fields.items():
            if lag_field.shape != (3, self.local_num_fish * num_lag_nodes):
                msg = (
                    f"Incompatible lag field shape: {lag_field.shape}. "
                    f"Expected shape: (3, {self.local_num_fish * num_lag_nodes})."
                )
                raise ValueError(msg)

        self.mpi_comm = mpi_comm
        self.local_num_fish_element = num_fish_element
        self.global_num_fish = global_num_fish
        self.num_lag_nodes = num_lag_nodes

        self.fish_dicts = []
        for i in range(self.local_num_fish):
            self.fish_dicts.append(
                {
                    "name": f"fish_{index_list[i]}",
                    "position": np.asarray(fish_list[i].position_collection),
                    "director": np.asarray(fish_list[i].director_collection),
                    "velocity": np.asarray(fish_list[i].velocity_collection),
                    "angular_velocity": np.asarray(fish_list[i].omega_collection),
                    "v_position": np.asarray(virtual_fish_list[i].position_collection),
                    "v_director": np.asarray(virtual_fish_list[i].director_collection),
                    "v_velocity": np.asarray(virtual_fish_list[i].velocity_collection),
                    "v_angular_velocity": np.asarray(virtual_fish_list[i].omega_collection),
                }
            )
            for key, value in lag_fields.items():
                if key in [
                    "name",
                    "position",
                    "director",
                    "velocity",
                    "angular_velocity",
                    "v_position",
                    "v_director",
                    "v_velocity",
                    "v_angular_velocity",
                ]:
                    warn(
                        f"Lagrangian field '{key}' is already used in fish_dict. Skipped."
                    )

                else:
                    self.fish_dicts[-1][key] = value[
                        :, i * num_lag_nodes : (i + 1) * num_lag_nodes
                    ].view()

        self.lag_field_names = list(lag_fields.keys())

    def write_to_file(self, filename: str, time: float | np.float64) -> None:
        with h5py.File(filename, "w", driver="mpio", comm=self.mpi_comm) as f:
            root = f.create_group("root")
            time_data = root.create_dataset(
                name="Time",
                shape=(1,),
                dtype=np.float64,
            )
            if self.mpi_comm.Get_rank() == 0:
                time_data[0] = np.float64(time)

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
                group.create_dataset(
                    name="v_position",
                    shape=(3, self.local_num_fish_element + 1),
                    dtype=np.float64,
                )
                group.create_dataset(
                    name="v_director",
                    shape=(3, 3, self.local_num_fish_element),
                    dtype=np.float64,
                )
                group.create_dataset(
                    name="v_velocity",
                    shape=(3, self.local_num_fish_element + 1),
                    dtype=np.float64,
                )
                group.create_dataset(
                    name="v_angular_velocity",
                    shape=(3, self.local_num_fish_element),
                    dtype=np.float64,
                )
                
                for lag_field_name in self.lag_field_names:
                    group.create_dataset(
                        name=lag_field_name,
                        shape=(3, self.num_lag_nodes),
                        dtype=np.float64,
                    )

            for fish_dict in self.fish_dicts:
                for key, value in fish_dict.items():
                    if key != "name":
                        path = "/root/" + fish_dict["name"] + "/" + key
                        root[path][...] = value

    def read_from_file(self, filename: str) -> np.float64:
        time = np.float64(-1.0)
        with h5py.File(filename, "r", driver="mpio", comm=self.mpi_comm) as f:
            try:
                time = f["/root/Time"][0]
            except KeyError:
                msg = (
                    f"Process {self.mpi_comm.Get_rank()}: "
                    f"Time not found in file {filename} at /root/Time"
                )
                raise KeyError(msg)

            for fish_dict in self.fish_dicts:
                path = "/root/" + fish_dict["name"]
                try:
                    group = f[path]
                except KeyError:
                    msg = (
                        f"Process {self.mpi_comm.Get_rank()}: "
                        f"Group '{fish_dict['name']}' not found in file {filename} at {path}"
                    )
                    raise KeyError(msg)

                for key, value in fish_dict.items():
                    if key != "name":
                        try:
                            value[...] = group[key]
                        except KeyError:
                            msg = (
                                f"Process {self.mpi_comm.Get_rank()}: "
                                f"Dataset '{key}' not found in group in file {filename} at {path}/{key}"
                            )
                            raise KeyError(msg)

        return time
