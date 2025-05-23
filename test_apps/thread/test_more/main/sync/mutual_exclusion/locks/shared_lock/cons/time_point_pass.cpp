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

// template <class Clock, class Duration>
//   shared_lock(mutex_type& m, const chrono::time_point<Clock, Duration>& abs_time);

#include <boost/thread/lock_types.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/detail/lightweight_test.hpp>
#include "../../../../../timming.hpp"

static boost::shared_mutex m;

typedef boost::chrono::high_resolution_clock Clock;
typedef Clock::time_point time_point;
typedef Clock::duration duration;
typedef boost::chrono::milliseconds ms;
typedef boost::chrono::nanoseconds ns;
static time_point t0;
static time_point t1;

const ms max_diff(BOOST_THREAD_TEST_TIME_MS);

static void f1()
{
  t0 = Clock::now();
  boost::shared_lock<boost::shared_mutex> lk(m, Clock::now() + ms(750));
  BOOST_TEST(lk.owns_lock() == true);
  t1 = Clock::now();
}

static void f2()
{
  t0 = Clock::now();
  boost::shared_lock<boost::shared_mutex> lk(m, Clock::now() + ms(250));
  BOOST_TEST(lk.owns_lock() == false);
  t1 = Clock::now();
  ns d = t1 - t0 - ms(250);
  BOOST_THREAD_TEST_IT(d, ns(max_diff));
}

static int test_main()
{
  {
    m.lock();
    boost::thread t(f1);
    time_point t2 = Clock::now();
    boost::this_thread::sleep_for(ms(250));
    time_point t3 = Clock::now();
    m.unlock();
    t.join();

    ns sleep_time = t3 - t2;
    ns d_ns = t1 - t0 - sleep_time;
    ms d_ms = boost::chrono::duration_cast<boost::chrono::milliseconds>(d_ns);
    // BOOST_TEST_GE(d_ms.count(), 0);
    BOOST_THREAD_TEST_IT(d_ms, max_diff);
    BOOST_THREAD_TEST_IT(d_ns, ns(max_diff));
  }
  {
    m.lock();
    boost::thread t(f2);
    boost::this_thread::sleep_for(ms(750));
    m.unlock();
    t.join();
  }

  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/locks/shared_lock, time_point_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
