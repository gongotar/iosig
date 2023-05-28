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


 /*
 * IOSIG gimple manipulation code: inject, remove and 
 * modify program's source code
 */

#include "iosig_passes.hpp"

/* traverse all statements of the function and print them */
unsigned int test_gimple_pass::execute(function *func) {
    // loop over statements of the given function
    basic_block bb;
    FOR_EACH_BB_FN (bb, func) {
	    for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi)) {

		    gimple *stmt = gsi_stmt (gsi);
//            printf ("=====================================\n");
            print_gimple_stmt(stdout, stmt, 0, TDF_SLIM);
//            for (unsigned int i = 0; i < gimple_num_ops (stmt); ++i) {
//                tree op = gimple_op (stmt, i);
//                debug_tree (op);
//            }

        }
    }
    return 0;
}

/* traverse all statements and fetch those fopen calls that are marked by a pragma */
unsigned int redirect_io_pass::execute(function *func) {
    // loop over statments of the given function
    basic_block bb;
    FOR_EACH_BB_FN (bb, func) {
	    for (gimple_stmt_iterator gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi)) {
		    gimple *stmt = gsi_stmt (gsi);
            if (gimple_code (stmt) == GIMPLE_CALL) {    // if this statement is a function call
                std::string func_full_name = registered_io::get_function_full_name (stmt);
                if (registered_io::is_io_function (func_full_name)) { // if IO call
                    location loc (gimple_location (stmt));
                    std::unique_ptr <const pragma_sig> pragma = 
                        pragma_sig::extract_pragma_from_list (loc.previous_line ());
                    if (pragma) {                       // if registered by a pragma

                        registered_io regio {std::move (pragma), stmt};
                        std::vector <gimple *> init_buffer_stmts = regio.generate_buffer_init ();
                        gimple *redirect_call = regio.generate_redirect_call ();
                        bool first_insert = true;

                        // insert buffer init statements
                        for (gimple *init_buffer_stmt: init_buffer_stmts) {
                            if (first_insert) {         // first insertion goes before the call
                                gsi_insert_before (&gsi, init_buffer_stmt, GSI_NEW_STMT);
                                first_insert = false;
                            }
                            else {
                                gsi_insert_after (&gsi, init_buffer_stmt, GSI_NEW_STMT);
                            }
                        }

                        // insert generic call
                        if (first_insert) {
                            gsi_insert_before (&gsi, redirect_call, GSI_NEW_STMT);
                            first_insert = false;
                        }
                        else {
                            gsi_insert_after (&gsi, redirect_call, GSI_NEW_STMT);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
