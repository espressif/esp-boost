// Copyright (C) 2010 Vicente Botet
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG

#include <iostream>
#include <boost/thread/thread_only.hpp>

class ThreadClass
{
public:
  ThreadClass()
  {
  }

  void operator()()
  {
    return;
  }
};

static int test_main()
{
  boost::posix_time::ptime currentTimeUTC;

  ThreadClass tc;
  boost::thread t(tc);
  t.join(); //causes a runtime access violation here

  std::cout << "done" << std::endl;
  //system("pause");

  return 0;
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(test_7160)
{
  common_init();
  std::thread([](){
    TEST_ASSERT(test_main() == 0);
  }).join();
}
