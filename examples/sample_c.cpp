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


 #include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#pragma io sigdef name("bursty") tmp size_per_io(10*KB) min_mttdl(10000*h) totalsize(20*MB)

int main(int argc, char *argv[]) {

    long ss_p_io = 100;
    char file[] = "filename"; 
    printf ("hi there 1\n");
    #pragma io sig name("bursty") totalsize(10*MB) size_per_io(ss_p_io) random_access 
    int fd = open(file, 1);
    close(fd);

    #pragma io sig name("bursty") totalsize(3*MB) size_per_io(ss_p_io*MB) archive 
    FILE* f= fopen(file, "w+");
    fclose(f);

    printf ("ss_p_io %ld\n", ss_p_io);
	return 0;

}

