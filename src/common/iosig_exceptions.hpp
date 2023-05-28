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
// Created by Masoud Gholami on 23.04.21.
//

#ifndef IOSIG_IOSIG_EXCEPTIONS_HPP
#define IOSIG_IOSIG_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace iosig::common::exceptions {

    class pipe_init_failure : public std::exception {
    public:
        pipe_init_failure () noexcept = default;

        ~pipe_init_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return "Could not initialise the pipe!";
        }
    };

    class process_fork_failure : public std::exception {
    public:
        process_fork_failure () noexcept = default;

        ~process_fork_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return "Could not fork a new process!";
        }
    };

    class exec_failure : public std::exception {
    public:
        exec_failure () noexcept = default;

        ~exec_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return "Could not execute the shell by performing the exec syscall!";
        }
    };

    class write_to_shell_failure : public std::exception {
    public:
        write_to_shell_failure () noexcept = default;

        ~write_to_shell_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return "Could not write the command to the shell!";
        }
    };

    class read_from_shell_failure : public std::exception {
    public:
        read_from_shell_failure () noexcept = default;

        ~read_from_shell_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return "Could not read the out of the command from the shell!";
        }
    };


    class system_detection_failure : public std::exception {
    private:
        std::string message;
    public:
        explicit system_detection_failure (std::string msg) noexcept: message {std::move (msg)} {}

        ~system_detection_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return message.c_str ();
        }
    };

    class benchmark_failure : public std::exception {
    private:
        std::string message;
    public:
        explicit benchmark_failure (std::string msg) noexcept: message {std::move (msg)} {}

        ~benchmark_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return message.c_str ();
        }
    };

    class fill_memory_failure : public std::exception {
    private:
        std::string message;
    public:
        explicit fill_memory_failure (std::string msg) noexcept: message {std::move (msg)} {}

        ~fill_memory_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return message.c_str ();
        }
    };

    class config_file_failure : public std::exception {
    private:
        std::string message;
    public:
        explicit config_file_failure (std::string msg) noexcept: message {std::move (msg)} {}

        ~config_file_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return message.c_str ();
        }
    };

    class file_handler_failure : public std::exception {
    private:
        std::string message;
    public:
        explicit file_handler_failure (std::string msg) noexcept: message {std::move (msg)} {}

        ~file_handler_failure () override = default;

        [[nodiscard]] const char *what () const noexcept override {
            return message.c_str ();
        }
    };

}


#endif //IOSIG_IOSIG_EXCEPTIONS_HPP
