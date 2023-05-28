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


 #ifndef IOSIG_REGISTERED_IO_HPP
#define IOSIG_REGISTERED_IO_HPP

#include "iosig_pragma.hpp"

#include <tree.h>
#include <gimple.h>
#include <langhooks.h>



class registered_io {

private:

    class io_function {
    public:
        io_function (std::string fullname, int path_index):
            function_full_name {fullname}, path_arg_index {path_index} {
        }
        const std::string function_full_name;
        const int path_arg_index;
    };

    static std::vector <io_function> supported_io_functions;
    static std::vector <io_function> fill_supported_io_functions ();

    static tree redirect_declaration;
    static tree create_redirect_declaration ();

    const gimple *call;
    std::string io_function_name;
    const std::unique_ptr <const pragma_sig> pragma;

    tree buffer_decl;
    size_t buffer_size;

    tree ref_array_at_index (const tree &, int) const;
    tree get_original_path_variable () const;

public:

    registered_io (std::unique_ptr <const pragma_sig> pr, const gimple *call);

    std::vector<gimple *> generate_buffer_init ();  // based on the filled pragma
    gimple *generate_redirect_call () const;        // creates a call to the generic function
    
    static bool is_io_function (const std::string&);
    static std::string get_function_full_name (const gimple *);

};

#endif
