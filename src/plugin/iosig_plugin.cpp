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
 * IOSIG plugin - GCC version 9.31
 */

#include "iosig_plugin.hpp"

/* this must be here so the plugin gets compiled */
int plugin_is_GPL_compatible;

/* 
 * io sig pragma handler: remember the location of the pragmas 
 * and also note the given parameters for each pragma.
 * If the pragma is of a predefined signiture, use 
 * the given signiture name to desctibe IO
 * */
void pragma_handler_io_sig (struct cpp_reader *pfile) {
    
    std::unique_ptr <pragma_sig> pragma = std::make_unique <pragma_sig> ();
    pragma->parse ();
    std::cout << *pragma << std::endl;
    pragma_sig::insert_pragma_to_list (std::move (pragma));

}

/*
 * io sigdef pragma handler: remember the defined parameters 
 * for a given sig name
 */
void pragma_handler_io_sigdef (struct cpp_reader *pfile) {
    std::unique_ptr <pragma_sigdef> pragma = std::make_unique <pragma_sigdef> ();
    pragma->parse ();
    std::cout << *pragma << std::endl;
    pragma_sigdef::insert_pragma_to_list (std::move (pragma));
}

/*
 * io jobwide pragma handler: remember the jobwide parameters 
 * that describe the global behaviour of the job's IO 
 */
void pragma_handler_io_jobwide (struct cpp_reader *pfile) {
    std::unique_ptr <pragma_jobwide> pragma = std::make_unique <pragma_jobwide> ();
    pragma->parse ();
    std::cout << *pragma << std::endl;
}

/* register different IOSIG pragmas */
static void register_iosig_pragma (void *event_data, void *data) {
    // c_register_pragma (NULL, "io", pragma_handler_iosig);
    c_register_pragma ("io", "sigdef", pragma_handler_io_sigdef);
    c_register_pragma ("io", "sig", pragma_handler_io_sig);
    c_register_pragma ("io", "jobwide", pragma_handler_io_jobwide);
}

static void final_checks (void *event_data, void *data) {
    if (pragma_sig::number_of_pragmas_in_list () > 0) {
        std::cerr << "iosig_plugin: Found "
            << pragma_sig::number_of_pragmas_in_list ()
            << " pragma notion(s) above non-supported IO function calls!" 
            << std::endl;
    }
}

/* the main function of the plugin, this is the first function executed */
int plugin_init (struct plugin_name_args *plugin_info, struct plugin_gcc_version *version) {

    /* check the GCC version */
    if (!plugin_default_version_check (version, &gcc_version)) {
        printf("not matching versions\n");    
        return 1;
    }

    
    /* redirect io pass: create a file and call symlink statement at the correct location */
    const pass_data &passd = {GIMPLE_PASS, "redirect_io_pass"};
    redirect_io_pass *r_io_pass = new redirect_io_pass (passd, g);
	struct register_pass_info riopass;
	riopass.pass = r_io_pass;
	riopass.reference_pass_name = "ssa";
	riopass.ref_pass_instance_number = 1;
	riopass.pos_op = PASS_POS_INSERT_BEFORE;
    
    /* test gimple pass: to see which gimple statements are created */
    const pass_data &tgpassd = {GIMPLE_PASS, "test_gimple_pass"};
    test_gimple_pass *t_g_pass = new test_gimple_pass (tgpassd, g);
	struct register_pass_info tgpass;
	tgpass.pass = t_g_pass;
	tgpass.reference_pass_name = "redirect_io_pass";
	tgpass.ref_pass_instance_number = 1;
	tgpass.pos_op = PASS_POS_INSERT_AFTER;

    /* register passes */
    register_callback(plugin_info->base_name, PLUGIN_PRAGMAS, register_iosig_pragma, NULL);
    register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &riopass);
    register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &tgpass);
    register_callback(plugin_info->base_name, PLUGIN_FINISH, final_checks, NULL);

    return 0;
}

