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
// Created by Masoud Gholami on 06.07.21.
//

#ifndef IOSIG_IO_GENERATOR_BASE_HPP
#define IOSIG_IO_GENERATOR_BASE_HPP

#include <cstddef>
#include <utility>
#include <string>
//#include <concepts>
#include <memory>
#include <chrono>
#include <random>


#include "file_handler_base.hpp"

namespace iosig::benchmarks::io_generators {

    template <typename File_Handler_T, typename Validator = void *, typename =
            std::enable_if_t <std::is_base_of_v <file_handlers::file_handler_base, File_Handler_T>>>
    class io_generator_base {
    protected:
        const std::string dest;
        typedef File_Handler_T File_Handler;
        Validator validator;
    public:

        explicit io_generator_base (std::string destination, Validator v = nullptr):
                dest {std::move (destination)}, validator {std::move (v)} {}

        [[nodiscard]] virtual std::pair <long, long>
        generate_sequential_io (const size_t blocksize, const size_t totalsize) const;

        [[nodiscard]] virtual std::pair <long, long>
        generate_random_io (const size_t blocksize, const size_t totalsize) const;

        virtual ~io_generator_base() noexcept = default;
    };


    template <typename File_Handler_T, typename Validator, typename T>
    std::pair <long, long>
    io_generator_base <File_Handler_T, Validator, T>::
    generate_sequential_io (const size_t blocksize, const size_t totalsize) const {

        if (totalsize == 0 || blocksize == 0 || totalsize % blocksize != 0) {
            throw common::exceptions::benchmark_failure ("io_generator_base: Invalid totalsize/blocksize");
        }

        std::unique_ptr fh = std::make_unique <File_Handler_T> (dest, totalsize);
        fh->open_for_write ();

        std::vector <char> data (blocksize);

        unsigned long i = 0;

        const auto start_w = std::chrono::high_resolution_clock::now ();

        for (size_t written = 0; written < totalsize; written += blocksize) {
            data.at (i % blocksize) = static_cast <char> (i);
            fh->write_data (data);
            i++;
        }

        fh->reset();

        const auto stop_w = std::chrono::high_resolution_clock::now ();

        fh->open_for_read ();

        const auto start_r = std::chrono::high_resolution_clock::now ();

        for (size_t read = 0; read < totalsize; read += blocksize) {
            fh->read_data (std::ref (data));
            if constexpr (std::is_invocable_r_v <bool, Validator, decltype (data)>) {
                if (!validator (data)) {
                    throw common::exceptions::file_handler_failure ("io_generator_base: Non-matching sequentially read data");
                }
            }
        }

        fh->close_file ();
        const auto stop_r = std::chrono::high_resolution_clock::now ();

        const auto dur_w = std::chrono::duration_cast <std::chrono::microseconds> (stop_w - start_w);
        const auto dur_r = std::chrono::duration_cast <std::chrono::microseconds> (stop_r - start_r);
        return std::make_pair (dur_w.count (), dur_r.count ());

    }


    template <typename File_Handler_T, typename Validator, typename T>
    std::pair <long, long>
    io_generator_base <File_Handler_T, Validator, T>::
    generate_random_io (const size_t blocksize, const size_t totalsize) const {
        if (totalsize == 0 || blocksize == 0 || totalsize % blocksize != 0) {
            throw common::exceptions::benchmark_failure ("io_generator_base: Invalid totalsize/blocksize");
        }

        std::unique_ptr fh = std::make_unique <File_Handler> (dest, totalsize);
        fh->open_for_write ();

        std::vector <char> data (blocksize);

        // shuffle the order of blocks
        const unsigned long nblocks = totalsize / blocksize;
        std::vector <size_t> block_orders (nblocks);
        unsigned long i = 0;
        for (size_t block_offset = 0; block_offset < totalsize; block_offset += blocksize) {
            block_orders.at (i++) = block_offset;
        }
        std::shuffle (block_orders.begin (), block_orders.end (), std::default_random_engine (0));

        ssize_t offset;
        const auto start_w = std::chrono::high_resolution_clock::now ();

        for (i = 0; i < nblocks; i ++) {
            offset = block_orders.at (i);

            fh->seek_file_writer (offset);
            data.at (i % blocksize) = static_cast <char> (i);
            fh->write_data (data);
        }
        fh->reset ();

        const auto stop_w = std::chrono::high_resolution_clock::now ();

        fh->open_for_read ();
        const auto start_r = std::chrono::high_resolution_clock::now ();

        for (i = 0; i < nblocks; i++) {
            offset = block_orders.at (i);
            fh->seek_file_reader (offset);
            fh->read_data (data);
            if constexpr (std::is_invocable_r_v <bool, Validator, decltype (data)>) {
                if (!validator (data)) {
                    throw common::exceptions::file_handler_failure ("io_generator_base: Non-matching randomly read data");
                }
            }
        }

        fh->close_file ();
        const auto stop_r = std::chrono::high_resolution_clock::now ();

        const auto dur_w = std::chrono::duration_cast <std::chrono::microseconds> (stop_w - start_w);
        const auto dur_r = std::chrono::duration_cast <std::chrono::microseconds> (stop_r - start_r);
        return std::make_pair (dur_w.count (), dur_r.count ());
    }

}



#endif //IOSIG_IO_GENERATOR_BASE_HPP
