import numpy
import pybind11
from setuptools import Extension, setup

extension = Extension(
    name="fish_utils._fish_grid_helper",
    sources=["utils/fish_grid.cpp"],
    include_dirs=[pybind11.get_include(), numpy.get_include()],
    language="c++", 
    extra_compile_args=["-O3", "-std=c++17", "-march=native"],
)

setup(ext_modules=[extension])