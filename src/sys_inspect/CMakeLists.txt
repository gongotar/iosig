add_library(sys_inspect SHARED
        system_model.cpp
        system_model.hpp
        system_config.cpp
        system_config.hpp
        ../common/iosig_exceptions.hpp
        filesystem.hpp
        blockdevice.hpp
        mountpoint.hpp
        )

target_link_libraries(sys_inspect PUBLIC benchmarks)
target_include_directories(sys_inspect PUBLIC
        ${PROJECT_SOURCE_DIR}/src/benchmarks
        ${PROJECT_SOURCE_DIR}/src/common)


