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

#ifndef IOSIG_RESULT_DATA_HPP
#define IOSIG_RESULT_DATA_HPP

#include <iostream>
#include <map>
#include <string_view>

namespace iosig::benchmarks::utils {

    struct result_data {

        const std::string_view benchmark_name;

        explicit result_data (const std::string &name, const std::string &mountpoint):
            benchmark_name {name + " on " + mountpoint} {}

        struct benchmark_result {
            double  rnd_write,
                    rnd_read,
                    seq_write,
                    seq_read;
        };

        std::map <unsigned long long, benchmark_result> data;

        friend std::ostream &operator << (std::ostream &os, const utils::result_data &results) {
            os << std::fixed << std::showpoint;
            for (const auto &result_pair: results.data) {
                const auto &key = result_pair.first;
                const auto &result = result_pair.second;
                os << "Scheme: " << results.benchmark_name
                   << " key " << key
                   << " rnd_write " << result.rnd_write
                   << " rnd_read " << result.rnd_read
                   << " seq_write " << result.seq_write
                   << " seq_read " << result.seq_read
                   << "\n";
            }
            os.unsetf (std::ios::showpoint | std::ios::fixed);
            return os;
        }

    };


}



#endif //IOSIG_RESULT_DATA_HPP
