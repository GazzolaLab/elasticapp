from vtkh5py.blocks.block import VH5Block
from vtkh5py.groups.group import VH5Group


class VH5PartitionedDataset(VH5Block):
    def __init__(self):
        super().__init__(
            block_type="PartitionedDataSetCollection",
            block_name="VTKHDF",  # PDC is always top level
        )

        self.num_blocks = 0

        self.group: VH5Group

        self.assembly = self.group.add_subgroup(VH5Group("Assembly"))

    def add_assembly_subgroup(self, name: str, parent: str = "/Assembly"):
        group = self._get_group_from_path(parent)
        group.add_subgroup(VH5Group(name))

    def add_block(self, block: VH5Block, parent: str = "/Assembly") -> None:
        assert not isinstance(block, type(self)), (
            "Nested " + type(self).__name__ + " is not allowed"
        )

        # Get parent group in the Assembly hierarchy
        parent_group = self._get_group_from_path(parent)

        # Add data block directly under root
        sub_group = self.group.add_subgroup(block.group)

        # PDC requires indexing on the blocks
        sub_group.attributes["Index"] = self.num_blocks
        self.num_blocks += 1

        # Add (sub_group.name, sub_group) as a key-value pair
        # as an attribute to parent_group
        parent_group.set_attribute(sub_group.name, sub_group)

    def _get_group_from_path(self, path: str) -> VH5Group:
        path_list = list(filter(None, path.split("/")))  # remove empty strings
        group = self.group
        for group_name in path_list:
            try:
                group = group[group_name]
            except ValueError:
                raise ValueError("Input path does not exist: " + path)

        return group

    def print_assembly_hierarchy(self):
        assembly_subgroups = self.assembly.recursive_get_group_str()
        for subgroup in assembly_subgroups:
            print("/VTKHDF" + subgroup)
