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

#ifndef IOSIG_SYSTEM_CONFIG_HPP
#define IOSIG_SYSTEM_CONFIG_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <list>
#include <regex>

#include "iosig_exceptions.hpp"

namespace iosig::sys_inspect {

    class system_config {
    private:

        std::string path;
        std::fstream file;

        static std::string search_for_default_config_file ();

    public:
        class config_data {
        public:
            std::string type;
            std::string name;
            std::list <std::pair <std::string, std::string>> paired_values;
            std::list <std::pair <std::string, std::list <std::string>>> list_values;

            explicit config_data (std::string obj_type, std::string obj_name) :
                    type {std::move (obj_type)}, name {std::move (obj_name)} {}
        };

        std::list <config_data> conf_data;

        system_config () : system_config (search_for_default_config_file ()) {}

        explicit system_config (const std::string &config_file) : path {config_file}, file {config_file} {
            parse ();
        }

        void parse ();

    };

}

#endif //IOSIG_SYSTEM_CONFIG_HPP
