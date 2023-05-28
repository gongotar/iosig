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

#ifndef IOSIG_FILE_HANDLER_BUFFERED_HPP
#define IOSIG_FILE_HANDLER_BUFFERED_HPP

#include <type_traits>

#include <cstdio>

#include "iosig_exceptions.hpp"
#include "file_handler_base.hpp"

namespace iosig::benchmarks::file_handlers {

    template <bool check = false>
    class file_handler_buffered : public file_handler_base {
    protected:
        FILE *fd = nullptr;
    public:
        explicit file_handler_buffered (std::string filepath, const size_t size):
                file_handler_base {std::move (filepath), size} {}

        file_handler_buffered (file_handler_buffered &&fh) noexcept: file_handler_base (std::move (fh)){
            *this = std::move (fh);
        }

        inline void open_for_write ();

        inline void open_for_read ();

        inline void write_data (const std::vector <char>&);

        inline void read_data (std::vector <char>&);

        inline void seek_file_reader (const ssize_t);

        inline void seek_file_writer (const ssize_t);

        inline void reset ();

        inline void close_file ();

        FILE *get_file_descriptor () {return fd;}

        file_handler_buffered &operator = (file_handler_buffered &&fh) noexcept {
            file_handler_base::operator= (std::move (fh));
            fd = fh.fd;
            fh.fd = nullptr;
            return *this;
        }

        ~file_handler_buffered() noexcept {
            fflush (fd);
            if (fd != nullptr) {
                fclose (fd);
            }
            fd = nullptr;
        }
    };



    template <bool check>
    void file_handler_buffered <check>::open_for_write () {
        if (fd != nullptr) {
            throw common::exceptions::file_handler_failure ("file_handler_buffered: Please close the previously opened file");
        }
        fd = fopen (path.c_str (), "w");
        if (fd == nullptr) {
            perror ("Error in creating the file");
            throw common::exceptions::file_handler_failure ("file_handler_buffered: Could not open the file");
        }
    }

    template <bool check>
    void file_handler_buffered <check>::open_for_read () {
        if (fd != nullptr) {
            throw common::exceptions::file_handler_failure ("file_handler_buffered: Please close the previously opened file");
        }
        fd = fopen (path.c_str (), "r");
        if (fd == nullptr) {
            perror ("Error in opening the file");
            throw common::exceptions::file_handler_failure ("file_handler_buffered: Could not open the file");
        }
    }

    template <bool check>
    void file_handler_buffered <check>::write_data (const std::vector <char> &data) {
        auto ret = fwrite (data.data(), sizeof (char), data.size(),fd);
        if constexpr (check) {
            if (ret != static_cast <long> (data.size())) {
                perror ("Write operation failed to write data");
                throw common::exceptions::file_handler_failure ("file_handler_buffered: Could not write to the file");
            }
        }
    }

    template <bool check>
    void file_handler_buffered <check>::read_data (std::vector <char> &data) {
        fread (data.data(), sizeof (char), data.size(),fd);
    }

    template <bool check>
    void file_handler_buffered <check>::seek_file_reader (const ssize_t offset) {
        auto ret = fseek (fd, offset, SEEK_SET);
        if constexpr (check) {
            if (ret != 0) {
                perror ("Seek operation failed");
                throw common::exceptions::file_handler_failure ("file_handler_buffered: Could not seek to the offset");
            }
        }
    }

    template <bool check>
    void file_handler_buffered <check>::seek_file_writer (const ssize_t offset) {
        seek_file_reader (offset);
    }

    template <bool check>
    void file_handler_buffered <check>::close_file () {
        fclose (fd);
        fd = nullptr;
    }

    template <bool check>
    void file_handler_buffered <check>::reset () {
        fflush (fd);
        fclose (fd);
        fd = nullptr;
    }
}




#endif //IOSIG_FILE_HANDLER_BUFFERED_HPP
