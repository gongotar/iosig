add_library(benchmarks SHARED
        benchmark_schemes.hpp
        benchmark_blocksizes.hpp
        benchmark_filesizes.hpp
        benchmark_base.hpp
        fill_memory.hpp
        fill_memory.cpp
        result_data.hpp
        file_handler_base.hpp
        file_handler_basic.hpp
        file_handler_basic_flush.hpp
        file_handler_buffered.hpp
        file_handler_mmap.hpp
        file_handler_fstream.hpp
        io_generator_base.hpp
        io_generators_decls.hpp
        file_handler_basic_trunc.hpp
        file_handler_basic_trunc_flush.hpp
        ../common/generic_utils.hpp)

target_include_directories(benchmarks PUBLIC
        ${PROJECT_SOURCE_DIR}/src/common)
