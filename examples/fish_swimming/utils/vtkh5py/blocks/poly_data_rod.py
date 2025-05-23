import numpy as np

from vtkh5py.blocks.block import VH5Block
from vtkh5py.groups.fixed_size_data_group import VH5FixedSizeDataGroup
from vtkh5py.groups.group import VH5Group


# TODO: periodic mesh functionality
class VH5PolyDataRod(VH5Block):
    def __init__(
        self,
        number_of_points: int,
        block_name: str = "VTKHDF",
    ):
        """Wrapper for PolyData with only line segments as cells"""
        super().__init__(block_type="PolyData", block_name=block_name)

        assert number_of_points >= 2, "Must have at least 2 points"

        self.group: VH5Group

        self.connectivities = ["Vertices", "Lines", "Polygons", "Strips"]

        # Assume line cells only arranged in 1d
        self.num_points = number_of_points
        self.num_cells = number_of_points - 1

        # Set attributes (No additional attributes)

        # Set datasets
        self.group.set_dataset(
            "NumberOfPoints", np.array([self.num_points], dtype=np.int64)
        )

        # Set subgroups
        self.point_data = self.group.add_subgroup(
            VH5FixedSizeDataGroup(group_name="PointData", data_shape=(self.num_points,))
        )
        self.cell_data = self.group.add_subgroup(
            VH5FixedSizeDataGroup(group_name="CellData", data_shape=(self.num_cells,))
        )
        self.field_data = self.group.add_subgroup(VH5Group(group_name="FieldData"))

        for connectivity in self.connectivities:
            self.group.add_subgroup(VH5Group(group_name=connectivity, cleanable=False))

        self._setup_connectivities()

    def set_points(self, points: np.ndarray) -> None:
        assert points.shape == (self.num_points, 3,), (
            f"Point array shape incompatible. Expected: "
            f"{(self.num_points, 3)}, but got {points.shape}"
        )

        self.group.set_dataset("Points", points.view())

    def add_field_data(self, **kwargs) -> None:
        for key, item in kwargs.items():
            self.field_data.set_dataset(key, item)

    def add_point_data(self, **kwargs) -> None:
        for key, item in kwargs.items():
            self.point_data.set_dataset(key, item.view())

    def add_cell_data(self, **kwargs) -> None:
        for key, item in kwargs.items():
            self.cell_data.set_dataset(key, item.view())

    def _setup_connectivities(self) -> None:
        int_null = np.array([], dtype=np.int64)
        int_zero = np.zeros((1,), dtype=np.int64)

        for conn_str in self.connectivities:
            conn = self.group[conn_str]
            if conn_str == "Lines":
                n_conn = 2 * self.num_cells

                conn.set_dataset(
                    "NumberOfCells", np.array([self.num_cells], dtype=np.int64)
                )
                conn.set_dataset(
                    "NumberOfConnectivityIds", np.array([n_conn], dtype=np.int64)
                )

                # connectivity = [0, 1, 1, 2, 2, ..., n-1, n-1, n]
                connectivity = np.empty((n_conn,), dtype=np.int64)
                connectivity[::2] = np.arange(0, self.num_cells)
                connectivity[1::2] = np.arange(1, self.num_cells + 1)
                conn.set_dataset("Connectivity", connectivity)

                # offsets = [0, 2, 4, ..., 2n-2, 2n]
                offsets = np.arange(0, n_conn + 1, 2, dtype=np.int64)
                conn.set_dataset("Offsets", offsets)

            else:
                conn.set_dataset("NumberOfCells", int_zero.copy())
                conn.set_dataset("NumberOfConnectivityIds", int_zero.copy())
                conn.set_dataset("Connectivity", int_null.copy())
                conn.set_dataset("Offsets", int_zero.copy())
