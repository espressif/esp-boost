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

// <boost/thread/null_mutex>

// class null_mutex;

// template <class Clock, class Duration>
//     bool try_lock_until(const chrono::time_point<Clock, Duration>& abs_time);

#include <boost/thread/null_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/detail/lightweight_test.hpp>
#include "../../../timming.hpp"

#if defined BOOST_THREAD_USES_CHRONO

boost::null_mutex m;

typedef boost::chrono::steady_clock Clock;
typedef Clock::time_point time_point;
typedef Clock::duration duration;
typedef boost::chrono::milliseconds ms;
typedef boost::chrono::nanoseconds ns;

const ms max_diff(BOOST_THREAD_TEST_TIME_MS);

static void f1()
{
  time_point t0 = Clock::now();
  BOOST_TEST(m.try_lock_until(Clock::now() + ms(250)) == true);
  time_point t1 = Clock::now();
  m.unlock();
  ns d = t1 - t0 ;
  BOOST_THREAD_TEST_IT(d, ns(max_diff));
}


static int test_main()
{
  {
    m.lock();
    boost::thread t(f1);
    m.unlock();
    t.join();
  }

  return boost::report_errors();
}

#else
#error "Test not applicable: BOOST_THREAD_USES_CHRONO not defined for this platform as not supported"
#endif
#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/null_mutex, try_lock_until_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
