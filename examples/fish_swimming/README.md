## Fish swimming case study
This case study implements a custom-build elastic fish in `elasticapp` swimming in a viscous, unbounded fluid. The flow environment and fluid-structure interaction is simulated by the external solver [SophT](https://github.com/SophT-Team/SophT.git) via the immersed boundary method. Relevant publication can be found at [Tekinalp et al., 2025](https://doi.org/10.1016/j.cma.2025.117910). In particular, this study mirrors but upscales one of the demonstrations in that work. 


### Shared memory simulation
The Python script `fish_swimming.py` showcases a single fish swimming in a smaller domain. To run this simulation, ensure you have the necessary dependencies installed:
```sh
export build_path="/path/to/elasticapp/build/dir"

# Compile the C++ code with pybindings
cmake --build ${build_path} --target PyFishCaseStudy -j

# Install elasticapp python bindings (e.g. into venv)
pip install /path/to/elasticapp/build/bin/python

# Install dependencies local to this case
pip install examples/fish_swimming
```

### Distributed memory (MPI) simulation
This case study also supports distributed memory simulation enabled by MPI. However, a build script is not provided as the dependency management is too complex and OS-dependent. To enable MPI, the following steps should be followed

1. Install `hdf5` with parallelization enabled. Note that the parallel build is mutually exclusive with the non-parallel version. 
2. Install `h5py` that uses the parallel `hdf5`. You can verify the installation by checking the `h5py.get_config().mpi` flag.
3. Install [`sopht-mpi`](https://github.com/fankiat/sopht-mpi) along with its dependencies. This package requires Python 3.10. 
4. Run the MPI fish swimming script `python fish_swimming_mpi.py --help` 
