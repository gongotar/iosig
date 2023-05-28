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


 #include "iosig_parameter.hpp"

template <>
long parameter_utils::get_tree_value (const tree &t) {
    if (t == NULL_TREE) {
        return 1;
    }
    gcc_assert (TREE_CODE (t) == INTEGER_CST);
    return wi::to_wide (t).to_shwi ();
}

template <>
std::string parameter_utils::get_tree_value (const tree &t) {
    if (t == NULL_TREE) {
        return "1";
    }
    switch (TREE_CODE (t)) {
        case STRING_CST:
            return std::string (TREE_STRING_POINTER (t));
        case INTEGER_CST:
            return std::to_string (parameter_utils::get_tree_value <long> (t));
        case IDENTIFIER_NODE:
            return std::string (IDENTIFIER_POINTER (t));
        case VAR_DECL:
        case PARM_DECL:
            return std::string (IDENTIFIER_POINTER (DECL_NAME (t)));
        default:
            warning (0, G_("iosig_parameter: invalid tokens in IOSIG pragma"));
            return std::string ();
    }
}

std::unordered_map <std::string, long> parameter_utils::generate_units() {

    std::unordered_map <std::string, long> ulist;

    // time units
    ulist["ms"] = 1l;
    ulist["s"]  = 1000l * units["ms"];
    ulist["m"]  = 60l * units["s"];
    ulist["h"]  = 60l * units["m"];
    ulist["d"]  = 24l * units["h"];
    ulist["w"]  = 7l * units["d"];
    ulist["M"]  = 30l * units["d"];
    ulist["y"]  = 12l * units["M"];

    // size units
    ulist["b"]  = 1l;
    ulist["Kb"] = 1024l * units["b"];
    ulist["Mb"] = 1024l * units["Kb"];
    ulist["Gb"] = 1024l * units["Mb"];
    ulist["Tb"] = 1024l * units["Gb"];
    ulist["B"]  = 8l * units["b"];
    ulist["KB"] = 1024l * units["B"];
    ulist["MB"] = 1024l * units["KB"];
    ulist["GB"] = 1024l * units["MB"];
    ulist["TB"] = 1024l * units["GB"];

    // data transfer units
    ulist["bs"]     = 1l;
    ulist["Kbs"]    = 1024l * units["bs"];
    ulist["Mbs"]    = 1024l * units["Kbs"];
    ulist["Gbs"]    = 1024l * units["Mbs"];
    ulist["Tbs"]    = 1024l * units["Gbs"];
    ulist["Bs"]     = 8l * units["bs"];
    ulist["KBs"]    = 1024l * units["Bs"];
    ulist["MBs"]    = 1024l * units["KBs"];
    ulist["GBs"]    = 1024l * units["MBs"];
    ulist["TBs"]    = 1024l * units["GBs"];

    return ulist;

}

std::unordered_map <std::string, parameter_type> parameter_utils::generate_parameters () {

    std::unordered_map <std::string, parameter_type> plist;
    plist["name"]               = parameter_type::name;
    plist["size_per_io"]        = parameter_type::size_io;
    plist["totalsize"]          = parameter_type::totalsize;
    plist["min_mttdl"]          = parameter_type::min_mttdl;
    plist["global"]             = parameter_type::global;
    plist["local"]              = parameter_type::local;
    plist["tmp"]                = parameter_type::tmp;
    plist["persist"]            = parameter_type::persist;
    plist["sequential_access"]  = parameter_type::seq_acc;
    plist["random_access"]      = parameter_type::rnd_acc;
    plist["tamper"]             = parameter_type::tamper;
    plist["archive"]            = parameter_type::archive;

    return plist;
}

bool parameter_utils::parameter_allowed_value_type (parameter_type param, enum cpp_ttype val_token) {
    switch (param) {
        case parameter_type::totalsize:
        case parameter_type::min_mttdl:
        case parameter_type::size_io:
            return (val_token == CPP_NUMBER || val_token == CPP_NAME);
        case parameter_type::name:
            return (val_token == CPP_STRING || val_token == CPP_NAME);
        case parameter_type::global:
        case parameter_type::local:
        case parameter_type::tmp:
        case parameter_type::persist:
        case parameter_type::seq_acc:
        case parameter_type::rnd_acc:
        case parameter_type::tamper:
        case parameter_type::archive:
            return (val_token == CPP_NUMBER || val_token == CPP_NAME || val_token == CPP_EOF);
        default:
            warning (0, G_("iosig_parameter: Unknown parameter!"));
    }
    return false;
}

parameter_type parameter_utils::parameter_enum (const std::string& key) {
    return parameter_list.at (key);
}

long parameter_utils::unit_value (const std::string& key) {
    return units.at (key);
}

std::string parameter_utils::parameter_string (parameter_type param_type) {
    const auto &pl = parameter_list;    
    auto find_param_str = std::find_if (pl.begin (), pl.end (),
            [&param_type] (auto&& pair) {return pair.second == param_type;});
    gcc_assert (find_param_str != pl.end ());
    return find_param_str->first;
}

std::unordered_map <std::string, parameter_type> 
parameter_utils::parameter_list = parameter_utils::generate_parameters ();

std::unordered_map <std::string, long>
parameter_utils::units = parameter_utils::generate_units ();

std::ostream &operator << (std::ostream &os, const parameter &param) {
    std::string tree_str = std::string ();

    os  << parameter_utils::parameter_string (param.param_type) 
        << "(value: "
        << parameter_utils::get_tree_value <std::string> (param.data)
        << ", type: "
        << ((param.data == NULL_TREE) ? 
                "null_tree" : 
                std::string (get_tree_code_name (TREE_CODE (param.data))))
        << ", unit: "
        << param.unit
        << ")";

    return os;
}
