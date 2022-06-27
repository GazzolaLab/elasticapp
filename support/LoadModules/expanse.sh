#!/bin/env sh

# Load system modules
elasticapp_load_sys_modules() {
    # Make sure the core default modules are loaded.
    module load cmake
    module load doxygen
    module load aocc
}

# Unload system modules
elasticapp_unload_sys_modules() {
    module unload cmake
    module unload doxygen
    module unload aocc
}


elasticapp_load_modules() {
    elasticapp_load_sys_modules

    module load blaze
    module load blaze_tensor
    module load brigand
    #module load catch
    module load yaml-cpp
    module load cxxopts
}

elasticapp_unload_modules() {
    module unload yaml-cpp
    #module unload catch
    module unload brigand
    module unload blaze
    module unload blaze_tensor
    module unload cxxopts

    elasticapp_unload_sys_modules
}
