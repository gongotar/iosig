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

#ifndef IOSIG_PASSES_HPP 
#define IOSIG_PASSES_HPP

#include <gcc-plugin.h>
#include <tree.h>
#include <tree-pass.h>
#include <context.h>        // g
#include <gimple.h>
#include <gimple-iterator.h>
#include <gimple-pretty-print.h>
#include <basic-block.h>
#include <print-tree.h>

#include "iosig_registered_io.hpp"
#include "iosig_pragma.hpp"


/* a pass that injects some istructions to redirect IO*/
class redirect_io_pass : public gimple_opt_pass {
    public:
        redirect_io_pass(const pass_data& data, gcc::context *ctxt)
            : gimple_opt_pass(data, ctxt)
        {}

        unsigned int execute(function *);
};

/* a pass that prints all instructions of the program, this helps us make sure
 * that the correct code is injected*/
class test_gimple_pass : public gimple_opt_pass {
    public:
        test_gimple_pass(const pass_data& data, gcc::context *ctxt)
            : gimple_opt_pass(data, ctxt)
        {}
        unsigned int execute(function *);
};

#endif
