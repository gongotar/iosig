add_library(plotter SHARED
        plot_benchmarks.cpp
        plot_benchmarks.hpp
        )

target_link_libraries(plotter PUBLIC benchmarks qcustomplot)

target_include_directories(plotter PUBLIC
        ${PROJECT_SOURCE_DIR}/src/benchmarks
        ${PROJECT_SOURCE_DIR}/src/common
        ${PROJECT_SOURCE_DIR}/dependencies/qcustomplot)


