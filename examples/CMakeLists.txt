
set (PLUGIN_OPTIONS "-fplugin=../src/plugin/libiosigpl.so")

add_executable (sample_c sample_c.cpp)
target_compile_options (sample_c PUBLIC ${PLUGIN_OPTIONS})
set_property(TARGET sample_c PROPERTY CXX_STANDARD 17)
target_link_libraries (sample_c iosigl)

#add_executable (cpp_sample sample.cpp)
#target_compile_options (cpp_sample PRIVATE ${PLUGIN_OPTIONS})
#set_property(TARGET cpp_sample PROPERTY CXX_STANDARD 17)
#target_link_libraries (cpp_sample iosigl)

#add_executable (images_app images_app.cpp)
#set_property(TARGET images_app PROPERTY CXX_STANDARD 17)
#target_link_libraries (images_app iosigl)
#target_compile_options (images_app PRIVATE ${PLUGIN_OPTIONS})



