add_library(iosigl SHARED 
    iosig_lib.cpp
    iosig_lib.hpp
    ${PROJECT_SOURCE_DIR}/src/common/iosig_common_parameter.hpp
)

list(APPEND PRIVATE_HEADERS .)

set_property(TARGET iosigl PROPERTY CXX_STANDARD 17)

target_include_directories(iosigl PUBLIC ${PUBLIC_HEADERS} PRIVATE ${PRIVATE_HEADERS})

install(TARGETS iosigl DESTINATION lib)
