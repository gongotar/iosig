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

#ifndef IOSIG_FILL_MEMORY_HPP
#define IOSIG_FILL_MEMORY_HPP

#include <unistd.h>
#include <sys/mman.h>
#include <memory>

#include "iosig_exceptions.hpp"

namespace iosig::benchmarks::utils {

    class fill_memory {
    private:
        inline static bool instantiated = false;

        size_t mem_size;
        std::unique_ptr <unsigned char> data;

    public:

        fill_memory();

        ~fill_memory();

        fill_memory (const fill_memory &) = delete;
        fill_memory (fill_memory &&) = delete;
        fill_memory& operator= (const fill_memory &) = delete;
        fill_memory& operator= (fill_memory &&) = delete;

    };

}

#endif //IOSIG_FILL_MEMORY_HPP
