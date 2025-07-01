import h5py
import numpy as np

from vtkh5py.blocks.block import VH5Block
from vtkh5py.groups.group import VH5Group


class VH5BlockWriter:
    def __init__(self, block: VH5Block):
        self.root_block = block
        self.root = block.group

        assert self.root.name == "VTKHDF", "Top level group must have name VTKHDF"

    def write_to_file(self, file_name: str):
        self.root_block.clean_up()
        with h5py.File(file_name, "w") as f:
            VH5BlockWriter._write_group(f, self.root)

    @staticmethod
    def _write_group(src_group: h5py.Group, vh_group: VH5Group):
        sub_group = src_group.create_group(np.bytes_(vh_group.name), track_order=True)

        for sub_vh_group in vh_group.subgroups:
            VH5BlockWriter._write_group(sub_group, sub_vh_group)

        for tag, dataset in vh_group.datasets.items():
            data_to_save = dataset
            if not isinstance(dataset, np.ndarray):
                data_to_save = np.asarray(dataset)
            shape = dataset.shape
            if len(shape) > 1:
                shape = (x for x in data_to_save.shape if x != 1)
            sub_group.create_dataset(
                name=tag,
                shape=shape,
                dtype=data_to_save.dtype,
                data=np.squeeze(data_to_save),
            )

        for attr, value in vh_group.attributes.items():
            if isinstance(value, VH5Group):
                sub_group[attr] = h5py.SoftLink("/VTKHDF" + value.get_group_str())

            else:
                sub_group.attrs[attr] = value
