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


 #ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <chrono>
#include <string>
#include <array>
#include <list>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <cstdio>
#include <filesystem>

#include "benchmark_base.hpp"
#include "iosig_exceptions.hpp"
#include "fill_memory.hpp"


namespace iosig::benchmarks::strategies {

    template <typename IO_Generator>
    class benchmark_blocksizes: public benchmark_base <IO_Generator> {
    private:

        using benchmark_helper::B;
        using benchmark_helper::KB;
        using benchmark_helper::MB;
        using benchmark_helper::s;
        using benchmark_helper::bw_GBs;
        using benchmark_base <IO_Generator>::io_generator;
        using benchmark_base <IO_Generator>::result;


        static constexpr const long long benchmark_duration_per_range       = 120 * s;
        static constexpr const size_t max_op_count                          = 100000;
        static constexpr const unsigned long long max_file_size             = 200 * MB;

        //                                      min                 max                  step
        static inline const range range1 {32*B,          512*B,          32*B};
        static inline const range range2 {128*B,         8*KB,           128*B};
        static inline const range range3 {512*B,         128*KB,         512*B};
        static inline const range range4 {1*MB - 64*KB,  1*MB + 64*KB,   512*B};


        template <typename ... P,
                class = std::enable_if_t <((common::generic::is_base_of_template_v <range, P>) && ...)>>
        inline void run_ranges_benchmark (const P &... args);

        template <typename T>
        decltype (auto) run_range_benchmark_for_size (const size_t file_size, const range <T> &, const bool store);

    public:

        explicit benchmark_blocksizes (const std::string_view &benchmark_name,
                                       const std::string &mountpoint,
                                       const std::string &prefix,
                                       unsigned long long free_space):
            benchmark_base <IO_Generator> {benchmark_name, mountpoint, prefix, free_space} {};

        void run_all_benchmarks () override;

    };

    template <typename IO_Generator>
    void benchmark_blocksizes <IO_Generator>::run_all_benchmarks () {

        utils::fill_memory fill_mem {};
        run_ranges_benchmark (range1, range2, range3, range4);

    }

    template <typename IO_Generator>
    template <typename... P, class>
    void benchmark_blocksizes <IO_Generator>::run_ranges_benchmark (const P &... args) {

        std::vector <std::common_type_t <P ...>> ranges {{args ...}};

        for (const auto &rng: ranges) {
            auto mini_bm_size = static_cast <unsigned long long> (rng.end);
            const auto mini_time = run_range_benchmark_for_size (mini_bm_size, rng, false);
            auto scaled_size = (mini_time > 0) ?
                               (mini_bm_size * benchmark_duration_per_range) / mini_time : max_file_size;
            auto bm_size = std::clamp (scaled_size, mini_bm_size, max_file_size);
            run_range_benchmark_for_size (bm_size, rng, true);
        }
    }


    template <typename IO_Generator>
    template <typename T>
    decltype (auto) benchmark_blocksizes <IO_Generator>::
    run_range_benchmark_for_size (const size_t file_size, const range <T> &rng, const bool store) {

        long long totaltime = 0;

        for (const auto &bs: rng.data) {

            size_t ts = bs * max_op_count;
            if (ts > file_size) {
                ts = file_size - file_size % bs;
            }

            const auto seq_io = io_generator.generate_sequential_io (bs, ts);
            const auto rnd_io = io_generator.generate_random_io (bs, ts);

            totaltime += seq_io.first + seq_io.second;
            totaltime += rnd_io.first + rnd_io.second;

            if (store) {
                result.data.emplace (bs, utils::result_data::benchmark_result {
                        bw_GBs (ts, seq_io.first),
                        bw_GBs (ts, seq_io.second),
                        bw_GBs (ts, rnd_io.first),
                        bw_GBs (ts, rnd_io.second)});
            }
        }

        return totaltime;
    }

}






#endif
