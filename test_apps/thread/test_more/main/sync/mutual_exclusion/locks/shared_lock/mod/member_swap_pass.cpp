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

// void swap(shared_lock& u);

#include <boost/thread/lock_types.hpp>
#include <boost/detail/lightweight_test.hpp>

struct shared_mutex
{
  void lock_shared()
  {
  }
  void unlock_shared()
  {
  }
};

static shared_mutex m;

static int test_main()
{
  boost::shared_lock<shared_mutex> lk1(m);
  boost::shared_lock<shared_mutex> lk2;
  lk1.swap(lk2);
  BOOST_TEST(lk1.mutex() == 0);
  BOOST_TEST(lk1.owns_lock() == false);
  BOOST_TEST(lk2.mutex() == &m);
  BOOST_TEST(lk2.owns_lock() == true);

  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/locks/shared_lock, member_swap_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
