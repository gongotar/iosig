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
// Created by Masoud Gholami on 18.08.21.
//

#ifndef IOSIG_PLOT_BENCHMARKS_HPP
#define IOSIG_PLOT_BENCHMARKS_HPP

#include <type_traits>
#include <list>
#include <memory>

//#include "qcustomplot.h"

#include "result_data.hpp"


namespace iosig::plotter {

    class plot_benchmarks {
    private:
        std::list <benchmarks::utils::result_data> plot_data;
    public:
        template <typename ... Data,
                typename = std::enable_if_t <(std::is_same_v <Data, iosig::benchmarks::utils::result_data> && ...)>>
        explicit plot_benchmarks (const Data& ... data) {
            (plot_data.push_back (data), ...);
        }

        void show () const;
    };
}


#endif //IOSIG_PLOT_BENCHMARKS_HPP
