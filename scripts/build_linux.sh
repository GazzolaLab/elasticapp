#!/bin/bash

mkdir -p build && cd "$_" || return

cmake \
	"$@" ..

make -j"$(nproc)"
