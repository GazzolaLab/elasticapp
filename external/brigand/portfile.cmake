vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
set(VCPKG_BUILD_TYPE release)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO edouarda/brigand
    REF 9a86130c87479a38659edd7c0dda776a27f7077d
    SHA512 4ce313556df769a5c528e9e136e61ac331e6780c4b9992dbd26767463090d29b5fea81d62d872366dbbed06fabff54fe2b22c63cb95c4339595b951bf7c24778
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DBUILD_TESTING=OFF)
vcpkg_cmake_install()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")