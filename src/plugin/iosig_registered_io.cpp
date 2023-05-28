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


 #include "iosig_registered_io.hpp"


registered_io::registered_io (std::unique_ptr <const pragma_sig> pr, const gimple *call): 
    call {call}, pragma {std::move (pr)} {
    
    io_function_name = get_function_full_name (call);

    // some assertions to make sure the correct pragma and call are 
    // gathered into this object

    // the location of the call must be one line after the pragma notion
    location loc {gimple_location (call)};
    gcc_assert ((loc.previous_line () == *(pragma->pragma_loc)));

    // the registered call must be an IO stream initializer function call
    gcc_assert (is_io_function (io_function_name));
}

std::vector <gimple *> registered_io::generate_buffer_init () {

    std::vector <gimple *> statements;

    // declare and define a long [22] buffer.
    // Before each parameter there is an indicator to 
    // indicate wether the value is set on the pragma or not.
    // If the value is set, then the indicator is 1, otherwise, 0

    buffer_size = 2 * pragma->get_signature ().size ();
    if (pragma->has_param (parameter_type::name)) {     // we don't transfer name
        buffer_size -= 2;
    }

    tree long_array_type_node = build_array_type_nelts (long_integer_type_node, buffer_size);

    buffer_decl = create_tmp_var (long_array_type_node, "_iosig_sig_buffer_");

    int index = 0;
    tree lhs;
    for (auto &param: pragma->get_signature ()) {
        if (param->param_type == parameter_type::name) {
            continue;
        }
        // The first index will the enum integer of the parameter type 
        // indicating the parameter corresponding to the value
        long ptype = static_cast <long> (param->param_type);

        lhs = ref_array_at_index (buffer_decl, index ++);
        tree param_integer_type_node = build_int_cst (long_integer_type_node, ptype);
        gimple *set_param_type = 
            gimple_build_assign (lhs, param_integer_type_node);

        lhs = ref_array_at_index (buffer_decl, index ++);

        // The second index will be the data of the parameter type 
        // of the previous index
        
        gimple *set_param_val;
        if (param->unit > 1) {
            gcc_assert (param->data != NULL_TREE);
            tree unit_tree = build_int_cst (long_integer_type_node, param->unit);
            tree tmp_var = create_tmp_var (long_integer_type_node, "_iosig_tmp_long_");
            gimple *tmp_stmt = gimple_build_assign (tmp_var, MULT_EXPR, param->data, unit_tree);
            statements.push_back (tmp_stmt);
            set_param_val = gimple_build_assign (lhs, tmp_var);
        }
        else if (param->data == NULL_TREE) {
            gcc_assert (param->unit == 1);
            set_param_val = gimple_build_assign (lhs, integer_one_node);
        }
        else {
            gcc_assert (param->unit == 1);
            set_param_val = gimple_build_assign (lhs, param->data);
        }

        statements.push_back (set_param_type);
        statements.push_back (set_param_val);

    }

    // memset the buffer to zero.
    /*
    tree memset_decl_type = build_function_type_list(ptr_type_node, ptr_type_node,
            integer_type_node, size_type_node, NULL_TREE);
    tree memset_decl = build_fn_decl ("memset", memset_decl_type);
    tree integer_buffer_size_node = 
        build_int_cst (long_integer_type_node, buffer_size*sizeof(long)); 
    tree addr_buffer = build_nt (ADDR_EXPR, buffer_decl);
    TREE_TYPE (addr_buffer) = build_pointer_type (long_array_type_node);
    gimple *memset_call = gimple_build_call (memset_decl, 3, 
            addr_buffer,
            integer_zero_node,
            integer_buffer_size_node);
    statements.push_back (memset_call);
    */
/*    
    for (auto &param: pragma->get_signature ()) {
        switch (param->ttype) {
            case CPP_NAME:
            {
                parameter <std::string> &p = static_cast <parameter <std::string>&> (*param); 
                int index = static_cast <int> (param->param_type);
                // set the indicator to 1, indicating the parameter is
                // being set in the pragma
                statements.push_back (assign_to_array_at_index (
                            buffer_decl, 
                            index - 1,
                            integer_one_node));

                // fill the buffer with the parameter at the corresponding index

//                tree parameter_variable = build1 (VAR_DECL, 
//                        long_integer_type_node,
//                        get_identifier (p.data.c_str ())
//                        );
//                tree parameter_variable = create_tmp_var (long_integer_type_node, 
//                        p.data.c_str ());

//                tree parameter_variable = make_node (VAR_DECL);
//                DECL_NAME (parameter_variable) = get_identifier (p.data.c_str ());
//                TREE_TYPE (parameter_variable) = long_integer_type_node;


                statements.push_back (assign_to_array_at_index (
                            buffer_decl, 
                            index,
                            parameter_variable));
                break;
            }
            case CPP_NUMBER:
            {
                parameter <long> &p = static_cast <parameter <long>&> (*param); 
                int index = static_cast <int> (param->param_type);

                // set the indicator to 1, indicating the parameter is
                // being set in the pragma
                statements.push_back (assign_to_array_at_index (
                            buffer_decl, 
                            index - 1,
                            integer_one_node));

                // fill the buffer with the parameter at the corresponding index
                tree integer_parameter_value_type =
                    build_int_cst (long_integer_type_node, p.data * p.unit);
                statements.push_back (assign_to_array_at_index (
                            buffer_decl, 
                            index,
                            integer_parameter_value_type));
                break;
            }
            case CPP_STRING:
                // nothing to do
                gcc_assert (param->param_type == parameter_type::name);
                break;
            default:
                warning (0, G_("iosig_registered_io: Unknown parameter value type!"));
        }
    }
    */
    return statements;
}

