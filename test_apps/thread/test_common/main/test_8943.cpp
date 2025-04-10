// Copyright (C) 2013 Vicente Botet
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/detail/lightweight_test.hpp>

#if defined(WIN32)
#include <tchar.h>
#endif

#include <cstdlib>
#include <iostream>
#include <boost/thread/once.hpp>

namespace {

class foo
{
public:
  void operator()() const
  {
    std::cout << "foo" << std::endl;
  }
}; // class foo

}

#if defined(WIN32)
static int test_main(int /*argc*/, _TCHAR* /*argv*/[])
#else
static int test_main(int /*argc*/, char* /*argv*/[])
#endif
{
  try
  {
    boost::once_flag once_flag = BOOST_ONCE_INIT;
    boost::call_once(once_flag, foo());
    return EXIT_SUCCESS;
  }
  catch (...)
  {
    std::cerr << "Unknown exception" << std::endl;
    BOOST_TEST(false);
  }
  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(test_8943)
{
      common_init();
    std::thread([]() {
        TEST_ASSERT(test_main(0, nullptr) == 0);
    }).join();
}
