// Tests for boost::signals2::signal_type

// Copyright Frank Mori Hess 2009.
// Distributed under the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/signals2 for library home page.

#include <boost/signals2.hpp>
#define BOOST_TEST_MODULE signal_type_test
// #include <boost/test/included/unit_test.hpp>

namespace bs2 = boost::signals2;

#include "common.hpp"

BOOST_AUTO_TEST_CASE(signal_type_test)
{
  {
    bs2::signal_type<void ()>::type mysig;
    bs2::signal<void ()> mysig2;
    BOOST_CHECK(typeid(mysig) == typeid(mysig2));
  }

  {
    bs2::signal_type<double (int), bs2::last_value<double> >::type mysig;
    bs2::signal<double (int), bs2::last_value<double> > mysig2;
    BOOST_CHECK(typeid(mysig) == typeid(mysig2));
  }

  {
    using namespace bs2::keywords;
    bs2::signal_type<double (int), group_compare_type<std::less<float> >, group_type<float> >::type mysig;
    bs2::signal<double (int), bs2::optional_last_value<double>, float, std::less<float> > mysig2;
    BOOST_CHECK(typeid(mysig) == typeid(mysig2));
  }

  {
    using namespace bs2::keywords;
    bs2::signal_type<signature_type<float (long*)> >::type mysig;
    bs2::signal<float (long*)> mysig2;
    BOOST_CHECK(typeid(mysig) == typeid(mysig2));
  }
}
