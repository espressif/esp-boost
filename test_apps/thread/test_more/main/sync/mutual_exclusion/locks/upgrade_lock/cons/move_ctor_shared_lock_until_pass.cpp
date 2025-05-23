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

// template <class Mutex> class upgrade_lock;

//         template <class Clock, class Duration>
//         upgrade_lock(shared_lock<mutex_type>&&,
//                     const chrono::time_point<Clock, Duration>&);

#define BOOST_THREAD_PROVIDES_SHARED_MUTEX_UPWARDS_CONVERSIONS
#define BOOST_THREAD_PROVIDES_EXPLICIT_LOCK_CONVERSION
#define BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN

#include <boost/thread/lock_types.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

static boost::shared_mutex m;

static int test_main()
{
  {
    boost::shared_lock<boost::shared_mutex> lk0(m);
    boost::upgrade_lock<boost::shared_mutex> lk( boost::move(lk0), boost::chrono::steady_clock::now()+boost::chrono::milliseconds(1));
    BOOST_TEST(lk.mutex() == &m);
    BOOST_TEST(lk.owns_lock() == true);
    BOOST_TEST(lk0.mutex() == 0);
    BOOST_TEST(lk0.owns_lock() == false);
  }
  {
    boost::upgrade_lock<boost::shared_mutex>
        lk( boost::shared_lock<boost::shared_mutex>(m), boost::chrono::steady_clock::now()+boost::chrono::milliseconds(1));
    BOOST_TEST(lk.mutex() == &m);
    BOOST_TEST(lk.owns_lock() == true);
  }
  {
    boost::shared_lock<boost::shared_mutex> lk0(m, boost::defer_lock);
    boost::upgrade_lock<boost::shared_mutex> lk( boost::move(lk0), boost::chrono::steady_clock::now()+boost::chrono::milliseconds(1));
    BOOST_TEST(lk.mutex() == &m);
    BOOST_TEST(lk.owns_lock() == false);
    BOOST_TEST(lk0.mutex() == 0);
    BOOST_TEST(lk0.owns_lock() == false);
  }
  {
    boost::shared_lock<boost::shared_mutex> lk0(m, boost::defer_lock);
    lk0.release();
    boost::upgrade_lock<boost::shared_mutex> lk( boost::move(lk0), boost::chrono::steady_clock::now()+boost::chrono::milliseconds(1));
    BOOST_TEST(lk.mutex() == 0);
    BOOST_TEST(lk.owns_lock() == false);
    BOOST_TEST(lk0.mutex() == 0);
    BOOST_TEST(lk0.owns_lock() == false);
  }

  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/locks/upgrade_lock, move_ctor_shared_lock_until_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
