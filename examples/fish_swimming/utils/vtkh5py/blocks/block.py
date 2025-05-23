import numpy as np

from vtkh5py.config import VTKHDF_VERSION
from vtkh5py.groups.group import VH5Group


class VH5Block:
    def __init__(
        self,
        block_type: str,
        block_name: str,
    ):
        version = [int(x) for x in str(VTKHDF_VERSION).split(".")]
        self.group = VH5Group(block_name)
        self.group.set_attribute("Version", (version[0], version[1]))
        self.group.set_attribute("Type", np.bytes_(block_type))

    def add_attribute(self, **kwargs):
        for key, item in kwargs.items():
            self.group.set_attribute(key, item)

    def clean_up(self):
        self.group.clean_up()

    def __str__(self):
        return "\n".join(self.group.recursive_get_group_str())

    def __repr__(self):
        return "\n".join(self.group.recursive_get_group_str_all())

    def __getitem__(self, item):
        return self.group[item]
