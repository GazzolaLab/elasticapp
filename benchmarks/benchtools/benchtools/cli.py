#!/usr/bin/env python3

import click

from .bench_utils import quick_benchmark


@click.command()
@click.option("--name", help="Benchmark name", type=str)
@click.argument("values", type=int, nargs=-1)
@click.option("--save", is_flag=True, help="Save", default=False)
def run(name, values, save):
    quick_benchmark(name, values, save)


if __name__ == "__main__":
    run()
