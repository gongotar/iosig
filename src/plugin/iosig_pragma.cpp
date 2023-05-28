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


 #include "iosig_pragma.hpp"    


template <typename T>
std::pair<bool, T> pragma_base::get_param_val (parameter_type param_type) const {
    for (auto &param: signature) {
        if (param->param_type == param_type) {
            return std::make_pair (true, parameter_utils::get_tree_value <T> (param->data));
        }
    }
    return std::make_pair (false, T ());
}
    
bool pragma_base::has_param (parameter_type param_type) const {
    for (auto &param: signature) {
        if (param->param_type == param_type) {
            return true;
        }
    }
    return false;
}

std::unique_ptr <const parameter> pragma_base::copy_param (parameter_type param_type) const {
    for (auto &param: signature) {
        if (param->param_type == param_type) {
            std::unique_ptr <const parameter> param_copy = 
                std::make_unique <const parameter> (*param);
            return param_copy;
        }
    }
    return nullptr;
}

long pragma_base::parse_unit (tree &t) {

    long unit_val = 1;                      // default unit
    enum cpp_ttype token = pragma_lex (&t);
    if (token == CPP_MULT) {                // optional unit, match "*"
        // fill parameter unit
        token = pragma_lex (&t);
        gcc_assert (token == CPP_NAME);     // unit
        std::string unit {IDENTIFIER_POINTER(t)};
        unit_val = parameter_utils::unit_value (unit); 
        token = pragma_lex (&t);
    }

    gcc_assert (token == CPP_CLOSE_PAREN);

    return unit_val;
}

void pragma_base::parse () {
    
    // get the pragma location
    location_t loc;
    tree t;
    enum cpp_ttype token = pragma_lex (&t, &loc);
    pragma_loc = std::make_unique <location> (loc);
    
    // repeat until there are no parameters left for the current pragma
    while (token != CPP_EOF) {
        gcc_assert (token == CPP_NAME);

        // fill parameter param_type
        std::string key {IDENTIFIER_POINTER(t)};        // string representation of the parameter
        parameter_type param_type = parameter_utils::parameter_enum (key);

        token = pragma_lex (&t);

        std::unique_ptr <parameter> param = std::make_unique <parameter> ();
        param->param_type = param_type;

        switch (token) {
            case CPP_EOF:
            case CPP_NAME:
            {
                if (!parameter_utils::parameter_allowed_value_type (param_type, CPP_EOF)) {
                    warning (0, G_("iosig_pragma: Wrong value type for the parameter"));
                }
                param->data = NULL_TREE;
                param->unit = 1;
                break;
            }
            case CPP_OPEN_PAREN:

                token = pragma_lex (&t);
                if (!parameter_utils::parameter_allowed_value_type (param_type, token)) {
                    warning (0, G_("iosig_pragma: Wrong value type for the parameter"));
                }

                switch (token) {
                    case CPP_NUMBER:
                        gcc_assert (TREE_CODE (t) == INTEGER_CST);
                    case CPP_STRING:
                        param->data = t;
                        break;
                    case CPP_NAME:
                    {
                        tree var = lookup_name (t);
                        if (var == NULL_TREE) {
                            warning (0, G_ ("iosig_pragma: Variable not found!"));
                        }
                        param->data = var;
                        break;
                    }
                    default:
                        warning (0, G_("iosig_pragma: Unknown pragma token!"));
                }

                param->unit = parse_unit (t);
                token = pragma_lex (&t);

                break;
            default:
                warning (0, G_("iosig_pragma: Unknown pragma token!"));
        }
        signature.push_back (std::move (param));
    }

    check_parameters ();
    
}

void pragma_base::check_parameters () const {

/*  TODO: compile time check parameters vs runtime check 
    auto name_val       = get_param_val <std::string> (parameter_type::name);
    auto global_val     = get_param_val <long> (parameter_type::global);
    auto local_val      = get_param_val <long> (parameter_type::local);
    auto rnd_val        = get_param_val <long> (parameter_type::rnd_acc);
    auto seq_val        = get_param_val <long> (parameter_type::seq_acc);
    auto tmp_val        = get_param_val <long> (parameter_type::tmp);
    auto persist_val    = get_param_val <long> (parameter_type::persist);
    auto tamper_val     = get_param_val <long> (parameter_type::tamper);
    auto archive_val    = get_param_val <long> (parameter_type::archive);
    auto size_io_val    = get_param_val <long> (parameter_type::size_io);
    auto totalsize_val  = get_param_val <long> (parameter_type::totalsize);
    auto min_mttdl_val  = get_param_val <long> (parameter_type::min_mttdl);

    // io sigdef type must contain a name
    gcc_assert (!(type == pragma_type::IO_SIGDEF && !name_val.first));
    
    // opposite parameters are not allowed to be set together
    if (global_val.first && local_val.first) {
        gcc_assert (!(global_val.second && local_val.second));
    }
    
    if (rnd_val.first && seq_val.first) {
        gcc_assert (!(rnd_val.second && seq_val.second));
    }

    if (tmp_val.first && persist_val.first) {
        gcc_assert (!(tmp_val.second && persist_val.second));
    }


    // persisted local data on nodes cannot be tamperproof
    if (local_val.first && persist_val.first && tamper_val.first) {
        gcc_assert (!(tamper_val.second && local_val.second && persist_val.second));
    }

    // archive data cannot be local or temporary
    if (archive_val.first && local_val.first) {
        gcc_assert (!(archive_val.second && local_val.second));
    }

    if (archive_val.first && tmp_val.first) {
        gcc_assert (!(archive_val.second && local_val.second));
    }

    // numerical assumptions for MTTDL, size per IO, and totalsize: all greater than zero
    if (size_io_val.first) {
        gcc_assert (size_io_val.second > 0);
    }
    
    if (totalsize_val.first) {
        gcc_assert (totalsize_val.second > 0);
    }
    
    if (min_mttdl_val.first) {
        gcc_assert (min_mttdl_val.second > 0);
    }
*/
}

