add_executable (pragma_multiple_file
    cstyle_main.cpp
    cstyle_file1.cpp
    cstyle_file1.hpp
    cstyle_file2.cpp
    cstyle_file2.hpp
)

set (PLUGIN_OPTIONS "-fplugin=../src/plugin/libiosigpl.so")
target_compile_options (pragma_multiple_file PUBLIC ${PLUGIN_OPTIONS})

target_link_libraries (pragma_multiple_file iosigl)
add_test (
    NAME pragma_multiple_file_test
    COMMAND pragma_multiple_file
)
