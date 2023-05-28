// Copyright 2023 Zuse Institute Berlin
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.


 //
// Created by Masoud Gholami on 08.07.21.
//
#ifndef IOSIG_BENCHMARK_SCHEMES_HPP
#define IOSIG_BENCHMARK_SCHEMES_HPP

#include "benchmark_filesizes.hpp"
#include "benchmark_blocksizes.hpp"

#include "io_generators_decls.hpp"
#include "generic_utils.hpp"

namespace iosig::benchmarks::schemes {

    /* filesize benchmarks */

    struct filesize_fstream: public strategies::benchmark_filesizes <io_generators::fstream_io_generator> {
        static constexpr const std::string_view benchmark_name = "filesize_fstream";

        filesize_fstream (const std::string &mountpoint,
                          const std::string &prefix,
                          unsigned long long free_capacity):
                strategies::benchmark_filesizes <io_generators::fstream_io_generator>
                        (benchmark_name, mountpoint, prefix, free_capacity) {}

    };

    struct filesize_mmap: public strategies::benchmark_filesizes <io_generators::mmap_io_generator> {
        static constexpr const std::string_view benchmark_name = "filesize_mmap";

        filesize_mmap (const std::string &mountpoint,
                       const std::string &prefix,
                       unsigned long long free_capacity):
                strategies::benchmark_filesizes <io_generators::mmap_io_generator>
                        (benchmark_name, mountpoint, prefix, free_capacity) {}

    };

    struct filesize_buffered: public strategies::benchmark_filesizes <io_generators::buffered_io_generator> {
        static constexpr const std::string_view benchmark_name = "filesize_buffered";

        filesize_buffered (const std::string &mountpoint,
                       const std::string &prefix,
                       unsigned long long free_capacity):
                strategies::benchmark_filesizes <io_generators::buffered_io_generator>
                        (benchmark_name, mountpoint, prefix, free_capacity) {}

    };


    /* blocksize benchmarks */

    struct blocksize_basic: public strategies::benchmark_blocksizes <io_generators::basic_io_generator> {
        static constexpr const std::string_view benchmark_name = "blocksize_basic";

        blocksize_basic (const std::string &mountpoint,
                              const std::string &prefix,
                              unsigned long long free_capacity):
                strategies::benchmark_blocksizes <io_generators::basic_io_generator>
                        (benchmark_name, mountpoint, prefix, free_capacity) {}

    };

    struct blocksize_basic_sync: public strategies::benchmark_blocksizes <io_generators::basic_sync_io_generator> {
        static constexpr const std::string_view benchmark_name = "blocksize_basic_sync";

        blocksize_basic_sync (const std::string &mountpoint,
                              const std::string &prefix,
                              unsigned long long free_capacity):
                strategies::benchmark_blocksizes <io_generators::basic_sync_io_generator>
                        (benchmark_name, mountpoint, prefix, free_capacity) {}

    };

    struct blocksize_buffered: public strategies::benchmark_blocksizes <io_generators::buffered_io_generator> {
        static constexpr const std::string_view benchmark_name = "blocksize_buffered";

        blocksize_buffered (const std::string &mountpoint,
                         const std::string &prefix,
                         unsigned long long free_capacity):
                strategies::benchmark_blocksizes <io_generators::buffered_io_generator>
                        (benchmark_name, mountpoint, prefix, free_capacity) {}

    };

    struct blocksize_fstream: public strategies::benchmark_blocksizes <io_generators::fstream_io_generator> {
        static constexpr const std::string_view benchmark_name = "blocksize_fstream";

        blocksize_fstream (const std::string &mountpoint,
                          const std::string &prefix,
                          unsigned long long free_capacity):
                strategies::benchmark_blocksizes <io_generators::fstream_io_generator>
                        (benchmark_name, mountpoint, prefix, free_capacity) {}

    };

    struct blocksize_mmap: public strategies::benchmark_blocksizes <io_generators::mmap_io_generator> {
        static constexpr const std::string_view benchmark_name = "blocksize_mmap";

        blocksize_mmap (const std::string &mountpoint,
                       const std::string &prefix,
                       unsigned long long free_capacity):
                strategies::benchmark_blocksizes <io_generators::mmap_io_generator>
                        (benchmark_name, mountpoint, prefix, free_capacity) {}

    };

    template <typename ... T,
            typename = std::enable_if_t <(common::generic::is_base_of_template_v <strategies::benchmark_base, T> && ...)>>
    void perform_schemes (std::ostream &os, T &... scheme) {
        ([&scheme, &os] () {scheme.run_all_benchmarks (); os << scheme.get_result () << std::endl;} (), ...);
    }
}

#endif //IOSIG_BENCHMARK_SCHEMES_HPP
