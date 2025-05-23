from typing import Any
from typing_extensions import Self

import numpy as np


class VH5Group:
    def __init__(
        self,
        group_name: str,
        cleanable: bool = True,
    ):
        """
        :param group_name:
        :param cleanable:
        """
        self.name: str = group_name
        self.cleanable: bool = cleanable
        self.attributes: dict = {}
        self.datasets: dict = {}
        self.subgroups: list[Self] = []

    def set_attribute(self, attr_name: str, attr_value: Any) -> None:
        self.attributes[attr_name] = attr_value

    def set_dataset(self, data_name: str, data_value: np.ndarray) -> None:
        assert isinstance(
            data_value, np.ndarray
        ), f"dataset type ({type(data_value)}) must be numpy array"
        self.datasets[data_name] = data_value.view()

    def add_subgroup(self, subgroup: Self) -> Self:
        self.subgroups.append(subgroup)
        return subgroup

    def get_group_str(self) -> str:
        return "/" + self.name

    def get_attributes_str(self) -> list[str]:
        prefix = self.get_group_str() + "/attributes/"
        attr_str = []
        for key, item in self.attributes.items():
            if isinstance(item, VH5Group):
                item_str = f"[SoftLink to {item.name}]"
            else:
                item_str = str(item)
            attr_str.append(prefix + key + ": " + item_str)

        return attr_str

    def get_datasets_str(self) -> list[str]:
        prefix = self.get_group_str() + "/datasets/"
        data_str = []
        for key, item in self.datasets.items():
            description = prefix + key + ": "
            if isinstance(item, np.ndarray):
                description += f"({' '.join(map(str, item.shape))})"
            else:
                description += f"{item}"

            data_str.append(description)

        return data_str

    def recursive_get_group_str(self) -> list[str]:
        groups_str = []
        src_group_str = self.get_group_str()

        for subgroup in self.subgroups:
            sub_group_str = subgroup.recursive_get_group_str()
            groups_str += [src_group_str + sub_str for sub_str in sub_group_str]

        if not groups_str:
            groups_str.append(src_group_str)

        return groups_str

    def recursive_get_group_str_all(self) -> list[str]:
        src_group_str = self.get_group_str()
        str_all = [src_group_str] + self.get_attributes_str() + self.get_datasets_str()
        for subgroup in self.subgroups:
            sub_group_str = subgroup.recursive_get_group_str_all()
            str_all += [src_group_str + sub_str for sub_str in sub_group_str]

        return str_all

    def clean_up(self):
        if not self.cleanable:
            return

        # Clean up attributes
        to_delete = []
        for key, item in self.attributes.items():
            empty = (
                (item is None)
                or (isinstance(item, list) and not item)
                or (isinstance(item, tuple) and not item)
            )
            if empty:
                to_delete.append(key)

        for key in to_delete:
            del self.attributes[key]

        # Clean up datasets
        to_delete.clear()
        for key, item in self.datasets.items():
            empty = (
                (item is None)
                or (isinstance(item, list) and not item)
                or (isinstance(item, tuple) and not item)
            )
            if empty:
                to_delete.append(key)

        for key in to_delete:
            del self.datasets[key]

        # Recursively clean up subgroups
        to_delete.clear()
        for idx, subgroup in enumerate(self.subgroups):
            subgroup.clean_up()
            if not subgroup:
                to_delete.append(idx)

        for idx in reversed(to_delete):
            self.subgroups.pop(idx)

    def __str__(self):
        out_str = "Group name: " + self.name + "\n"

        out_str += "Attributes:\n"
        for key, item in self.attributes.items():
            out_str += f"  {key}: {item}\n"

        out_str += "Datasets:\n"
        for key, item in self.datasets.items():
            if isinstance(item, np.ndarray):
                out_str += f"  {key}: {item.shape}\n"
            else:
                out_str += f"  {key}: {item}\n"

        out_str += "Sub-groups:\n"
        for subgroup in self.subgroups:
            out_str += f"  {subgroup.name}\n"

        return out_str

    def __bool__(self):
        return bool(self.attributes) or bool(self.datasets) or bool(self.subgroups)

    def __getitem__(self, item: str):
        for subgroup in self.subgroups:
            if subgroup.name == item:
                return subgroup

        raise ValueError(f"Unrecognized subscript item: {item}")
