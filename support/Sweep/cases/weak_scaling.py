import os
import sys
import multiprocessing

import click

import numpy as np
from ray import tune

from datetime import datetime

from ElasticappSweep.grid_run import execute


def get_search_space():
    # Configuration
    min_blocksize_log2 = 7
    max_blocksize_log2 = 12
    max_n_rods_per_thread_log2 = 14
    max_num_threads = multiprocessing.cpu_count()

    # Search space API:
    # https://docs.ray.io/en/latest/tune/api_docs/search_space.html#
    search_space = dict(
            block_policy="LooselyRestrictSizeAcrossBlockTypes",
            num_elements=2**4,
            block_size=tune.grid_search(
                (2 ** np.arange(min_blocksize_log2, max_blocksize_log2 + 1)).tolist()
            ),
            num_threads=tune.grid_search(
                np.arange(1, max_num_threads + 1).tolist()
            ),
            z_num_rods_per_thread=2**max_n_rods_per_thread_log2,
        )
    return search_space


@click.command()
@click.option(
    "-e",
    "--executable",
    type=str,
    required=True,
    help="Path to the executable file from Elastica++",
)
@click.option(
    "-i",
    "--input_yaml",
    type=str,
    required=True,
    help="Path to the input yaml file that contains configuration for executable file",
)
@click.option(
    "-s",
    "--save_path",
    type=str,
    help="Path to save ray-result. (Default='./ray_default')",
)
def cli(executable, input_yaml, save_path):
    now = datetime.now()
    timestamp = now.strftime("%m_%d_%Y_%H_%M_%S")
    experiment_name = f"weak_scaling_{timestamp}"
    search_space = get_search_space()
    execute(
        search_space=search_space,
        input_path=input_yaml,
        execute_name=executable,
        save_path=save_path,
        name=experiment_name
    )


if __name__ == "__main__":
    cli()
