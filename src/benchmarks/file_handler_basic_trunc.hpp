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
// Created by Masoud Gholami on 09.07.21.
//

#ifndef IOSIG_FILE_HANDLER_BASIC_TRUNC_HPP
#define IOSIG_FILE_HANDLER_BASIC_TRUNC_HPP

#include <type_traits>

#include <unistd.h>
#include <fcntl.h>      // open
#include <unistd.h>     // write

#include "iosig_exceptions.hpp"
#include "file_handler_basic.hpp"

namespace iosig::benchmarks::file_handlers {

    template <int readflags = 0, int writeflags = 0, bool check = false>
    class file_handler_basic_trunc : public file_handler_basic <readflags, writeflags, check> {
    public:
        explicit file_handler_basic_trunc (std::string filepath, const size_t size):
                file_handler_basic <readflags, writeflags, check> {std::move (filepath), size} {}

        inline void open_for_write () {
            file_handler_basic <readflags, writeflags, check>::open_for_write ();
            ftruncate (file_handler_basic <readflags, writeflags, check>::fd, file_handler_basic <readflags, writeflags, check>::size);
        }
    };

}

#endif //IOSIG_FILE_HANDLER_BASIC_TRUNC_HPP
