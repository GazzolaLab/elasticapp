vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
set(VCPKG_BUILD_TYPE release)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO edouarda/brigand
    REF master
    SHA512 8b66183b91d5ad2282cec4850f1f33288679932e46d4ba0a7c8d4dd851d3377cd089932191d9416ed8c4b6c0ffb2070de4a4b6e2887ebb8b940a1e8f686c9e14
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DBUILD_TESTING=OFF)
vcpkg_cmake_install()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")