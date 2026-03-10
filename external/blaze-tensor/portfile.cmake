vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
set(VCPKG_BUILD_TYPE release)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO STEllAR-GROUP/blaze_tensor
    REF master
    SHA512 8a6da1edfcfa92f6a9705104618536d40d7db74edc19b6ba811de405d9ae306398c45f6ec86154d81f2f0d4b3d8eee973c52122527ea9b6ea77a1f559425a731
    HEAD_REF master
    PATCHES
        elastica-fix.patch
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")