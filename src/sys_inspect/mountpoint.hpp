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
// Created by Masoud Gholami on 25.04.21.
//

#ifndef IOSIG_MOUNTPOINT_HPP
#define IOSIG_MOUNTPOINT_HPP

#include <utility>
#include <unordered_set>
#include <memory>

#include "blockdevice.hpp"
#include "filesystem.hpp"

namespace iosig::sys_inspect {

    class mountpoint {
    private:
        inline const static std::unordered_set <std::string>
                system_mountpoints {"/run", "/sys", "/dev", "/boot"};
    public:
        explicit mountpoint (std::string path,
                             std::shared_ptr <blockdevice> blockdev,
                             std::shared_ptr <filesystem> filesys) :
                path {std::move (path)},
                bd {std::move (blockdev)},
                fs {std::move (filesys)},
                label {bd->label} {
        }

        // benchmarks
        double op_latency {};
        double seek_latency {};
        long max_r_iops {};
        long max_w_iops {};
        long seq_read_bw {};
        long seq_write_bw {};


        const std::string path {};
        const std::shared_ptr <blockdevice> bd;
        const std::shared_ptr <filesystem> fs;
        std::string label {};
        bool global {};
        long mttdl {};
        unsigned long long capacity {};
        unsigned long long free_space {};


        // from config
        std::string prefix;
        bool ignore = false;

        inline static bool is_system_mountpoint (std::string &mp) {
            if (mp == "/dev/shm") {
                return false;
            }

            return std::any_of (system_mountpoints.begin (), system_mountpoints.end (), [&mp] (auto &smp) {
                return mp.substr (0, smp.size ()) == smp;
            });

        }

        inline static bool is_valid_mountpoint (std::string &mp) {
            return mp.substr (0, 1) == "/";
        }

        mountpoint (const mountpoint &) = delete;

        mountpoint &operator= (const mountpoint &) = delete;

    };
}

#endif //IOSIG_MOUNTPOINT_HPP
