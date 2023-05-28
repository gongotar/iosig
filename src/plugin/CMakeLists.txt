add_library(iosigpl SHARED 
    iosig_location.hpp
    iosig_parameter.hpp
    iosig_parameter.cpp
    iosig_pragma.cpp
    iosig_pragma.hpp
    iosig_registered_io.cpp
    iosig_registered_io.hpp
    iosig_passes.hpp
    iosig_passes.cpp
    iosig_plugin.cpp
    iosig_plugin.hpp
    ${PROJECT_SOURCE_DIR}/src/common/iosig_common_parameter.hpp
)

execute_process(COMMAND bash -c "${CMAKE_C_COMPILER} -print-file-name=plugin | tr -d '\n'" 
    OUTPUT_VARIABLE GCCPLUGINS_DIR)
message (STATUS "Using the plugin headers located in ${GCCPLUGINS_DIR}")

list(APPEND PRIVATE_HEADERS ${PROJECT_SOURCE_DIR}/include/gcc_headers)
list(APPEND PRIVATE_HEADERS .)


set_property(TARGET iosigpl PROPERTY CXX_STANDARD 14)

target_compile_options(iosigpl PRIVATE -fno-rtti)

target_include_directories(iosigpl PUBLIC ${PUBLIC_HEADERS} PRIVATE ${PRIVATE_HEADERS})
target_include_directories(iosigpl SYSTEM PRIVATE ${GCCPLUGINS_DIR}/include)

