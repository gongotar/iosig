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
 * CURRENTLY NOT WORKING
 *
 * The API (sometimes injeted in programs) to check the destination 
 * file and make decisions on wether or not to move the 
 * file or to symlink to it.
 *
 * from comments:
 * 1. check if the file is a symlink
 * then, if symlink, check the size, the disk bandwidth (how?), 
 * the opt_file disk bandwidth, calculate the delta, if 
 * worth, move the file to the opt, otherwise, write to the 
 * same file without movements
 * 
 * 2. check if the file exists, if so, same as above
 * 3. if neither of the cases of 1 and 2, then execute the simple csae 
 * of what we already have
 *
 */
#ifndef IOSIG_LIB_HPP
#define IOSIG_LIB_HPP

#include <filesystem>
#include <fstream>
#include <iostream>

#include "common/iosig_common_parameter.hpp"

#ifdef __cplusplus
    extern "C" {
#endif

int _iosig_fn_redirect_inject_ (const char* path, long *params, size_t params_size);

#ifdef __cplusplus
    }
#endif

#endif
