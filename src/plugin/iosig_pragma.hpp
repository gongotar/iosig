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


 #ifndef IOSIG_PRAGMA_HPP
#define IOSIG_PRAGMA_HPP

#include "iosig_parameter.hpp"
#include "iosig_location.hpp"

#include <ostream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

#include <cp-tree.h>
#include <c-family/c-pragma.h>
#include <tree.h>
#include <intl.h>           // G_()

class pragma_base {

private:

    long parse_unit (tree&);

protected:
    
    void check_parameters () const;

    /* different classes/types of pragmas in form of #pragma io <type> */
    enum class pragma_type {
        IO_SIG,     // simple signature describtion before fopen/open calls
        IO_SIGDEF,  // signature definition somewhere before being used (i.e., by sig pragmas)
        IO_JOBWIDE  // job wide definitions describing IO behaviour of the job
    };
    
    const pragma_type type;
    std::vector <std::unique_ptr<const parameter>> signature;


    pragma_base (pragma_type type): type {type}, pragma_loc {nullptr} {
    }
    
public:
    std::unique_ptr <const location> pragma_loc;

    void parse (); // also adds the pragma to the list
    const std::vector <std::unique_ptr<const parameter>> &get_signature () const {
        return signature;
    }

    bool has_param (parameter_type) const;

    template <typename T>
    std::pair<bool, T> get_param_val (parameter_type param_type) const;

    std::unique_ptr <const parameter> copy_param (parameter_type param_type) const;

    friend std::ostream &operator << (std::ostream &os, const pragma_base &pragma);
    
};

class pragma_sigdef: public pragma_base {
private:
    std::string sig_name;
    static std::unordered_map <std::string, std::unique_ptr <const pragma_sigdef>> pragma_sigdef_list; 

public:
    pragma_sigdef (): pragma_base {pragma_type::IO_SIGDEF} {
    }

    void parse ();

    static void insert_pragma_to_list (std::unique_ptr<const pragma_sigdef> pragma) {
        pragma_sigdef_list.insert
            (std::make_pair (pragma->sig_name, std::move (pragma)));
    }

    static bool pragma_exists_in_list (const std::string &name) {
        return pragma_sigdef_list.find (name) != pragma_sigdef_list.end ();
    }

    static const pragma_sigdef &get_pragma_from_list (const std::string &name) {
        return *(pragma_sigdef_list.at (name));
    }

};

class pragma_sig: public pragma_base {
private:
    static std::unordered_map <location, std::unique_ptr <const pragma_sig>> 
        pragma_sig_list;
    
    void inherit (const pragma_sigdef&);
public:
    pragma_sig (): pragma_base {pragma_type::IO_SIG} {
    }

    void parse ();
    
    static void insert_pragma_to_list (std::unique_ptr<const pragma_sig> pragma) {
        pragma_sig_list.insert 
            (std::make_pair (*pragma->pragma_loc, std::move (pragma)));
    }

    static bool pragma_exists_in_list (const location &loc) {
        return pragma_sig_list.find (loc) != pragma_sig_list.end ();
    }

    static const pragma_sig &get_pragma_from_list (const location &loc) {
        return *(pragma_sig_list.at (loc));
    }

    static int number_of_pragmas_in_list () {
        return pragma_sig_list.size ();
    }

    static std::unique_ptr <const pragma_sig> extract_pragma_from_list (const location &loc);
};

class pragma_jobwide: public pragma_base {
public:
    pragma_jobwide (): pragma_base {pragma_type::IO_JOBWIDE} {
    }
};

#endif
