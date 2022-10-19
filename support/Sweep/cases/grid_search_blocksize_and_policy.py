import os
import sys
import multiprocessing

import click

import numpy as np
from ray import tune

from ElasticappSweep.grid_run import execute


def get_search_spaces():
    # Configuration
    max_blocksize_log2 = 12  # TODO: Not sure what size we wish to test
    max_n_rods_log2 = 10  # TODO: Not sure what size we wish to test

    # Search space API:
    # https://docs.ray.io/en/latest/tune/api_docs/search_space.html#
    search_spaces = [
        dict(
            block_size=tune.grid_search(
                (2 ** np.arange(max_blocksize_log2 + 1)).tolist()
            ),
            block_policy=tune.grid_search(
                [
                    "RestrictSizeAcrossBlockTypes",
                    "LooselyRestrictSizeAcrossBlockTypes",
                    "AlwaysNewAcrossBlockTypesPolicy",
                    "AlwaysSameAcrossBlockTypesPolicy",
                ]
            ),
            num_rods=tune.grid_search(
                (3 * (2 ** np.arange(max_n_rods_log2 + 1))).tolist()
            ),
            num_threads=multiprocessing.cpu_count(),
        ),
    ]
    return search_spaces


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
    search_spaces = get_search_spaces()
    for search_space in search_spaces:
        execute(
            search_space=search_space,
            input_path=input_yaml,
            execute_name=executable,
            save_path=save_path,
        )


if __name__ == "__main__":
    cli()
