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

#ifndef IOSIG_FILE_HANDLER_MMAP_HPP
#define IOSIG_FILE_HANDLER_MMAP_HPP

#include <type_traits>

#include <unistd.h>
#include <sys/mman.h>
#include <cstring>

#include "iosig_exceptions.hpp"
#include "file_handler_basic.hpp"

namespace iosig::benchmarks::file_handlers {


    template <int readflags = 0, int writeflags = 0, int mmap_readflags = 0, int mmap_writeflags = 0, bool check = false>
    class file_handler_mmap : public file_handler_basic <readflags, writeflags, check> {
    protected:
        char *mmapped_addr_w {nullptr};
        char *mmapped_addr_r {nullptr};
        using file_handler_basic <readflags, writeflags, check>::fd;
        using file_handler_basic <readflags, writeflags, check>::size;
        size_t seek_read_pointer {}, seek_write_pointer {};

    public:
        explicit file_handler_mmap (std::string filepath, const size_t size):
                file_handler_basic <readflags, writeflags, check> {std::move (filepath), size} {}

        file_handler_mmap (file_handler_mmap &&fh) noexcept: file_handler_base (std::move (fh)) {
            *this = std::move (fh);
        };

        inline void open_for_write ();
        inline void open_for_read ();
        inline void write_data (const std::vector <char>&);
        inline void read_data (std::vector <char>&);
        inline void seek_file_reader (const ssize_t);
        inline void seek_file_writer (const ssize_t);
        inline void reset ();
        inline void close_file ();

        file_handler_mmap &operator = (file_handler_mmap &&fh) noexcept {
            file_handler_basic <readflags, writeflags, check>::operator= (std::move (fh));
            mmapped_addr_w = fh.mmapped_addr_w;
            mmapped_addr_r = fh.mmapped_addr_r;
            seek_read_pointer = fh.seek_read_pointer;
            seek_write_pointer = fh.seek_write_pointer;
            fh.mmapped_addr_r = nullptr;
            fh.mmapped_addr_w = nullptr;
            fh.seek_pointer = 0;
            return *this;
        }

        ~file_handler_mmap () noexcept {
            if (mmapped_addr_w != nullptr) {
                munmap (mmapped_addr_w, size);
                mmapped_addr_w = nullptr;
            }
            if (mmapped_addr_r != nullptr) {
                munmap (mmapped_addr_r, size);
                mmapped_addr_r = nullptr;
            }
        }
    };



    template <int readflags, int writeflags, int mmap_readflags, int mmap_writeflags, bool check>
    void file_handler_mmap <readflags, writeflags, mmap_readflags, mmap_writeflags, check>::open_for_write () {

        file_handler_basic <readflags, writeflags, check>::open_for_write ();
        ftruncate (fd, size);

        if (mmapped_addr_w != nullptr) {
            throw common::exceptions::file_handler_failure ("file_handler_mmap: write mmap is called again before unmapping the previous mapping");
        }

        mmapped_addr_w = static_cast <char *> (mmap (NULL, size, PROT_WRITE, MAP_SHARED | mmap_writeflags, fd, 0));
        if (mmapped_addr_w == MAP_FAILED) {
            perror ("Error in mapping to the file");
            throw common::exceptions::file_handler_failure ("file_handler_mmap: Could not mmap to the file");
        }
    }

    template <int readflags, int writeflags, int mmap_readflags, int mmap_writeflags, bool check>
    void file_handler_mmap <readflags, writeflags, mmap_readflags, mmap_writeflags, check>::open_for_read () {

        if (mmapped_addr_r != nullptr) {
            throw common::exceptions::file_handler_failure ("read mmap is called again before unmapping the previous mapping");
        }

        file_handler_basic <readflags, writeflags, check>::open_for_read ();

        mmapped_addr_r = static_cast <char *> (mmap (NULL, size, PROT_READ, MAP_SHARED | mmap_readflags, fd, 0));
        if (mmapped_addr_w == MAP_FAILED) {
            perror ("Error in mapping to the file");
            throw common::exceptions::file_handler_failure ("file_handler_mmap: Could not mmap to the file");
        }
    }

    template <int readflags, int writeflags, int mmap_readflags, int mmap_writeflags, bool check>
    void file_handler_mmap <readflags, writeflags, mmap_readflags, mmap_writeflags, check>::write_data (const std::vector <char> &data) {
        memcpy (mmapped_addr_w + seek_write_pointer, data.data (), data.size ());
        seek_write_pointer += data.size ();
    }

    template <int readflags, int writeflags, int mmap_readflags, int mmap_writeflags, bool check>
    void file_handler_mmap <readflags, writeflags, mmap_readflags, mmap_writeflags, check>::read_data (std::vector <char> &data) {
        memcpy (data.data (), mmapped_addr_r + seek_read_pointer, data.size ());
        seek_read_pointer += data.size ();
    }

    template <int readflags, int writeflags, int mmap_readflags, int mmap_writeflags, bool check>
    void file_handler_mmap <readflags, writeflags, mmap_readflags, mmap_writeflags, check>::seek_file_reader (const ssize_t offset) {
        seek_read_pointer = offset;
    }

    template <int readflags, int writeflags, int mmap_readflags, int mmap_writeflags, bool check>
    void file_handler_mmap <readflags, writeflags, mmap_readflags, mmap_writeflags, check>::seek_file_writer (const ssize_t offset) {
        seek_write_pointer = offset;
    }

    template <int readflags, int writeflags, int mmap_readflags, int mmap_writeflags, bool check>
    void file_handler_mmap <readflags, writeflags, mmap_readflags, mmap_writeflags, check>::close_file () {

        file_handler_basic <readflags, writeflags, check>::close_file();

        if (mmapped_addr_w != nullptr) {
            munmap (mmapped_addr_w, size);
            mmapped_addr_w = nullptr;
        }
        if (mmapped_addr_r != nullptr) {
            munmap (mmapped_addr_r, size);
            mmapped_addr_r = nullptr;
        }
    }

    template <int readflags, int writeflags, int mmap_readflags, int mmap_writeflags, bool check>
    void file_handler_mmap <readflags, writeflags, mmap_readflags, mmap_writeflags, check>::reset () {

        syncfs (fd);
        close_file ();
        fd = int {};
        seek_read_pointer = 0;
        seek_write_pointer = 0;
    }

}



#endif //IOSIG_FILE_HANDLER_MMAP_HPP
