#!/usr/bin/env python

# Distributed under the MIT License.
# See LICENSE.txt for details.

from distutils.core import setup

setup(
    name="benchtools",
    version="0.0.1",
    description="Internal benchmarking tools for Elastica++",
    author="Tejaswin Parthasarathy, Seung Hyun Kim, GazzolaLab",
    url="https://github.com/tp5uiuc/elasticapp",
    license="MIT",
    packages=["benchtools"],
    install_requires=["numpy", "matplotlib", "tabulate==0.8.7", "tqdm"],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    entry_points={
        "console_scripts": [
            "benchtools=benchtools:run",
        ],
    },
)
