#!/usr/bin/env bash

declare -A compilers

# compilers[gcc10]="g++-10"
compilers[gcc11]="g++-11"
# compilers[gcc12]="g++-12"

compilers[clang11]="clang++-11"
# compilers[clang12]="clang++-12"
# compilers[clang13]="clang++-13"

declare -A cmake_build_types
cmake_build_types[debug]="Debug"
# cmake_build_types[release]="Release"
# cmake_build_types[relwithdebuginfo]="RelWithDebInfo"

declare -A smp_options
# smp_options[smp_enabled]="ON"
smp_options[smp_disabled]="OFF"

# CMAKE="${HOME}/Desktop/third_party/vcpkg/downloads/tools/cmake-3.21.1-linux/cmake-3.21.1-linux-x86_64/bin/cmake"
CMAKE="cmake"

for compiler in "${!compilers[@]}"; do
	for cmake_build_type in "${!cmake_build_types[@]}"; do
		for smp_option in "${!smp_options[@]}"; do
			BUILD_PATH="build_${compiler}_${cmake_build_type}_${smp_option}"
			mkdir -p "${BUILD_PATH}" && cd "$_" || exit
			CXX=${compilers[$compiler]}
			CMAKE_BUILD_TYPE=${cmake_build_types[$cmake_build_type]}
			SMP_OPTION=${smp_options[$smp_option]}
			${CMAKE} .. -DCMAKE_CXX_COMPILER="${CXX}" \
				-DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
				-DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
				-GNinja \
				-DBUILD_TESTING=ON \
				-DELASTICA_BUILD_DOCUMENTATION=ON \
				-DELASTICA_BUILD_EXAMPLES=ON \
				-DELASTICA_BUILD_BENCHMARKS=ON \
				-DELASTICA_BUILD_PYTHON_BINDINGS=ON \
				-DELASTICA_ENABLE_PROFILING=ON \
				-DELASTICA_SMP="${SMP_OPTION}" \
				-DUSE_SLEEF=OFF \
				-DDEBUG_SYMBOLS=ON \
				-DENABLE_WARNINGS=ON \
				-DASAN=OFF \
				-DUBSAN_UNDEFINED=OFF \
				-DUBSAN_INTEGER=OFF \
				-DPY_DEV_MODE=OFF
			cd ..
		done
	done
done
