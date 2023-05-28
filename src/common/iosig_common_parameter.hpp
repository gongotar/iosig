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


 #ifndef IOSIG_COMMON_PARAMETER_HPP
#define IOSIG_COMMON_PARAMETER_HPP

// Parameters and their integer representations.
enum class parameter_type {
    size_io     = 1,
    totalsize   = 2, 
    min_mttdl   = 3,
    global      = 4,
    local       = 5,
    tmp         = 6,
    persist     = 7,
    seq_acc     = 8,
    rnd_acc     = 9,
    tamper      = 10,
    archive     = 11,
    name        = 12
};

class cparameter {
public:
    parameter_type  param_type;
    long            value;
};

#endif
