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


 #ifndef IOSIG_LOCATION_HPP
#define IOSIG_LOCATION_HPP

#include <string>

#include <gcc-plugin.h>

class location {
public:
    std::string file;
    int line;
    const location_t loc;
    
    location (const location_t &loc): loc {loc} {
        expanded_location xloc = expand_location(loc);
        line = xloc.line;
        file = std::string (xloc.file);
    }

    location previous_line () const {
        location prev {*this};
        prev.line --;
        return prev;
    }

    bool operator== (const location &loc2) const {
        return (line == loc2.line) && (file == loc2.file);
    }
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

namespace std {
    template <> struct hash <location> {
        std::size_t operator () (const location &loc) const {
            std::size_t hash = 0;
            hash_combine(hash, loc.file);
            hash_combine(hash, loc.line);
            return hash;
        }
    };
}

#endif
