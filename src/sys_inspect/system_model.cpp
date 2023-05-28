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
// Created by Masoud Gholami on 21.04.21.
//


#include "system_model.hpp"

void iosig::sys_inspect::system_model::lookup_blockdevices () {

    std::string lsblk = exec_command ("lsblk --output NAME,MAJ:MIN,LABEL,MODEL,SIZE,MIN-IO,PHY-SEC,LOG-SEC,TYPE,PKNAME -pPbn");
    std::stringstream lsblk_ss {lsblk};

    std::regex regname      {R"(NAME="[^"]*")"};
    std::regex regtype      {R"(TYPE="[^"]*")"};
    std::regex reglabel     {R"(LABEL="[^"]*")"};
    std::regex regpkname    {R"(PKNAME="[^"]*")"};
    std::regex reglogsec    {R"(LOG-SEC="[^"]*")"};
    std::regex regphysec    {R"(PHY-SEC="[^"]*")"};
    std::regex regsize      {R"(SIZE="[^"]*")"};
    std::regex regmodel     {R"(MODEL="[^"]*")"};
    std::regex regmajmin    {R"(MAJ:MIN="[^"]*")"};

    std::smatch match;
    std::string line, type, name;

    while (std::getline (lsblk_ss, line)) {

        // fetch name and type of the device
        std::regex_search (line, match, regtype);
        type = match.str ().substr (6, match.str ().size ()-7);

        std::regex_search (line, match, regname);
        name = match.str ().substr (6, match.str ().size ()-7);

        // create the corresponding device instance + type specific operations
        std::shared_ptr <blockdevice> dev;

        if (type == "disk") {
            dev = std::make_shared <simple_blockdevice> (name);

            static_cast <simple_blockdevice&> (*dev).is_volatile = false;
        }
        else if (type == "part") {
            dev = std::make_shared <partition_blockdevice> (name);

            std::regex_search (line, match, regpkname);
            std::string owner = match.str ().substr (8, match.str ().size ()-9);
            static_cast <partition_blockdevice&> (*dev).owner = devices.at (owner);
        }
        else if (type == "lvm") {
            dev = std::make_shared <lvm_blockdevice> (name);
            
            std::regex_search (line, match, regpkname);
            std::string parent = match.str ().substr (8, match.str ().size ()-9);
            static_cast <lvm_blockdevice&> (*dev).parents.push_back (devices.at (parent));
        }
        else {
            std::clog << "Unhandeled blockdevice type " << type << std::endl;
        }
        
        
        std::regex_search (line, match, reglabel);
        dev->label = match.str ().substr (7, match.str ().size ()-8);
        std::regex_search (line, match, reglogsec);
        dev->logical_sector = std::stol (match.str ().substr (9, match.str ().size ()-10));
        std::regex_search (line, match, regphysec);
        dev->physical_sector = std::stol (match.str ().substr (9, match.str ().size ()-10));
        std::regex_search (line, match, regsize);
        dev->capacity = std::stoul (match.str ().substr (6, match.str ().size ()-7));
        std::regex_search (line, match, regmodel);
        dev->model = match.str ().substr (7, match.str ().size ()-7);
        std::regex_search (line, match, regmajmin);
        std::string majmin = match.str ().substr (9, match.str ().size ()-10);
        size_t pos = majmin.find (':');
        dev->major = std::stoi (majmin.substr (0, pos));
        dev->minor = std::stoi (majmin.substr (pos+1, majmin.size ()));

        devices.insert (std::make_pair (name, dev));
    }
}

void iosig::sys_inspect::system_model::lookup_mountpoints () {

    if (devices.empty ()) {
        throw common::exceptions::system_detection_failure
            ("To lookup for mountpoints, blockdevices must be detected first!");
    }

    std::string df = exec_command ("df --output=source,avail,fstype,target,size -k");
    std::stringstream df_ss {df};

    std::string target, source, fstype, line;
    unsigned long long avail, size;
    
    while (std::getline (df_ss, line)) {
        std::stringstream line_ss {line};

        line_ss >> source >> avail >> fstype >> target >> size;

        // unit is kilobyte because of the -k flag passed to the df command
        size = size * 1024ull;
        avail = avail * 1024ull;
        
        // ignore if not applicable
        if (mountpoint::is_system_mountpoint (target) || 
                !mountpoint::is_valid_mountpoint (target)) {
            continue;
        }

        // handle the blockdevice
        auto bd_find = devices.find (source);
        if (bd_find == devices.end ()) {
            // must be either a tmpfs or a network/remote blockdevice
            if (source == "tmpfs") {
                std::shared_ptr <simple_blockdevice> dev = 
                    std::make_shared <simple_blockdevice> (source);
                dev->is_volatile = true;
                dev->capacity = size;
                devices.emplace (source, dev);
            }
            else {
                std::shared_ptr <remote_blockdevice> dev = 
                    std::make_shared <remote_blockdevice> (source);
                dev->capacity = size;
                devices.emplace (source, dev);
            }
        }

        // handle the filesystem
        auto fs_find = filesystems.find (fstype);
        if (fs_find == filesystems.end ()) {
            filesystems.emplace (fstype, std::make_shared <filesystem> (fstype));
        }

        auto &mp = mountpoints.emplace_back (target, devices.at (source), filesystems.at (fstype));
        mp.free_space = avail;
        mp.capacity = size;


    }

}

void iosig::sys_inspect::system_model::match_system_config () {
    if (mountpoints.empty ()) {
        throw common::exceptions::system_detection_failure
            ("To match the system config, mountpoints must be detected first!");
    }

    auto conf = sysconf.conf_data;
    
    for (auto &mp: mountpoints) {
        auto conf_mp = std::find_if (conf.begin (), conf.end (), 
                [&mp] (auto &item) {
                    return item.type == "mountpoint" && item.name == mp.path;
                });

        if (conf_mp == conf.end ()) {
            // TODO: create a template from the mountpoint in the config
            mp.ignore = true;
        }
        else { 
            for (const auto &property: conf_mp->paired_values) {
                if (property.first == "prefix") {
                    mp.prefix = property.second;
                }

                if (property.first == "ignore") {
                    std::istringstream sb {property.second};
                    sb >> mp.ignore;
                    if (sb.fail ()) {
                        sb.clear();
                        sb >> std::boolalpha >> mp.ignore;
                        if (sb.fail ()) {
                            throw common::exceptions::system_detection_failure
                                ("Failure in casting the string value to boolean for ignore");
                        }
                    }
                }

                // TODO: match other properties here
            }
        }

        // TODO: list_values properties as well
        
        if (!mp.ignore) {
            std::clog << "For the mountpoint " << mp.path << " prefix " << 
                        mp.prefix << " will be used."<< std::endl;
        }
        else {
            std::clog << "The mountpoint " << mp.path << " will be ignored" << std::endl;
        }
    }
}


void iosig::sys_inspect::system_model::exec_benchmarks () {
    
    if (mountpoints.empty ()) {
        throw common::exceptions::system_detection_failure
            ("To execute  benchmarks, mountpoints must be detected first!");
    }

    for (auto &mp: mountpoints) {
       
        if (mp.ignore) {
            continue;
        }

        if (mp.prefix.empty()) {
            throw common::exceptions::system_detection_failure ("No prefix is defined for the mountpoint " + mp.path);
        }

        benchmarks::schemes::blocksize_basic        bs_basic {mp.path, mp.prefix, mp.free_space};
        benchmarks::schemes::blocksize_basic_sync   bs_basic_sync {mp.path, mp.prefix, mp.free_space};
        benchmarks::schemes::blocksize_buffered     bs_buffered {mp.path, mp.prefix, mp.free_space};
        benchmarks::schemes::blocksize_fstream      bs_fstream {mp.path, mp.prefix, mp.free_space};
        benchmarks::schemes::blocksize_mmap         bs_mmap {mp.path, mp.prefix, mp.free_space};
        benchmarks::schemes::filesize_mmap          fs_mmap {mp.path, mp.prefix, mp.free_space};
        benchmarks::schemes::filesize_fstream       fs_fstream {mp.path, mp.prefix, mp.free_space};

        benchmarks::schemes::perform_schemes (std::clog,
                                              bs_basic,
                                              bs_basic_sync,
                                              bs_buffered,
                                              bs_fstream,
                                              bs_mmap,
                                              fs_mmap,
                                              fs_fstream);

        //plotter::plot_benchmarks pl (fs_mmap.get_result());
        //pl.show();
        // TODO: implement a machine learner in python
        // TODO: to generate random IO use mmap and random access to the mapped array
    }


}

std::string iosig::sys_inspect::system_model::exec_command (std::string cmd) const {

    ssize_t ret;
    char buffer[max_command_output_size];

    cmd += '\n';

    ret = write (write_to_shell[1], cmd.c_str (), cmd.size ());
    if (ret != static_cast <ssize_t> (cmd.size ())) {
        throw common::exceptions::write_to_shell_failure ();
    }

    ret = read (read_from_shell[0], buffer, max_command_output_size);
    if (ret < 0) {
        throw common::exceptions::read_from_shell_failure ();
    }
    buffer[std::max (ret - 1, 0l)] = '\0';

    return std::string (buffer);
}


void iosig::sys_inspect::system_model::init_shell () {

    // create a pipe to communicate between child and parent
    if (pipe (read_from_shell) < 0 || pipe (write_to_shell) < 0) {
        throw common::exceptions::pipe_init_failure ();
    }

    pid_t pid = fork ();

    if (pid == 0) {             // child process

        dup2 (write_to_shell[0], STDIN_FILENO);
        dup2 (read_from_shell[1], STDOUT_FILENO);
        dup2 (read_from_shell[1], STDERR_FILENO);

        close (read_from_shell[0]);
        close (write_to_shell[0]);
        close (read_from_shell[1]);
        close (write_to_shell[1]);

        char *args[] = {const_cast<char *> (shell), nullptr};
        execv (args[0], args);
        throw common::exceptions::exec_failure ();   // if exec returns then we have an error
    } else if (pid > 0) {         // parent
        close (write_to_shell[0]);
        close (read_from_shell[1]);
    } else {                      // unsuccessful fork
        throw common::exceptions::process_fork_failure ();
    }
}

void iosig::sys_inspect::system_model::finalize_shell () {
    exec_command (shell_exit);
    close (write_to_shell[1]);
    close (read_from_shell[0]);

    int status;
    wait (&status);
    std::clog << "Closed the shell with status " << status << std::endl;
}
