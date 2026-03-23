vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
set(VCPKG_BUILD_TYPE release)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO tp5uiuc/spline
    REF master
    SHA512 0f4335bae97998f7fdf042fb0f76ef58767a17487e4d26a1f9630aff2323e3f7014cd55ca2b642ddecd64ab81c9f8fdd9fce2e883eda3501b69ba930eaf89ac2
    HEAD_REF master
)
vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")