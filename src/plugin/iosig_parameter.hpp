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


 #ifndef IOSIG_PARAMETER_HPP
#define IOSIG_PARAMETER_HPP

#include <string>
#include <ostream>
#include <unordered_map>
#include <algorithm>

#include <gcc-plugin.h>
#include <intl.h>           // G_()
#include <cp-tree.h>
#include <tree.h>

#include "common/iosig_common_parameter.hpp"

class parameter_utils {

private:

    static std::unordered_map <std::string, parameter_type> parameter_list;
    static std::unordered_map <std::string, long> units;

    static std::unordered_map <std::string, parameter_type> generate_parameters ();
    static std::unordered_map <std::string, long> generate_units ();

    parameter_utils () = delete;
    
public:
    static parameter_type parameter_enum (const std::string&);
    static std::string parameter_string (parameter_type);
    static long unit_value (const std::string&);
    static bool parameter_allowed_value_type (parameter_type param, enum cpp_ttype val_token);

    template <typename T>
    static T get_tree_value (const tree&);

};

class parameter {
public:
    parameter_type  param_type;
    tree            data;
    long            unit;

    friend std::ostream &operator << (std::ostream &os, const parameter &param);
};


#endif
