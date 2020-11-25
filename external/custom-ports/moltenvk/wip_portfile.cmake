vcpkg_fail_port_install(ON_TARGET "Windows" "Linux")

vcpkg_download_distfile(ARCHIVE
    URLS https://github.com/KhronosGroup/MoltenVK/archive/ce85a96d8041b208e6f32898912b217957019b5a.zip
    FILENAME MoltenVK.zip
    SHA512 2a6ffcf504985b47529200578f3b8b09abeaf32d56097abfe2e35981ed503e66ce2b661c663d2135d2ea5650efb889691b10d57b35e89ff19f30f8eb650aecc7
)

vcpkg_extract_source_archive_ex(
    OUT_SOURCE_PATH SOURCE_PATH
    ARCHIVE ${ARCHIVE}
)

execute_process(
    COMMAND ./fetchDependencies --macos
    WORKING_DIRECTORY ${SOURCE_PATH}
)

vcpkg_install_make(BUILD_TARGET "macos")