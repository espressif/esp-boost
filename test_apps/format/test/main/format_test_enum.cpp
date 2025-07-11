// ------------------------------------------------------------------------------
//  format_test_enum.cpp :  test format use with enums
// ------------------------------------------------------------------------------

//  Copyright Steven Watanabe 2009.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/format for library home page

// ------------------------------------------------------------------------------

#include <boost/detail/lightweight_test.hpp>
#include <boost/format.hpp>

#include "common.hpp"

namespace {

enum enum_plain { PLAIN };
enum { ANONYMOUS };
enum enum_overloaded { OVERLOADED };
typedef enum { OVERLOADED_TYPEDEF } enum_overloaded_typedef;

std::ostream& operator<<(std::ostream& os, enum_overloaded) {
    os << "overloaded";
    return(os);
}

std::ostream& operator<<(std::ostream& os, enum_overloaded_typedef) {
    os << "overloaded";
    return(os);
}

int main(int, char*[]) {
    // in this case, we should implicitly convert to int
    BOOST_TEST_EQ((boost::format("%d") % PLAIN).str(), "0");
    BOOST_TEST_EQ((boost::format("%d") % ANONYMOUS).str(), "0");

    // but here we need to use the overloaded operator
    BOOST_TEST_EQ((boost::format("%s") % OVERLOADED).str(), "overloaded");
    BOOST_TEST_EQ((boost::format("%s") % OVERLOADED_TYPEDEF).str(), "overloaded");

    return boost::report_errors();
}

} // namespace 

BOOST_AUTO_TEST_CASE(format_test_enum)
{
    TEST_ASSERT(main(0, nullptr) == 0);
}