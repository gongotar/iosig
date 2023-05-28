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


 #include <cstring>
#include <fstream>

#pragma io sigdef name("bursty") tmp size_per_io(10*KB) min_mttdl(10000*h)
int main(int argc, char *argv[]) {

    std::string filename ("test/file");
    std::fstream fd;    
    #pragma io sig name("bursty") totalsize(10*MB) size_per_io(20*KB) random_access 
    fd.open (filename, std::fstream::in);
    fd.close(); 


}

