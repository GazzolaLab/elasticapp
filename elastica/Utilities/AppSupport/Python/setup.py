#!/usr/bin/env python

# Distributed under the MIT License.
# See LICENSE.txt for details.

from distutils.core import setup

setup(
    name='elasticapp',
    version='@elastica_VERSION@',
    description="Python bindings for Elastica++",
    author="Empty",
    url="cosseratrods.org",
    license="MIT",
    packages=['elasticapp'],
    install_requires=['numpy', 'scipy'],
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
)
