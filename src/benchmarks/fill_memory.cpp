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

#include "fill_memory.hpp"

iosig::benchmarks::utils::fill_memory::fill_memory () {
    if (instantiated) {
        throw common::exceptions::fill_memory_failure ("Multiple instances of the class fill_memory is not allowed!");
    }

    long pages = sysconf (_SC_AVPHYS_PAGES);
    long page_size = sysconf (_SC_PAGE_SIZE);
    mem_size = static_cast <size_t> (pages) * static_cast <size_t> (page_size);
    data = std::make_unique <unsigned char> (mem_size);
    mlock (data.get (), mem_size);
    instantiated = true;
}

iosig::benchmarks::utils::fill_memory::~fill_memory () {
    munlock (data.get(), mem_size);
    instantiated = false;
}