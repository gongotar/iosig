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


 #ifndef CSTYLE_FILE1_HPP
#define CSTYLE_FILE1_HPP

#include <fcntl.h>      // open
#include <unistd.h>     // close
#include <stdio.h>      // fopen, fclose

#include "cstyle_file2.hpp"
#include "cstyle_pragma_defs.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void file1_method1 (long);

#ifdef __cplusplus
}
#endif

#endif
