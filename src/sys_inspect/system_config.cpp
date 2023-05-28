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

#include "system_config.hpp"

void iosig::sys_inspect::system_config::parse () {
    if (!file.is_open ()) {
       throw common::exceptions::config_file_failure ("Could not open the file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf ();
    std::string conf {buffer.str ()};

    std::regex system_object_reg 
        {R"(([^\s\{\}"]+)(\s+")([^\{\}"]+)("\s*\{\s*)([^\{\}]*)(\s*\}))"};
    std::regex obj_properties_pair_reg {R"(([^\s"]+)(\s*=\s*")([^\n"]+)("))"};
    std::regex obj_properties_arr_reg {R"(([^\s"]+)(\s*=\s*\[)([^\]]*\]))"};
    std::regex arr_reg {R"((")([^\n"]+)("\s*(,|\])))"};


    std::sregex_iterator start_sys_obj {conf.begin(), conf.end(), system_object_reg};
    std::sregex_iterator end_sys_obj;
    
    for (std::sregex_iterator sys_obj = start_sys_obj; sys_obj != end_sys_obj; sys_obj++) {

        // parse the type (key) and name of the objects
        std::smatch match = *sys_obj;

        conf_data.emplace_back (match[1], match[3]);

        std::string body = match[5];
 
        // parse the body of properties
        // first, the pairs of key and string values
        std::sregex_iterator start_obj_prop {body.begin(), body.end(), obj_properties_pair_reg};
        std::sregex_iterator end_obj_prop;

        for (auto obj_prop = start_obj_prop; obj_prop != end_obj_prop; obj_prop++) {
            std::smatch prop_match = *obj_prop;
            conf_data.back ().paired_values.emplace_back (prop_match[1], prop_match[3]);
        }

        // second, the pairs of key and array of values
        std::sregex_iterator start_obj_arr_prop
                {body.begin(), body.end(), obj_properties_arr_reg}; 
        std::sregex_iterator end_obj_arr_prop;

        for (auto obj_prop = start_obj_arr_prop; obj_prop != end_obj_arr_prop; obj_prop++) {
            std::smatch prop_match = *obj_prop;

            std::string prop_key = prop_match[1];
            std::string prop_arr = prop_match[3];

            std::sregex_iterator start_arr {prop_arr.begin(), prop_arr.end(), arr_reg};
            std::sregex_iterator end_arr;

            conf_data.back ().list_values.emplace_back (prop_key, std::list <std::string> ());
            for (auto arr_val = start_arr; arr_val != end_arr; arr_val++) {
                std::smatch val_match = *arr_val;
                conf_data.back ().list_values.back ().second.emplace_back (val_match[2]);

            }

        }

    }
}

std::string iosig::sys_inspect::system_config::search_for_default_config_file () {
    // TODO implement, executing the command "find config.iosig" on bash
    return "config.iosig";
}

/*
bool system_config::store_system_model (const system_model &sys) {


    // write all filesystems first
    for (const auto &mp: sys.mountpoints) {

        std::string tk_fs = get_token_label (config_tokens::filesystem);
        std::string tk_bd = get_token_label (config_tokens::blockdevice);
        std::string tk_max_fsz = get_token_label (config_tokens::max_filesize);
        std::string tk_seg_sz = get_token_label (config_tokens::segment_size);
        std::string tk_err_fix = get_token_label (config_tokens::err_fix_per_segment);

        file << tk_fs << " " << mp.fs->name << " {\n"

             << std::setw (tab_width) << ""
             << tk_bd << std::setw (value_offset - tk_bd.size ()) << std::right << "= "
             << mp.fs->name << "\n"

             << std::setw (tab_width) << ""
             << tk_max_fsz << std::setw (value_offset - tk_max_fsz.size ()) << std::right << "= "
             << mp.fs->max_filesize << "\n"

             << std::setw (tab_width) << ""
             << tk_seg_sz << std::setw (value_offset - tk_seg_sz.size ()) << std::right << "= "
             << mp.fs->segment_size << "\n"

             << std::setw (tab_width) << ""
             << tk_err_fix << std::setw (value_offset - tk_err_fix.size ()) << std::right << "= "
             << mp.fs->err_fix_per_segment

             << std::endl;
    }
    return true;
}


std::shared_ptr <system_model> system_config::load_system_model () {

    std::string line;
    while (std::getline (file, line)) {

        std::stringstream line_stream {line};
        std::string token;
        while (line_stream >> token) {
            // ignore comments (for now, a line that starts with comment will be ignored)
            consume_comment (file, line_stream, token);


        }
    }
    return std::shared_ptr <system_model> ();
}


void system_config::consume_comment (std::fstream &file, std::stringstream &line_stream, std::string &token) {
    if (!comment_start (token)) {
        return;
    } else if (comment_start_line (token)) {
    } else if (comment_start_block (token)) {
        do {
            if (line_stream)
                line_stream >> token;
            else
                file >> token;
        } while (!comment_end_block (token));

    }

    file.ignore (std::numeric_limits <std::streamsize>::max (), '\n');
    std::string line;
    token.clear ();
    line_stream.clear ();

    std::getline (file, line);
    line_stream.str (line);
    line_stream >> token;


    if (comment_start (token)) {
        consume_comment (file, line_stream, token);
    }
}



bool system_config::comment_start_line (const std::string &token) {
    if (token.size () < 2)
        return token.substr (0, 1) == "#";
    return (token.substr (0, 2) == "//");
}

bool system_config::comment_start (const std::string &token) {
    return comment_start_block (token) || comment_start_line (token);
}

std::string system_config::get_token_label (system_config::config_tokens token) {
    switch (token) {
        case config_tokens::blockdevice:
            return "blockdevice";
        case config_tokens::filesystem:
            return "filesystem";
    }
    return std::string ();
}
*/
