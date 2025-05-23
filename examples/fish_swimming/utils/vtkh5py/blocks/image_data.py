from typing import Tuple

from vtkh5py.blocks.block import VH5Block
from vtkh5py.groups.fixed_size_data_group import VH5FixedSizeDataGroup
from vtkh5py.groups.group import VH5Group


class VH5ImageData(VH5Block):
    def __init__(
        self,
        spacing: Tuple,
        whole_extent: Tuple,
        block_name: str = "VTKHDF",
        origin: Tuple = (0.0, 0.0, 0.0),
        direction: Tuple = (1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0),
    ):
        super().__init__(block_type="ImageData", block_name=block_name)

        assert len(origin) == 3, f"origin must be of length 3, but got {len(origin)}"
        assert len(spacing) == 3, f"spacing must be of length 3, but got {len(spacing)}"
        assert len(whole_extent) == 6, (
            f"whole_extent must be of length 6, but got {len(whole_extent)}"
        )
        assert len(direction) == 9, (
            f"direction must be of length 9, but got {len(direction)}"
        )

        # Number of grid points in each direction
        self.dims = (
            whole_extent[1] - whole_extent[0] + 1,
            whole_extent[3] - whole_extent[2] + 1,
            whole_extent[5] - whole_extent[4] + 1,
        )
        for dim in self.dims:
            if dim < 0:
                raise ValueError(
                    "whole_extent must consists of 6 integers structured as \n"
                    "x0, xN, y0, yN, z0, zN"
                )
        self.nx, self.ny, self.nz = self.dims

        self.group: VH5Group

        # Set attributes
        self.add_attribute(
            Origin=origin,
            Spacing=spacing,
            WholeExtent=whole_extent,
            Direction=direction,
        )

        # Set datasets (Image data has no top-level datasets)

        # Set subgroups
        self.point_data = self.group.add_subgroup(
            VH5FixedSizeDataGroup(
                group_name="PointData", data_shape=(self.nz, self.ny, self.nx)
            )
        )
        self.cell_data = self.group.add_subgroup(
            VH5FixedSizeDataGroup(
                group_name="CellData",
                data_shape=(self.nz - 1, self.ny - 1, self.nx - 1),
            )
        )
        self.field_data = self.group.add_subgroup(VH5Group(group_name="FieldData"))

    def add_field_data(self, **kwargs):
        for key, item in kwargs.items():
            self.field_data.set_dataset(key, item)

    def add_point_data(self, **kwargs):
        for key, item in kwargs.items():
            self.point_data.set_dataset(key, item.view())

    def add_cell_data(self, **kwargs):
        for key, item in kwargs.items():
            self.cell_data.set_dataset(key, item.view())
