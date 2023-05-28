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
// Created by Masoud Gholami on 24.04.21.
//

#ifndef IOSIG_FILESYSTEM_HPP
#define IOSIG_FILESYSTEM_HPP

#include <string>

namespace iosig::sys_inspect {

    class filesystem {
    public:
        explicit filesystem (std::string fs_name) : name {std::move (fs_name)} {
            max_filesize = segment_size = err_fix_per_segment = blocksize = -1;
        }

        const std::string name;
        long blocksize;
        long max_filesize;
        long segment_size;
        long err_fix_per_segment;

        filesystem (const filesystem &) = delete;

        filesystem &operator= (const filesystem &) = delete;

    };
}
#endif //IOSIG_FILESYSTEM_HPP
