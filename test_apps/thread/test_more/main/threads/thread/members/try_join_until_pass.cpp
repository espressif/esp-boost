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

// <boost/thread/thread.hpp>

// class thread

//        template <class Clock, class Duration>
//        bool try_join_until(const chrono::time_point<Clock, Duration>& t);

#define BOOST_THREAD_VESRION 3
/* esp32: Enable this macro to make `join()` throw an exception when joining itself */
#define BOOST_THREAD_PROVIDE_JOIN_CHECK_ID
#include <boost/thread/thread_only.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <new>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <boost/detail/lightweight_test.hpp>

#if defined BOOST_THREAD_USES_CHRONO

namespace {

class G
{
  int alive_;
public:
  static bool op_run;

  G() :
    alive_(1)
  {
  }
  G(const G& g) :
    alive_(g.alive_)
  {
  }
  ~G()
  {
    alive_ = 0;
  }

  void operator()()
  {
    BOOST_TEST(alive_ == 1);
    op_run = true;
  }
};

bool G::op_run = false;

boost::thread* resource_deadlock_would_occur_th=0;
boost::mutex resource_deadlock_would_occur_mtx;
static void resource_deadlock_would_occur_tester()
{
  try
  {
    boost::unique_lock<boost::mutex> lk(resource_deadlock_would_occur_mtx);
    resource_deadlock_would_occur_th->try_join_until(boost::chrono::steady_clock::now()+boost::chrono::milliseconds(50));
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::resource_deadlock_would_occur);
  }
  catch (...)
  {
    BOOST_TEST(false&&"exception thrown");
  }
}

static void th_250_ms()
{
  boost::this_thread::sleep_for(boost::chrono::milliseconds(250));
}


static int test_main()
{
  {
    boost::thread t0( (G()));
    BOOST_TEST(t0.joinable());
    t0.try_join_until(boost::chrono::steady_clock::now()+boost::chrono::milliseconds(250));
    BOOST_TEST(!t0.joinable());
  }
  {
    boost::thread t0( (th_250_ms));
    BOOST_TEST(!t0.try_join_until(boost::chrono::steady_clock::now()+boost::chrono::milliseconds(50)));
    t0.join();
  }

  {
    boost::unique_lock<boost::mutex> lk(resource_deadlock_would_occur_mtx);
    boost::thread t0( resource_deadlock_would_occur_tester );
    resource_deadlock_would_occur_th = &t0;
    BOOST_TEST(t0.joinable());
    lk.unlock();
    boost::this_thread::sleep_for(boost::chrono::milliseconds(250));
    boost::unique_lock<boost::mutex> lk2(resource_deadlock_would_occur_mtx);
    t0.join();
    BOOST_TEST(!t0.joinable());
  }

  {
    boost::thread t0( (G()));
    t0.detach();
    try
    {
      t0.try_join_until(boost::chrono::steady_clock::now()+boost::chrono::milliseconds(50));
      BOOST_TEST(false);
    }
    catch (boost::system::system_error& e)
    {
      BOOST_TEST(e.code().value() == boost::system::errc::invalid_argument);
    }
  }
  {
    boost::thread t0( (G()));
    BOOST_TEST(t0.joinable());
    t0.join();
    try
    {
      t0.try_join_until(boost::chrono::steady_clock::now()+boost::chrono::milliseconds(50));
      BOOST_TEST(false);
    }
    catch (boost::system::system_error& e)
    {
      BOOST_TEST(e.code().value() == boost::system::errc::invalid_argument);
    }

  }
  {
    boost::thread t0( (G()));
    BOOST_TEST(t0.joinable());
    t0.try_join_until(boost::chrono::steady_clock::now()+boost::chrono::milliseconds(250));
    try
    {
      t0.join();
      BOOST_TEST(false);
    }
    catch (boost::system::system_error& e)
    {
      BOOST_TEST(e.code().value() == boost::system::errc::invalid_argument);
    }

  }

  return boost::report_errors();
}

#else
#error "Test not applicable: BOOST_THREAD_USES_CHRONO not defined for this platform as not supported"
#endif

}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(threads/thread/members, try_join_until_pass)
{
  common_init();
  std::thread([&]() {
    TEST_ASSERT(test_main() == 0);
  }).join();
}
