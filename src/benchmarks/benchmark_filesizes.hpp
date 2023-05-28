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

#ifndef IOSIG_BENCHMARK_FILESIZES_HPP
#define IOSIG_BENCHMARK_FILESIZES_HPP

#include "benchmark_base.hpp"
#include "fill_memory.hpp"


namespace iosig::benchmarks::strategies {
    template <typename IO_Generator>
    class benchmark_filesizes: public benchmark_base <IO_Generator> {
    private:

        using benchmark_helper::B;
        using benchmark_helper::KB;
        using benchmark_helper::MB;
        using benchmark_helper::GB;
        using benchmark_helper::s;
        using benchmark_helper::m;
        using benchmark_helper::bw_GBs;
        using benchmark_helper::duration_from;
        using benchmark_base <IO_Generator>::io_generator;
        using benchmark_base <IO_Generator>::result;
        using benchmark_base <IO_Generator>::free_space;

        static constexpr const size_t bs = 1*KB;

        //                                      min                 max                  step
        static inline const range range1 {8*KB,          128*KB,         4*KB};
        static inline const range range2 {128*KB,        2*MB,           128*KB};
        static inline const range range3 {20*MB,         200*MB,         20*MB};
        static inline const range range4 {2*GB,          10*GB,          2*GB};
        static inline const range range5 {40*GB,         200*GB,         40*GB};

        static constexpr const long long max_duration                   = 10*m;
        static constexpr const unsigned long long leave_free_capacity   = 2*GB;

        std::chrono::time_point<std::chrono::high_resolution_clock>  bm_start;

        template <typename ... P,
                class = std::enable_if_t <((common::generic::is_base_of_template_v <range, P>) && ...)>>
        inline void run_ranges_benchmark (const P &... args);

        template <typename T>
        void run_range_benchmark (const range <T> &rng);

        inline bool break_benchmarking (unsigned long long size);


    public:

        explicit benchmark_filesizes (const std::string_view &benchmark_name,
                                      const std::string &mountpoint,
                                      const std::string &prefix,
                                      unsigned long long free_capacity):
                benchmark_base <IO_Generator> {benchmark_name, mountpoint, prefix, free_capacity} {};

        void run_all_benchmarks () override;


    };


    template <typename IO_Generator>
    void benchmark_filesizes <IO_Generator>::run_all_benchmarks () {

        utils::fill_memory fill_mem {};
        run_ranges_benchmark (range1, range2, range3, range4, range5);

    }

    template <typename IO_Generator>
    template <typename... P, class>
    void benchmark_filesizes <IO_Generator>::run_ranges_benchmark (const P &... args) {

        std::vector <std::common_type_t <P ...>> ranges {{args ...}};

        bm_start = std::chrono::high_resolution_clock::now ();
        for (const auto &rng: ranges) {
            run_range_benchmark (rng);
        }
    }


    template <typename IO_Generator>
    template <typename T>
    void benchmark_filesizes <IO_Generator>::run_range_benchmark (const range <T> &rng) {

        for (const auto &ts: rng.data) {

            if (break_benchmarking (ts)) {
                break;
            }

            const auto seq_io = io_generator.generate_sequential_io (bs, ts);
            const auto rnd_io = io_generator.generate_random_io (bs, ts);

            result.data.emplace (ts, utils::result_data::benchmark_result {
                    bw_GBs (ts, seq_io.first),
                    bw_GBs (ts, seq_io.second),
                    bw_GBs (ts, rnd_io.first),
                    bw_GBs (ts, rnd_io.second)});
        }
    }



    template <typename IO_Generator>
    inline bool benchmark_filesizes <IO_Generator>::break_benchmarking (unsigned long long size) {
        if (size + leave_free_capacity >= free_space) {
            std::clog << "filesize " << size << " exceeded maximum allowed capacity of " <<
                      free_space << " minus " << leave_free_capacity << std::endl;
            return true;
        }
        else if (const auto bm_duration = duration_from (bm_start); bm_duration > max_duration) {
            std::clog << "benchmarking duration " << bm_duration
                      << " exceeded maximum allowed duration of " << max_duration << std::endl;
            return true;
        }
        else {
            return false;
        }
    }

}



#endif //IOSIG_BENCHMARK_FILESIZES_HPP