gimple *registered_io::generate_redirect_call () const {
    
    if (redirect_declaration == NULL_TREE) {
        redirect_declaration = registered_io::create_redirect_declaration();
    }

    tree addr_buffer = build_nt (ADDR_EXPR, buffer_decl);
    TREE_TYPE (addr_buffer) = build_pointer_type (TREE_TYPE (buffer_decl));

    gimple *redirect_call = gimple_build_call (redirect_declaration, 3, 
            get_original_path_variable (),
            addr_buffer,
            build_int_cst (size_type_node, buffer_size));
    return redirect_call;
}

tree registered_io::ref_array_at_index (const tree &array, int index) const {
    tree integer_index_type = build_int_cst (integer_type_node, index);
    tree lhs = build4 (ARRAY_REF, long_integer_type_node, array, integer_index_type, 
            NULL_TREE, NULL_TREE);
    return lhs;
}

tree registered_io::get_original_path_variable () const {
    int path_index = -1;
    for (auto &func: supported_io_functions) {
        if (func.function_full_name == io_function_name) {
            path_index = func.path_arg_index;
        }
    }

    gcc_assert (path_index >= 0);

    return gimple_call_arg(call, path_index);
}


bool registered_io::is_io_function (const std::string &function_name) {
    for (auto &func: supported_io_functions) {
        if (func.function_full_name == function_name) {
            return true;
        }
    }
    return false;
}

// static members

std::string registered_io::get_function_full_name (const gimple *call) {

    tree fnt = gimple_call_fn (call);

    // We always know the open file stream function calls are 
    // the only operand of the statements right hand side. This means, 
    // it makes no sense to have for instance the following statement:
    // int fd = open (filename, mode) + 4
    // For this reason, we fetch the  first operand of the rhs
    tree first_op = TREE_OPERAND (fnt, 0); 

    // Here the second input, "1" indicates the degree of the details we want to 
    // see in the output about the function
    std::string func_full_name = lang_hooks.decl_printable_name (first_op, 1);

    return func_full_name;
}



tree registered_io::create_redirect_declaration() {
    tree redirect_decl_type = build_function_type_list (integer_type_node, 
            const_string_type_node,
            build_pointer_type (long_integer_type_node),
            size_type_node, 
            NULL_TREE);

    tree redirect_decl =  build_fn_decl ("_iosig_fn_redirect_inject_", redirect_decl_type);
    pushdecl (redirect_decl);
    return redirect_decl;
}

tree registered_io::redirect_declaration = NULL_TREE;


std::vector <registered_io::io_function> 
registered_io::fill_supported_io_functions () {

    std::vector <io_function> functions;

    functions.emplace_back ("std::basic_fstream<char>::open", 1);
    functions.emplace_back ("open", 0);
    functions.emplace_back ("fopen", 0);

    return functions;
}

std::vector<registered_io::io_function> registered_io::supported_io_functions = 
    registered_io::fill_supported_io_functions ();
