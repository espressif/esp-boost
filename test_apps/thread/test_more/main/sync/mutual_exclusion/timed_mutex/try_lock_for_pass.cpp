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

// <boost/thread/timed_mutex.hpp>

// class timed_mutex;

// template <class Rep, class Period>
//     bool try_lock_for(const chrono::duration<Rep, Period>& rel_time);

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/detail/lightweight_test.hpp>
#include "../../../timming.hpp"

#if defined BOOST_THREAD_USES_CHRONO

static boost::timed_mutex m;

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
  BOOST_TEST(m.try_lock_for(ms(750)) == true);
  t1 = Clock::now();
  m.unlock();
}

static void f2()
{
  t0 = Clock::now();
  BOOST_TEST(m.try_lock_for(ms(250)) == false);
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

#else
#error "Test not applicable: BOOST_THREAD_USES_CHRONO not defined for this platform as not supported"
#endif
#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/timed_mutex, try_lock_for_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
