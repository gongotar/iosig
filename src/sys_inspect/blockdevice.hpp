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
// Created by Masoud Gholami on 24.04.21.
//

#ifndef IOSIG_BLOCKDEVICE_HPP
#define IOSIG_BLOCKDEVICE_HPP

#include <string>
#include <memory>
#include <list>
#include <utility>
#include <cassert>
#include <cstdio>

#include "filesystem.hpp"

namespace iosig::sys_inspect {

    class blockdevice {
    public:
        explicit blockdevice (std::string path) : path (std::move (path)) {}

        std::shared_ptr <filesystem> fs;
        const std::string path;

        // lsblk
        long physical_sector {};
        long logical_sector {};
        unsigned long long capacity {};
        std::string label;
        std::string model;
        int minor {};
        int major {};


        blockdevice (const blockdevice &) = delete;

        blockdevice &operator= (const blockdevice &) = delete;

    };

    class simple_blockdevice : public blockdevice {
    public:
        explicit simple_blockdevice (std::string path) : blockdevice (std::move (path)) {}

        long mttf {};
        bool is_volatile {};
        double ber {};
    };

    class complex_blockdevice : public blockdevice {
    public:
        explicit complex_blockdevice (std::string path) : blockdevice (std::move (path)) {}

        std::list <std::shared_ptr <blockdevice>> sub_devices;
    };

    class partition_blockdevice : public blockdevice {
    public:
        explicit partition_blockdevice (std::string path) : blockdevice (std::move (path)) {}

        std::shared_ptr <blockdevice> owner;
    };

    class lvm_blockdevice : public blockdevice {
    public:
        explicit lvm_blockdevice (std::string path) : blockdevice (std::move (path)) {}

        std::list <std::shared_ptr <blockdevice>> parents;
    };

    class remote_blockdevice : public blockdevice {
    public:
        std::string server;
        std::string path_on_server;

        explicit remote_blockdevice (const std::string &path) : blockdevice (path) {
            auto separator = path.find_first_of (':');
            assert (separator != std::string::npos);

            server = path.substr (0, separator);
            path_on_server = path.substr (separator + 1, path.size ());
        }
    };

}
#endif //IOSIG_BLOCKDEVICE_HPP
