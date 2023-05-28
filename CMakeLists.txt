cmake_minimum_required(VERSION 3.10)

project(iosig VERSION 1.0)

add_definitions('-Wall')

include(GNUInstallDirs)

add_subdirectory (src)

enable_testing ()
add_subdirectory (tests)

#add_subdirectory (examples)
