from typing import Protocol
from typing import Dict, Any, Union

import os
import subprocess
import sys
import logging

import numpy as np

import time

import yaml


class HookProtocol(Protocol):
    def __call__(self) -> float:
        ...


class ElasticappHook:
    def __init__(self, config: Dict, execute_name: str, input_path: str):
        self.config = config
        self.execute_name = execute_name
        self._input_path = input_path
        self._metadata_path = "elastica_metadata.yml"

    def __call__(self):
        input_path = self.set_result()
        p = subprocess.Popen(
            [
                self.execute_name,
                "--input-file",
                input_path,
            ],
            #capture_output=True,
            #text=True,
            env={"ELASTICA_NO_SPLASH": "1"},
        )
        # Set affinity
        if "num_threads" in self.config:
            pid = p.pid
            #os.sched_setaffinity(pid, set(np.arange(self.config["num_threads"]) ))
        p_status = p.wait()
        output = self.get_result()
        return output

    def set_result(self):
        path = "_mod_" + os.path.basename(self._input_path)
        with open(self._input_path, "r") as yamlfile:
            load_data = yaml.full_load(yamlfile)
            casename = list(load_data.keys())[0]
            # TODO: Key Value pair should be created outside and passed
            if "block_policy" in self.config:
                policy = self.config["block_policy"]
                if "block_size" in self.config:
                    block_size = self.config["block_size"]
                load_data[casename]["Parameters"]["BlockingPolicy"] = { policy:{"CriterionParameter": {"Size":block_size}}}

            if "num_threads" in self.config:
                load_data[casename]["Parameters"]["n_threads"] = self.config["num_threads"]

            if "num_rods" in self.config:
                load_data[casename]["Parameters"]["n_rods"] = self.config["num_rods"]
            elif "z_num_rods" in self.config:
                load_data[casename]["Parameters"]["n_rods"] = self.config["z_num_rods"]
            elif "z_num_rods_per_thread" in self.config:
                load_data[casename]["Parameters"]["n_rods"] = self.config["z_num_rods_per_thread"] * self.config["num_threads"]

            if "num_elements" in self.config:
                load_data[casename]["Parameters"]["n_elements"] = self.config["num_elements"]

        with open(path, "w") as yamlfile:
            yaml.dump(load_data, yamlfile)
        return path

    def get_result(self) -> Union[float, Dict]:
        """get_result."""
        if not os.path.exists(self._metadata_path):
            return -1
        with open(self._metadata_path, "r") as yamlfile:
            metadata = yaml.full_load(yamlfile)
            last_saved_time = max(list(metadata.keys()))
            datapath = metadata[last_saved_time]["Filename"]
        with open(datapath, "r") as yamlfile:
            data = yaml.full_load(yamlfile)
            results = data["profiling"]["sections"]
            results["n_blocks"] = int(data["n_blocks"])
        return results


class PyBindHook:
    def __init__(self, config: Dict, pybind_module, elasticapp_kwargs=None):
        kwargs = elasticapp_kwargs or {}
        kwargs.update(config)
        self.module = pybind_module(**kwargs)

    def execute(self) -> float:
        return self.module.run()
