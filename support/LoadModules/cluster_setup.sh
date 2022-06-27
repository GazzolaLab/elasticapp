#!/bin/env bash

# Distributed under the MIT License.
# See LICENSE.txt for details.

#set -e

read -rd '' globalhelp <<-EOF
    how to use cluster_setup
    ------------------------
    ./cluster_setup.sh <options>
    
    options and explanations
    ---------------------------
      help : Print this help message
    
      path : Required. Path to install dependencies/modules (created if it does not exist).
EOF
if [[ $1 =~ ^([hH][eE][lL][pP]|[hH])$ ]]; then
    echo "${globalhelp}"
    exit 1
fi

if [ $# != 1 ]; then
    echo "You must pass one argument to cluster_setup modules, which"
    echo "is the directory where you want the dependencies to be built."
    exit 1
fi

start_dir=`pwd`
dep_dir=`realpath $1`
mkdir -p $dep_dir
cd $dep_dir

# log all output from this script
exec > >(tee "elasticapp.install.log.$(date +%F-%T)") 2>&1

mkdir -p $dep_dir/modules

elasticapp_load_sys_modules

#if [ -f catch/include/catch.hpp ]; then
#    echo "Catch is already installed"
#else
#    echo "Installing catch..."
#    mkdir -p $dep_dir/catch/include
#    cd $dep_dir/catch/include
#    wget \
#        https://github.com/catchorg/Catch2/releases/download/v2.13.0/catch.hpp \
#        -O catch.hpp
#    echo "Installed Catch into $dep_dir/catch"
#    cat >$dep_dir/modules/catch <<EOF
##%Module1.0
#prepend-path CPATH "$dep_dir/catch/include"
#prepend-path CMAKE_PREFIX_PATH "$dep_dir/catch/"
#EOF
#fi
#cd $dep_dir

if [ -f blaze/include/blaze/Blaze.h ]; then
    echo "Blaze is already installed"
else
    echo "Installing Blaze..."
    mkdir -p $dep_dir/blaze/
    cd $dep_dir/blaze/
    wget https://bitbucket.org/blaze-lib/blaze/downloads/blaze-3.8.1.tar.gz \
         -O blaze.tar.gz
    echo "Unpacking Blaze. This can take a few minutes..."
    tar -xzf blaze.tar.gz
    cd blaze-3.8.1
    mkdir build
    cd build
    cmake -D CMAKE_BUILD_TYPE=Release \
          -D BLAZE_SHARED_MEMORY_PARALLELIZATION=NO \
          -D CMAKE_INSTALL_PREFIX=$dep_dir/blaze/ ..
    make install
    echo "Installed Blaze into $dep_dir/blaze"
    cat >$dep_dir/modules/blaze <<EOF
#%Module1.0
prepend-path CPATH "$dep_dir/blaze/include"
prepend-path CMAKE_PREFIX_PATH "$dep_dir/blaze/"
EOF
fi
cd $dep_dir

# Blaze_tensor must be installed after blaze
if [ -f blaze_tensor/include/blaze_tensor/Blaze.h ]; then
    echo "Blaze-Tensor is already installed"
else
    echo "Installing Blaze-Tensor..."
    mkdir -p $dep_dir/blaze_tensor/
    cd $dep_dir/blaze_tensor/
    wget https://github.com/STEllAR-GROUP/blaze_tensor/archive/refs/heads/master.zip \
         -O blaze_tensor.zip
    echo "Unpacking Blaze-Tensor. This can take a few minutes..."
    unzip -qo blaze_tensor.zip
    cd blaze_tensor-master
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=$dep_dir/blaze_tensor/ \
          -Dblaze_DIR=$dep_dir/blaze/share/blaze/cmake/ ..
    make -j4
    make install
    echo "Installed Blaze-Tensor into $dep_dir/blaze_tensor"
    cat >$dep_dir/modules/blaze_tensor <<EOF
#%Module1.0
prepend-path CPATH "$dep_dir/blaze_tensor/include"
prepend-path CMAKE_PREFIX_PATH "$dep_dir/blaze_tensor/"
EOF
fi
cd $dep_dir

if [ -f brigand/include/brigand/brigand.hpp ]; then
    echo "Brigand is already installed"
else
    echo "Installing Brigand..."
    rm -rf $dep_dir/brigand
    git clone https://github.com/edouarda/brigand.git
    echo "Installed Brigand into $dep_dir/brigand"
    cat >$dep_dir/modules/brigand <<EOF
#%Module1.0
prepend-path CPATH "$dep_dir/brigand/include"
prepend-path CMAKE_PREFIX_PATH "$dep_dir/brigand/"
EOF
fi
cd $dep_dir

if [ -f $dep_dir/cxxopts/include/cxxopts.hpp ]; then
    echo "cxxopts is already installed"
else
    echo "Installing cxxopts..."
    wget https://github.com/jarro2783/cxxopts/archive/refs/tags/v3.0.0.tar.gz \
         -O cxxopts.tar.gz
    tar -xzf cxxopts.tar.gz
    mv cxxopts-* cxxopts-build
    cd $dep_dir/cxxopts-build
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=$dep_dir/cxxopts \
          -S ..
    make -j4
    make install
    cd $dep_dir
    rm -r cxxopts-build
    rm -r cxxopts.tar.gz
    echo "Installed cxxopts into $dep_dir/cxxopts"
    cat >$dep_dir/modules/cxxopts <<EOF
#%Module1.0
prepend-path CPATH "$dep_dir/cxxopts/include"
prepend-path CMAKE_PREFIX_PATH "$dep_dir/cxxopts/"
EOF
fi
cd $dep_dir

if [ -f $dep_dir/yaml-cpp/lib64/libyaml-cpp.a ]; then
    echo "yaml-cpp is already installed"
else
    echo "Installing yaml-cpp..."
    wget https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-0.7.0.tar.gz \
         -O yaml-cpp.tar.gz
    tar -xzf yaml-cpp.tar.gz
    mv yaml-cpp-* yaml-cpp-build
    cd $dep_dir/yaml-cpp-build
    mkdir build
    cd build
    cmake -D CMAKE_BUILD_TYPE=Release -D YAML_CPP_BUILD_TESTS=OFF \
          -D CMAKE_C_COMPILER=gcc -D CMAKE_CXX_COMPILER=g++ \
          -D YAML_CPP_BUILD_CONTRIB=OFF \
          -D YAML_CPP_BUILD_TOOLS=ON \
          -D CMAKE_INSTALL_PREFIX=$dep_dir/yaml-cpp ..
    make -j4
    make install
    cd $dep_dir
    rm -r yaml-cpp-build
    rm -r yaml-cpp.tar.gz
    echo "Installed yaml-cpp into $dep_dir/yaml-cpp"
    cat >$dep_dir/modules/yaml-cpp <<EOF
#%Module1.0
prepend-path LIBRARY_PATH "$dep_dir/yaml-cpp/lib"
prepend-path LD_LIBRARY_PATH "$dep_dir/yaml-cpp/lib"
prepend-path CPATH "$dep_dir/yaml-cpp/include"
prepend-path CMAKE_PREFIX_PATH "$dep_dir/yaml-cpp/"
EOF
fi
cd $dep_dir

elasticapp_unload_sys_modules

module use modules
cd $start_dir
echo "Dependency setup completed."
