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
// Created by Masoud Gholami on 16.07.21.
//

#ifndef IOSIG_GENERIC_UTILS_HPP
#define IOSIG_GENERIC_UTILS_HPP

namespace iosig::common::generic {

    /**
     * A class template to determine whether the Derived class is
     * inherited from a specialisation of the Base class.
     * @tparam Base
     * @tparam Derived
     */
    template <template <typename...> class Base, typename Derived>
    struct is_base_of_template {
        template <typename... T>
        static constexpr std::true_type test (const Base <T ...> *);
        static constexpr std::false_type test (...);
        using type = decltype (test (std::declval <Derived *> ()));
    };
    template <template <typename...> class Base, typename Derived>
    inline constexpr bool is_base_of_template_v = is_base_of_template <Base, Derived>::type::value;

}


#endif //IOSIG_GENERIC_UTILS_HPP
