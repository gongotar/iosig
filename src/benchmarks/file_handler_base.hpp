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

#ifndef IOSIG_FILE_HANDLER_BASE_HPP
#define IOSIG_FILE_HANDLER_BASE_HPP

#include <string>
#include <vector>

namespace iosig::benchmarks::file_handlers {

    class file_handler_base {
    protected:

        std::string path;
        size_t size;

    public:
        explicit file_handler_base (std::string filepath, const size_t size):
                path {std::move(filepath)}, size {size} {
        }


        file_handler_base (file_handler_base &&fh) {
            *this = std::move (fh);
        }

        /*
        template <typename T>
        inline void open_for_write (T &&)                        {}

        template <typename T>
        inline void open_for_read (T &&)                         {}

        template <typename T>
        inline void write_data (const std::vector <char>&, T &&) {}

        template <typename T>
        inline void read_data (std::vector <char>&, T &&)        {}

        template <typename T>
        inline void seek_file_reader (const ssize_t, T &&)       {}

        template <typename T>
        inline void seek_file_writer (const ssize_t, T &&)       {}

        template <typename T>
        inline void reset (T &&)                                 {}

        template <typename T>
        inline void close_file (T &&)                            {}
         */

        [[nodiscard]] inline std::string get_path () const {
            return path;
        }

        file_handler_base (const file_handler_base &) = delete;
        file_handler_base &operator = (const file_handler_base &) = delete;

        file_handler_base &operator = (file_handler_base &&fh) noexcept {
            size = fh.size;
            path = std::move (fh.path);
            fh.size = 0;
            return *this;
        }

        ~file_handler_base () noexcept {
            std::remove (path.c_str ());
        };

    };


    /*
    template <typename ... File_Handler>
    struct combine_file_handlers: File_Handler ... {

        combine_file_handlers (const std::string &filepath, const size_t size):
                File_Handler (filepath, size) ... {
            static_assert ((std::is_base_of_v <file_handler_base, File_Handler> && ...));
        }

        constexpr inline void open_for_write () {
            (static_cast <File_Handler *> (this)->open_for_write (*this), ...);
        }

        constexpr inline void open_for_read () {
            (static_cast <File_Handler *> (this)->open_for_read (*this), ...);
        }

        constexpr inline void write_data (const std::vector <char> &data) {
            (static_cast <File_Handler *> (this)->write_data (data, *this), ...);
        }

        constexpr inline void read_data (std::vector <char> &data) {
            (static_cast <File_Handler *> (this)->read_data (data, *this), ...);
        }

        constexpr inline void seek_file_reader (const size_t size) {
            (static_cast <File_Handler *> (this)->seek_file_reader (size, *this), ...);
        }

        constexpr inline void seek_file_writer (const size_t size) {
            (static_cast <File_Handler *> (this)->seek_file_writer (size, *this), ...);
        }

        constexpr inline void reset () {
            (static_cast <File_Handler *> (this)->reset (*this), ...);
        }

        constexpr inline void close_file () {
            (static_cast <File_Handler *> (this)->close_file (*this), ...);
        }
    };
    */
}


#endif //IOSIG_FILE_HANDLER_BASE_HPP