void pragma_sigdef::parse () {
    pragma_base::parse ();
    auto found_name = get_param_val <std::string> (parameter_type::name);
    if (!found_name.first) {
        warning (0, G_("iosig_pragma: Pragma sigdef must have a name!"));
    }
    else {
        sig_name = found_name.second;
    }


}
void pragma_sig::parse () {
    pragma_base::parse ();
    auto found_name = get_param_val <std::string> (parameter_type::name);
    if (found_name.first) {
        try {
            const pragma_sigdef &sigdef = pragma_sigdef::get_pragma_from_list (found_name.second);
            inherit (sigdef);
            check_parameters ();
        } catch (const std::out_of_range &e) {
            warning (0, G_("iosig_pragma: Signiture name not found!"));
        }
    }
}


void pragma_sig::inherit (const pragma_sigdef &sigdef) {
    
    // first load the required parameters
    auto size_io_sigdef     = sigdef.copy_param (parameter_type::size_io);
    auto totalsize_sigdef   = sigdef.copy_param (parameter_type::totalsize);
    auto min_mttdl_sigdef   = sigdef.copy_param (parameter_type::min_mttdl);
    auto global_sigdef      = sigdef.copy_param (parameter_type::global);
    auto local_sigdef       = sigdef.copy_param (parameter_type::local);
    auto rnd_sigdef         = sigdef.copy_param (parameter_type::rnd_acc);
    auto seq_sigdef         = sigdef.copy_param (parameter_type::seq_acc);
    auto tmp_sigdef         = sigdef.copy_param (parameter_type::tmp);
    auto persist_sigdef     = sigdef.copy_param (parameter_type::persist);
    auto tamper_sigdef      = sigdef.copy_param (parameter_type::tamper);
    auto archive_sigdef     = sigdef.copy_param (parameter_type::archive);

    bool size_io_sig_set    = has_param (parameter_type::size_io);
    bool totalsize_sig_set  = has_param (parameter_type::totalsize);
    bool min_mttdl_sig_set  = has_param (parameter_type::min_mttdl);
    bool global_sig_set     = has_param (parameter_type::global);
    bool local_sig_set      = has_param (parameter_type::local);
    bool rnd_sig_set        = has_param (parameter_type::rnd_acc);
    bool seq_sig_set        = has_param (parameter_type::seq_acc);
    bool tmp_sig_set        = has_param (parameter_type::tmp);
    bool persist_sig_set    = has_param (parameter_type::persist);
    bool archive_sig_set    = has_param (parameter_type::archive);
    bool tamper_sig_set     = has_param (parameter_type::tamper);

    // check the conditions and inherit from the  sigdef
    if (size_io_sigdef && !size_io_sig_set) {
        signature.push_back (std::move (size_io_sigdef));
    }
    
    if (totalsize_sigdef && !totalsize_sig_set) {
        signature.push_back (std::move (totalsize_sigdef));
    }
    
    if (min_mttdl_sigdef && !min_mttdl_sig_set) {
        signature.push_back (std::move (min_mttdl_sigdef));
    }
    
    if (global_sigdef && !global_sig_set && !local_sig_set) {
        signature.push_back (std::move (global_sigdef));
    }
    else if (local_sigdef && !local_sig_set && !global_sig_set) {
        signature.push_back (std::move (local_sigdef));
    }
    
    if (rnd_sigdef && !rnd_sig_set && !seq_sig_set) {
        signature.push_back (std::move (rnd_sigdef));
    }
    else if (seq_sigdef && !seq_sig_set && !rnd_sig_set) {
        signature.push_back (std::move (seq_sigdef));
    }
    
    if (tmp_sigdef && !tmp_sig_set && !persist_sig_set && !archive_sig_set) {
        signature.push_back (std::move (tmp_sigdef));
    }
    
    if (persist_sigdef && !persist_sig_set && !tmp_sig_set && 
            !(tamper_sig_set && local_sig_set)) {
        signature.push_back (std::move (persist_sigdef));
    }

    if (tamper_sigdef && tamper_sig_set && 
            !(local_sig_set && persist_sig_set)) {
        signature.push_back (std::move (tamper_sigdef));
    }

    if (archive_sigdef && archive_sig_set && !local_sig_set && !tmp_sig_set) {
        signature.push_back (std::move (archive_sigdef));
    }

}

std::ostream &operator << (std::ostream &os, const pragma_base &pragma) {
    switch (pragma.type) {
        case pragma.pragma_type::IO_SIG:        os << "io sig: "; break;
        case pragma.pragma_type::IO_SIGDEF:     os << "io sigdef: "; break;
        case pragma.pragma_type::IO_JOBWIDE:    os << "io jobwide: "; break;
    }
    for (auto &param: pragma.signature) {
        os << *param << "; ";
    }
    return os;
}

std::unique_ptr <const pragma_sig> pragma_sig::extract_pragma_from_list (const location &loc) {
    try {
        // for now, we are limitted to c++14, in case of GCC updates, we can use extract
        std::unique_ptr <const pragma_sig> pragma = std::move (pragma_sig_list.at (loc));
        pragma_sig_list.erase (loc);
        return pragma;
    } catch (const std::out_of_range &e) {
        return nullptr;
    }
}

std::unordered_map <location, std::unique_ptr <const pragma_sig>> pragma_sig::pragma_sig_list;
std::unordered_map <std::string, std::unique_ptr <const pragma_sigdef>> pragma_sigdef::pragma_sigdef_list;

