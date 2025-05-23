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

// template <class Mutex> class shared_lock;

// template <class Rep, class Period>
//   bool try_lock_for(const chrono::duration<Rep, Period>& rel_time);

#include <boost/thread/lock_types.hpp>
//#include <boost/thread/shared_mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

static bool try_lock_called = false;

struct shared_mutex
{
  bool try_lock_shared()
  {
    try_lock_called = !try_lock_called;
    return try_lock_called;
  }
  void unlock_shared()
  {
  }
};

static shared_mutex m;

static int test_main()
{
  boost::shared_lock<shared_mutex> lk(m, boost::defer_lock);
  BOOST_TEST(lk.try_lock() == true);
  BOOST_TEST(try_lock_called == true);
  BOOST_TEST(lk.owns_lock() == true);
  try
  {
    lk.try_lock();
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::resource_deadlock_would_occur);
  }
  lk.unlock();
  BOOST_TEST(lk.try_lock() == false);
  BOOST_TEST(try_lock_called == false);
  BOOST_TEST(lk.owns_lock() == false);
  lk.release();
  try
  {
    lk.try_lock();
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::operation_not_permitted);
  }

  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/locks/shared_lock, try_lock_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
