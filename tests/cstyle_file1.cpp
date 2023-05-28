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


 #include "cstyle_file1.hpp"

void file1_method1 (long ss_p_io) {

    char file[] = "file1_path"; 
    int tmpval = 1;
    #pragma io sig totalsize(14*MB) size_per_io(ss_p_io*MB) tmp
    FILE* f= fopen(file, "w+");
    fclose(f);

    #pragma io sig name("bursty") totalsize(10*MB) size_per_io(ss_p_io) sequential_access 
    int fd = open(file, 1);
    close(fd);

    {
        int ss_p_io = 40;
        int temprary_flag = 1;
        char file2[] = "local_path";
        #pragma io sig size_per_io(ss_p_io) tmp(temprary_flag)
        int fd = open (file2, 1);
        close (fd);
    }

    file2_method1 ();

}
