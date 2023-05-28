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

#ifndef IOSIG_BENCHMARK_BASE_HPP
#define IOSIG_BENCHMARK_BASE_HPP

#include <string>
#include <array>
#include <vector>
#include <filesystem>
#include <iostream>
#include <type_traits>

#include "iosig_exceptions.hpp"
#include "io_generator_base.hpp"
#include "result_data.hpp"
#include "generic_utils.hpp"

namespace iosig::benchmarks::strategies {

    /**
     * Range class template to represent a range of values between begin and end,
     * incremented according to the given step and operation.
     * @tparam T
     * @tparam OP
     */
    template <typename T, typename OP = std::plus <>>
    struct range {
        const T begin, end, step;
        const std::vector <T> data;

        range (const T &begin, const T &end, const T &step, const OP &op = OP {}) noexcept:
                begin {begin}, end {end}, step {step},
                data {construct_range_vector (op)} {}

        inline auto construct_range_vector (const OP &op) {
            std::vector <T> vec;
            T item = begin;
            while (item < end) {
                vec.emplace_back (item);
                item = std::invoke (op, item, step);
            }
            vec.shrink_to_fit ();
            return vec;
        }
    };

    struct range_ll: public range <long long> {using range <long long>::range;};



    struct benchmark_helper {

        static constexpr const size_t B = 1;
        static constexpr const size_t KB = 1024 * B;
        static constexpr const size_t MB = 1024 * KB;
        static constexpr const size_t GB = 1024 * MB;
        static constexpr const long long s = 1000 * 1000;
        static constexpr const long long m = 60*s;

        static constexpr inline auto duration_from = [] (const auto &start_time) {
            return (std::chrono::duration_cast <std::chrono::microseconds>
                    (std::chrono::high_resolution_clock::now () - start_time)).count ();
        };

        static constexpr inline auto bw_GBs = [] (const auto size_B, const auto time_us) {
            return static_cast <double> (size_B * s) / static_cast <double> (time_us * GB);
        };

        template <typename ... T, class = std::enable_if_t <(std::is_same_v <T, std::string> && ...)>>
        static inline std::string combine_paths (const T &... args) {

            static auto fix_tail_and_head = [] (std::string str) {
                if (str.front () != '/') {
                    str.insert (0, "/");
                }
                if (str.back () == '/') {
                    str.pop_back ();
                }
                return str;
            };

            return (fix_tail_and_head (args) + ...);
        }

        /**
         * Non-constructable class (static class)
         */
        //benchmark_helper () = delete;

    };

    /**
     * Benchmarking base class. Every other benchmarking class should inherit from this class.
     * @tparam IO_Generator
     */
    template <typename IO_Generator,
            typename = std::enable_if_t <common::generic::is_base_of_template_v <io_generators::io_generator_base, IO_Generator>>>
    class benchmark_base: protected benchmark_helper {
    protected:
        const std::string dest;
        const IO_Generator io_generator;
        const unsigned long long free_space;

        static inline const std::string filename {"iosig_test"};

        explicit benchmark_base (const std::string &benchmark_name,
                                 const std::string &mountpoint,
                                 const std::string &prefix,
                                 unsigned long long free_space) :
                dest {combine_paths (mountpoint, prefix, filename)},
                io_generator {dest},
                free_space {free_space},
                result {benchmark_name, mountpoint} {

            std::filesystem::path p {dest};
            if (!std::filesystem::exists (p.parent_path ())) {
                if (!std::filesystem::create_directories (p.parent_path ())) {
                    throw common::exceptions::benchmark_failure ("Failed to create benchmarking directories!");
                }
            }

            std::clog << "The benchmarking destination for the mountpoint "
                      << mountpoint << " will be " << dest << std::endl;
        };

        utils::result_data result;

    public:
        virtual void run_all_benchmarks () = 0;
        const auto &get_result () {return result;}
    };
}
#endif //IOSIG_BENCHMARK_BASE_HPP
