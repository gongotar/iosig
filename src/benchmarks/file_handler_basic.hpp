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

#ifndef IOSIG_FILE_HANDLER_BASIC_HPP
#define IOSIG_FILE_HANDLER_BASIC_HPP

#include <type_traits>

#include <unistd.h>
#include <fcntl.h>      // open
#include <unistd.h>     // write

#include "iosig_exceptions.hpp"
#include "file_handler_base.hpp"

namespace iosig::benchmarks::file_handlers {

    template <int readflags = 0, int writeflags = 0, bool check = false>
    class file_handler_basic : public file_handler_base {
    protected:
        int fd {};
    public:
        explicit file_handler_basic (std::string filepath, const size_t size):
                file_handler_base {std::move (filepath), size} {}

        file_handler_basic (file_handler_basic &&fh) noexcept: file_handler_base (std::move (fh)){
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

        int get_file_descriptor () {return fd;}

        file_handler_basic &operator = (file_handler_basic &&fh) noexcept {
            file_handler_base::operator= (std::move (fh));
            fd = fh.fd;
            fh.fd = int {};
            return *this;
        }

        ~file_handler_basic() noexcept {
            syncfs (fd);
            close (fd);
            fd = int {};
        }
    };



    template <int readflags, int writeflags, bool check>
    void file_handler_basic <readflags, writeflags, check>::open_for_write () {
        if (fd != 0) {
            throw common::exceptions::file_handler_failure ("file_handler_basic: Please close the previously opened file");
        }
        fd = open (path.c_str (), O_CREAT | O_TRUNC | O_RDWR | writeflags, S_IWUSR | S_IRUSR);
        if (fd < 0) {
            perror ("Error in creating the file");
            throw common::exceptions::file_handler_failure ("file_handler_basic: Could not open the file");
        }
    }

    template <int readflags, int writeflags, bool check>
    void file_handler_basic <readflags, writeflags, check>::open_for_read () {
        if (fd != 0) {
            throw common::exceptions::file_handler_failure ("file_handler_basic: Please close the previously opened file");
        }
        fd = open (path.c_str(), O_RDONLY | readflags);
        if (fd < 0) {
            perror ("Error in opening the file");
            throw common::exceptions::file_handler_failure ("file_handler_basic: Could not open the file");
        }
    }

    template <int readflags, int writeflags, bool check>
    void file_handler_basic <readflags, writeflags, check>::write_data (const std::vector <char> &data) {
        auto ret = write (fd, data.data(), data.size());
        if constexpr (check) {
            if (ret != static_cast <long> (data.size())) {
                perror ("Write operation failed to write data");
                throw common::exceptions::file_handler_failure ("file_handler_basic: Could not write to the file");
            }
        }
    }

    template <int readflags, int writeflags, bool check>
    void file_handler_basic <readflags, writeflags, check>::read_data (std::vector <char> &data) {
        read (fd, data.data(), data.size());
    }

    template <int readflags, int writeflags, bool check>
    void file_handler_basic <readflags, writeflags, check>::seek_file_reader (const ssize_t offset) {
        auto ret = lseek (fd, offset, SEEK_SET);
        if constexpr (check) {
            if (ret == -1) {
                perror ("Seek operation failed");
                throw common::exceptions::file_handler_failure ("file_handler_basic: Could not seek to the offset");
            }
        }
    }

    template <int readflags, int writeflags, bool check>
    void file_handler_basic <readflags, writeflags, check>::seek_file_writer (const ssize_t offset) {
        seek_file_reader (offset);
    }

    template <int readflags, int writeflags, bool check>
    void file_handler_basic <readflags, writeflags, check>::close_file () {
        close (fd);
    }

    template <int readflags, int writeflags, bool check>
    void file_handler_basic <readflags, writeflags, check>::reset () {
        syncfs (fd);
        close (fd);
        fd = int {};
    }
}




#endif //IOSIG_FILE_HANDLER_BASIC_HPP
