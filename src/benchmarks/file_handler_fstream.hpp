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

#ifndef IOSIG_FILE_HANDLER_FSTREAM_HPP
#define IOSIG_FILE_HANDLER_FSTREAM_HPP

#include <fstream>
#include <utility>

#include "iosig_exceptions.hpp"
#include "file_handler_base.hpp"

namespace iosig::benchmarks::file_handlers {

    class file_handler_fstream : public file_handler_base {
    protected:
        std::ofstream ofd {};
        std::ifstream ifd {};
    public:
        explicit file_handler_fstream (const std::string &filepath, const size_t size):
                file_handler_base {filepath, size} {}


        file_handler_fstream (file_handler_fstream &&fh) noexcept: file_handler_base (std::move (fh)) {
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


        file_handler_fstream &operator = (file_handler_fstream &&fh) noexcept {
            file_handler_base::operator= (std::move (fh));
            ofd = std::move (fh.ofd);
            ifd = std::move (fh.ifd);
            return *this;
        }

        ~file_handler_fstream () noexcept {
            if (ofd.is_open ()) {
                ofd.flush ();
                ofd.close ();
            }
            if (ifd.is_open ()) {
                ifd.close ();
            }
        }

    };



    void file_handler_fstream::open_for_write () {
        ofd.open (path, std::fstream::out | std::fstream::binary | std::fstream::trunc);

        if (!ofd.is_open ()) {
            throw common::exceptions::benchmark_failure ("file_handler_fstream: Could not create the benchmarking file");
        }
    }

    void file_handler_fstream::open_for_read () {
        ifd.open (path, std::ifstream::in | std::ifstream::binary);

        if (!ifd.is_open ()) {
            throw common::exceptions::benchmark_failure ("file_handler_fstream: Could not open the benchmarking file for reading");
        }
    }

    void file_handler_fstream::write_data (const std::vector <char> &data) {
        ofd.write (data.data (), static_cast <long> (data.size ()));
    }

    void file_handler_fstream::read_data (std::vector <char> &data) {
        ifd.read (data.data (), static_cast <long> (data.size ()));
    }

    void file_handler_fstream::seek_file_reader (const ssize_t offset) {
        ifd.seekg (offset);
    }

    void file_handler_fstream::seek_file_writer (const ssize_t offset) {
        ofd.seekp (offset);
    }

    void file_handler_fstream::reset () {
        ofd.flush ();
        close_file ();
    }

    void file_handler_fstream::close_file () {
        if (ofd.is_open ()) {
            ofd.close ();
        }
        if (ifd.is_open ()) {
            ifd.close ();
        }
    }


}


#endif //IOSIG_FILE_HANDLER_FSTREAM_HPP
