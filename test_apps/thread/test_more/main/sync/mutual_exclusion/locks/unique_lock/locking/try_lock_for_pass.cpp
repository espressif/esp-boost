//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// Copyright (C) 2011 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/locks.hpp>

// template <class Mutex> class unique_lock;

// template <class Rep, class Period>
//   bool try_lock_for(const chrono::duration<Rep, Period>& rel_time);

#include <boost/thread/lock_types.hpp>
//#include <boost/thread/mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

#if defined BOOST_THREAD_USES_CHRONO


static bool try_lock_for_called = false;

typedef boost::chrono::milliseconds ms;

struct mutex
{
  template <class Rep, class Period>
  bool try_lock_for(const boost::chrono::duration<Rep, Period>& rel_time)
  {
    BOOST_TEST(rel_time == ms(5));
    try_lock_for_called = !try_lock_for_called;
    return try_lock_for_called;
  }
  void unlock()
  {
  }
};

static mutex m;

static int test_main()
{
  boost::unique_lock<mutex> lk(m, boost::defer_lock);
  BOOST_TEST(lk.try_lock_for(ms(5)) == true);
  BOOST_TEST(try_lock_for_called == true);
  BOOST_TEST(lk.owns_lock() == true);
  try
  {
    lk.try_lock_for(ms(5));
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::resource_deadlock_would_occur);
  }
  lk.unlock();
  BOOST_TEST(lk.try_lock_for(ms(5)) == false);
  BOOST_TEST(try_lock_for_called == false);
  BOOST_TEST(lk.owns_lock() == false);
  lk.release();
  try
  {
    lk.try_lock_for(ms(5));
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::operation_not_permitted);
  }

  return boost::report_errors();
}

#else
#error "Test not applicable: BOOST_THREAD_USES_CHRONO not defined for this platform as not supported"
#endif

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/locks/unique_lock, try_lock_for_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
