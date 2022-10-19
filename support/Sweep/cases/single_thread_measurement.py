"""
The case to measure T1: serialized timing.

The idea is to investigate how number of block can influence "single-threade" performance.
Ideally speaking, block_size should not be a variable to optimize single-threaded performance, but 
we want to make sure if that is the case.

The intended plot is:
    x-axis: number of rods (large case)
    y-axis: wall time
    for each: [policy x block_size]

Note:
- Before compile, make sure TBB is off.
- Parallelize code in parallel_butterfly/main.cpp might needs to be removed.
"""

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
    min_n_rods_log2 = 9
    max_n_rods_log2 = 11
    max_blocksize_log2 = 15

    # Search space API:
    # https://docs.ray.io/en/latest/tune/api_docs/search_space.html#
    search_space = dict(
            num_threads=1,
            block_policy=tune.grid_search([
                "LooselyRestrictSizeAcrossBlockTypes",
                "RestrictSizeAcrossBlockTypes",
                "AlwaysSameAcrossBlockTypes",
                "AlwaysNewAcrossBlockTypes",
            ]),
            block_size=tune.grid_search(
                (2**np.arange(max_blocksize_log2+1)).tolist()
            ),
            num_rods=tune.grid_search(
                (2 ** np.arange(min_n_rods_log2, max_n_rods_log2+1)).tolist(),
            ),
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
    experiment_name = f"single_thread_{timestamp}"
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
