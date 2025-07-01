from typing import Tuple

import numpy as np

from vtkh5py.groups.group import VH5Group


class VH5FixedSizeDataGroup(VH5Group):
    def __init__(
        self,
        group_name: str,
        data_shape: Tuple,
        cleanable: bool = True,
    ):
        super().__init__(group_name=group_name, cleanable=cleanable)

        if not data_shape:
            raise ValueError(
                "For data group without shape/size restriction, use VH5Group instead"
            )

        self.data_shape = data_shape
        self.fixed_dim = len(data_shape)

    def set_dataset(self, data_name: str, data_value: np.ndarray):
        assert isinstance(
            data_value, np.ndarray
        ), f"Dataset type ({type(data_value)}) must be numpy array"
        assert data_value.shape[: self.fixed_dim] == self.data_shape, (
            f"{data_name} does not conform to fixed shape\n",
            f"Expected outer dimension: {self.data_shape}, but got {data_value.shape}",
        )
        self.datasets[data_name] = data_value.view()
