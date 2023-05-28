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
// Created by Masoud Gholami on 11.07.21.
//

#ifndef IOSIG_IO_GENERATORS_DECLS_HPP
#define IOSIG_IO_GENERATORS_DECLS_HPP

#include "file_handler_basic_flush.hpp"
#include "file_handler_buffered.hpp"
#include "file_handler_mmap.hpp"
#include "io_generator_base.hpp"
#include "file_handler_fstream.hpp"
#include "file_handler_basic_trunc_flush.hpp"

namespace iosig::benchmarks::io_generators {

    struct basic_io_generator : public io_generator_base <file_handlers::file_handler_basic <>> {
        explicit basic_io_generator (const std::string &destination):
                io_generator_base <file_handlers::file_handler_basic <>> {destination} {}
    };

    struct basic_sync_io_generator : public io_generator_base <file_handlers::file_handler_basic_flush <>> {
        explicit basic_sync_io_generator (const std::string &destination):
                io_generator_base <file_handlers::file_handler_basic_flush <>> {destination} {}
    };

    struct buffered_io_generator : public io_generator_base <file_handlers::file_handler_buffered <>> {
        explicit buffered_io_generator (const std::string &destination):
                io_generator_base <file_handlers::file_handler_buffered <>> {destination} {}
    };

    struct direct_io_generator : public io_generator_base <file_handlers::file_handler_basic <0, O_DIRECT>> {
    explicit direct_io_generator (const std::string &destination):
            io_generator_base <file_handlers::file_handler_basic <0, O_DIRECT>> {destination} {}
    };

    struct basic_trunc_io_generator : public io_generator_base <file_handlers::file_handler_basic_trunc <>> {
        explicit basic_trunc_io_generator (const std::string &destination):
                io_generator_base <file_handlers::file_handler_basic_trunc <>> {destination} {}
    };

    struct basic_trunc_flush_io_generator : public io_generator_base <file_handlers::file_handler_basic_trunc_flush <>> {
        explicit basic_trunc_flush_io_generator (const std::string &destination):
                io_generator_base <file_handlers::file_handler_basic_trunc_flush <>> {destination} {}
    };

    struct basic_trunc_direct_io_generator : public io_generator_base <file_handlers::file_handler_basic_trunc <0, O_DIRECT>> {
        explicit basic_trunc_direct_io_generator (const std::string &destination):
                io_generator_base <file_handlers::file_handler_basic_trunc <0, O_DIRECT>> {destination} {}
    };

    struct mmap_io_generator : public io_generator_base <file_handlers::file_handler_mmap <>> {
    explicit mmap_io_generator (const std::string &destination):
            io_generator_base <file_handlers::file_handler_mmap <>> {destination} {}
    };

    struct fstream_io_generator : public io_generator_base <file_handlers::file_handler_fstream> {
        explicit fstream_io_generator (const std::string &destination):
                io_generator_base <file_handlers::file_handler_fstream> {destination} {}
    };

}


#endif //IOSIG_IO_GENERATORS_DECLS_HPP
