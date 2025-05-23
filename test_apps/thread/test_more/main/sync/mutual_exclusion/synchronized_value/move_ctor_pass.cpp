// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/synchronized_value.hpp>

// class synchronized_value<T,M>

// synchronized_value(synchronized_value &&);

#define BOOST_THREAD_VERSION 4

#include <boost/thread/synchronized_value.hpp>

#include <boost/detail/lightweight_test.hpp>

static int test_main()
{

  {
    boost::synchronized_value<int> v1(1);
    boost::synchronized_value<int> v2(boost::move(v1));
    BOOST_TEST(v2.value() == 1);
  }

  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/synchronized_value, move_ctor_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
