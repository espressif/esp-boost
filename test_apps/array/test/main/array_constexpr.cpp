/* tests using constexpr on boost:array
 * (C) Copyright Marshall Clow 2012
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */ 

// Note: This test is not working

/*
#include <string>
#include <iostream>
#include <boost/array.hpp>
#include <algorithm>

#include "common.hpp"

namespace {

#ifndef BOOST_NO_CXX11_HDR_ARRAY
#include <array>
#endif

#ifndef BOOST_NO_CXX11_CONSTEXPR
constexpr boost::array<int, 10> arr  {{ 0,1,2,3,4,5,6,7,8,9 }};
constexpr std::array<int, 10> arr_std {{ 0,1,2,3,4,5,6,7,8,9 }};

template <typename T>
void sink ( T t ) {}

template <typename T, size_t N>
void sink ( boost::array<T,N> &arr ) {}

int main()
{
//    constexpr int two = arr_std.at (2);
    constexpr int three = arr.at (3);
    int whatever [ arr.at(4) ];
    (void)three;
    (void) whatever;
}

#else   // no constexpr means no constexpr tests!
int main()
{
}
#endif

} // namespace

BOOST_AUTO_TEST_CASE( array_constexpr )
{
    TEST_ASSERT(main() == 0);
}
*/
