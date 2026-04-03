from warnings import warn

import numpy as np
import h5py

from elasticapp.Systems.FishModel._PyFishModel import Fish


class ImageDataIO:
    def __init__(
        self,
        dx: float,
        grid_size: tuple[int, int, int],
    ) -> None:
        self.grid_size = grid_size
        self.origin = (0.5 * dx,) * 3
        self.spacing = (dx,) * 3

    def write_to_file(self, filename: str, time: float | np.float64, **kwargs) -> None:
        with h5py.File(filename, "w") as f:
            image_data_group = f.create_group(name="VTKHDF")
            image_data_group.attrs["Version"] = (2, 2)
            image_data_group.attrs["Type"] = "ImageData".encode("ascii")
            image_data_group.attrs["WholeExtent"] = (
                0,
                self.grid_size[2] - 1,
                0,
                self.grid_size[1] - 1,
                0,
                self.grid_size[0] - 1,
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

            field_data_group.create_dataset(
                name="Time",
                shape=(1,),
                dtype=np.float64,
                data=np.array([time], dtype=np.float64),
            )

            for key, value in kwargs.items():
                if not isinstance(value, np.ndarray):
                    warn(f"Value for {key} is not a numpy array. Skipped.")
                    continue

                if len(value.shape) != 3 and len(value.shape) != 4:
                    warn(f"Value for {key} is not a 3D or 4D array. Skipped.")
                    continue

                if len(value.shape) == 3:
                    if value.shape != self.grid_size:
                        warn(
                            f"Value for {key} has shape {value.shape} "
                            f"but expected {self.grid_size}. Skipped."
                        )
                        continue

                    point_data_group.create_dataset(
                        name=key,
                        shape=self.grid_size,
                        dtype=np.float64,
                        data=value.copy(),
                    )

                if len(value.shape) == 4:
                    if value.shape != (3,) + self.grid_size:
                        warn(
                            f"Value for {key} has shape {value.shape} "
                            f"but expected {(3,) + self.grid_size}. Skipped."
                        )
                        continue

                    point_data_group.create_dataset(
                        name=key,
                        shape=(*self.grid_size, 3),
                        dtype=np.float64,
                        data=np.transpose(value, (1, 2, 3, 0)).copy(),
                    )

    def read_from_file(self, filename: str, **kwargs) -> np.float64:
        time = np.float64(-1.0)
        with h5py.File(filename, "r", driver="mpio", comm=self.mpi_comm) as f:
            time = f["/VTKHDF/FieldData/Time"][0]
            for field_name, field_view in kwargs.items():
                path = "/VTKHDF/PointData/" + field_name
                try:
                    dataset = f[path]
                except KeyError:
                    msg = f"Dataset '{field_name}' not found in PointData of file {filename}"
                    raise KeyError(msg)

                if dataset.shape != self.global_grid_size and dataset.shape != (
                    *self.global_grid_size,
                    3,
                ):
                    msg = (
                        f"Dataset '{field_name}' has shape {dataset.shape} "
                        f"but expected {self.global_grid_size} or (*{self.global_grid_size}, 3)."
                    )
                    raise ValueError(msg)

                if len(dataset.shape) == 3:
                    field_view[...] = dataset

                elif len(dataset.shape) == 4:
                    field_view[...] = np.transpose(dataset, (3, 0, 1, 2))

        return time


class FishIO:
    def __init__(
        self,
        fish_list: list[Fish],
        virtual_fish_list: list[Fish],
    ) -> None:
        self.num_fish = len(fish_list)
        if self.num_fish <= 0:
            msg = "fish_list must have at least one fish."
            raise ValueError(msg)
        if self.num_fish != len(virtual_fish_list):
            msg = "virtual_fish_list must have the same length as fish_list."
            raise ValueError(msg)

        self.n_elem = int(fish_list[0].n_elems[0])
        for _n_elem in [int(fish.n_elems[0]) for fish in fish_list + virtual_fish_list]:
            if self.n_elem != _n_elem:
                msg = "All fish must have the same number of elements."
                raise ValueError(msg)

        self.fish_dicts = []
        for i in range(self.num_fish):
            self.fish_dicts.append(
                {
                    "name": f"fish_{i}",
                    "semi_major_axis": np.asarray(fish_list[i].semi_major_axis),
                    "semi_minor_axis": np.asarray(fish_list[i].semi_minor_axis),
                    "mass": np.asarray(fish_list[i].mass),
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

    def write_to_file(self, filename: str, time: float | np.float64) -> None:
        with h5py.File(filename, "w") as f:
            root = f.create_group("root")
            root.create_dataset(
                name="Time",
                shape=(1,),
                dtype=np.float64,
                data=np.array([time], dtype=np.float64),
            )

            for fish_dict in self.fish_dicts:
                group = root.create_group(fish_dict["name"])
                for field_name, field_array in fish_dict.items():
                    if field_name == "name":
                        continue
                    group.create_dataset(
                        name=field_name,
                        shape=field_array.shape,
                        dtype=np.float64,
                        data=field_array.copy(),
                    )

    def read_from_file(self, filename: str) -> np.float64:
        time = np.float64(-1.0)
        with h5py.File(filename, "r") as f:
            time = f["/root/Time"][0]
            for fish_dict in self.fish_dicts:
                path = "/root/" + fish_dict["name"]
                try:
                    group = f[path]
                except KeyError:
                    msg = f"Group '{fish_dict['name']}' not found in file {filename} at {path}"
                    raise KeyError(msg)

                for key, value in fish_dict.items():
                    if key != "name":
                        try:
                            value[...] = group[key]
                        except KeyError:
                            msg = f"Dataset '{key}' not found in group in file {filename} at {path}/{key}"
                            raise KeyError(msg)

        return time


class GridIO:
    def __init__(
        self,
        num_fish: int,
        num_lag_points_per_fish: int,
    ) -> None:
        self.num_fish = num_fish

        self.connectivities = ["Vertices", "Lines", "Polygons", "Strips"]
        self.num_points = num_fish * num_lag_points_per_fish
        self.num_cells = num_fish * (num_lag_points_per_fish - 1)

        self.num_line_conn = 2 * self.num_cells
        self.line_connectivity = np.empty((self.num_line_conn,), dtype=np.int32)

        idx_start = 0
        pos_start = 0
        for _ in range(num_fish):
            for j in range(num_lag_points_per_fish - 1):
                self.line_connectivity[idx_start + 2 * j] = pos_start + j
                self.line_connectivity[idx_start + 2 * j + 1] = pos_start + j + 1

            idx_start += 2 * (num_lag_points_per_fish - 1)
            pos_start += num_lag_points_per_fish

        self.line_offsets = np.arange(0, self.num_line_conn + 1, 2, dtype=np.int32)

        self.filter_kwargs = {
            "compression": "gzip",
            "compression_opts": 9,
        }

    def write_to_file(
        self, filename: str, time: float | np.float64, position: np.ndarray, **kwargs
    ) -> None:
        if position.shape != (3, self.num_points):
            msg = f"Position array has shape {position.shape} but expected (3, {self.num_points})."
            raise ValueError(msg)

        with h5py.File(filename, "w") as f:
            root = f.create_group("VTKHDF")
            root.attrs["Version"] = (2, 2)
            ascii_type = "PolyData".encode("ascii")
            root.attrs.create("Type", ascii_type, dtype=h5py.string_dtype("ascii", len(ascii_type)))

            root.create_dataset(
                "NumberOfPoints",
                data=np.array([self.num_points], dtype=np.int32),
            )
            root.create_dataset(
                "Points",
                data=position.T,
                dtype=np.float64,
                **self.filter_kwargs,
            )

            for conn_type in self.connectivities:
                conn_group = root.create_group(conn_type)
                if conn_type == "Lines":
                    conn_group.create_dataset(
                        "NumberOfConnectivityIds",
                        data=np.array([self.num_line_conn], dtype=np.int32),
                    )
                    conn_group.create_dataset(
                        "NumberOfCells",
                        data=np.array([self.num_cells], dtype=np.int32),
                    )
                    conn_group.create_dataset(
                        "Connectivity",
                        data=self.line_connectivity,
                        dtype=np.int32,
                        **self.filter_kwargs,
                    )
                    conn_group.create_dataset(
                        "Offsets",
                        data=self.line_offsets,
                        dtype=np.int32,
                        **self.filter_kwargs,
                    )

                else:
                    conn_group.create_dataset(
                        "NumberOfConnectivityIds",
                        data=np.array([0], dtype=np.int32),
                    )
                    conn_group.create_dataset(
                        "NumberOfCells",
                        data=np.array([0], dtype=np.int32),
                    )
                    conn_group.create_dataset(
                        "Connectivity",
                        data=np.array([], dtype=np.int32),
                    )
                    conn_group.create_dataset(
                        "Offsets",
                        data=np.array([0], dtype=np.int32),
                    )

            # Point data
            point_data = root.create_group("PointData")
            for key, value in kwargs.items():
                if not isinstance(value, np.ndarray):
                    msg = f"Value for {key} is not a numpy array."
                    raise ValueError(msg)
                if value.shape != (3, self.num_points):
                    msg = f"Value for {key} has shape {value.shape} but expected (3, {self.num_points})."
                    raise ValueError(msg)
                point_data.create_dataset(
                    key,
                    data=value.T,
                    dtype=np.float64,
                    **self.filter_kwargs,
                )

            # Field data
            field_data = root.create_group("FieldData")
            field_data.create_dataset(
                "time",
                data=np.array([time], dtype=np.float64),
            )

    def read_from_file(self, filename: str, **kwargs) -> np.float64:
        with h5py.File(filename, "r") as file:
            root = file["VTKHDF"]
            time = root["FieldData"]["time"][0]

            for key, value in kwargs.items():
                if key == "position":
                    dataset = root["Points"]
                    if dataset.shape != (self.num_points, 3):
                        msg = f"Dataset 'Points' has shape {dataset.shape} but expected ({self.num_points}, 3)."
                        raise ValueError(msg)

                    if value.shape != (3, self.num_points):
                        msg = f"Value for 'position' has shape {value.shape} but expected (3, {self.num_points})."
                        raise ValueError(msg)
                    value[...] = dataset.T

                elif key not in root["PointData"].keys():
                    msg = f"Dataset '{key}' not found in PointData of file {filename}."
                    raise KeyError(msg)

                elif value.shape != (3, self.num_points):
                    msg = f"Value for {key} has shape {value.shape} but expected (3, {self.num_points})."
                    raise ValueError(msg)

                else:
                    dataset = root["PointData"][key]
                    if dataset.shape != (self.num_points, 3):
                        msg = f"Dataset '{key}' has shape {dataset.shape} but expected ({self.num_points}, 3)."
                        raise ValueError(msg)
                    value[...] = dataset.T

        return time
