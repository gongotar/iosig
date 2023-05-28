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
// Created by Masoud Gholami on 21.04.21.
//

#ifndef IOSIG_SYSTEM_MODEL_HPP
#define IOSIG_SYSTEM_MODEL_HPP

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>
#include <regex>

#include "iosig_exceptions.hpp"
#include "filesystem.hpp"
#include "blockdevice.hpp"
#include "mountpoint.hpp"
#include "system_config.hpp"
#include "benchmark_schemes.hpp"

namespace iosig::sys_inspect {

    class system_model {
    private:

        // shell
        static constexpr size_t max_command_output_size = 2000;
        static constexpr const char shell[] = "/bin/sh";
        static constexpr const char shell_exit[] = "exit 0";

        int read_from_shell[2] {};
        int write_to_shell[2] {};

        std::unordered_map <std::string, std::shared_ptr <filesystem>> filesystems;
        std::unordered_map <std::string, std::shared_ptr <blockdevice>> devices;

        system_config sysconf;

        void init_shell ();

        std::string exec_command (std::string) const;

        void finalize_shell ();


    public:
        system_model () {
            init_shell ();
        }

        ~system_model () {
            finalize_shell ();
        }

        std::list <mountpoint> mountpoints;

        void lookup_blockdevices ();

        void lookup_mountpoints ();

        void match_system_config ();

        void match_benchmark_config (); // defines a list of mountpoints for which
        // benchmarking has to be performed.
        // the list could be stored in a field of
        // mountpoints

        virtual void exec_benchmarks ();

        void update_config ();

    };

}

#endif //IOSIG_SYSTEM_MODEL_HPP
