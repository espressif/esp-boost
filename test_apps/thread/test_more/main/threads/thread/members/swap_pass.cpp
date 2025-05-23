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

// native_handle_type native_handle();

#include <boost/thread/thread_only.hpp>
#include <cstdlib>
#include <boost/detail/lightweight_test.hpp>

namespace {

class G
{
  int alive_;
public:
  static int n_alive;
  static bool op_run;

  G() :
    alive_(1)
  {
    ++n_alive;
  }
  G(const G& g) :
    alive_(g.alive_)
  {
    ++n_alive;
  }
  ~G()
  {
    alive_ = 0;
    --n_alive;
  }

  void operator()()
  {
    BOOST_TEST(alive_ == 1);
    std::cout << n_alive << std::endl;
    //BOOST_TEST(n_alive == 1);
    op_run = true;
  }
};

int G::n_alive = 0;
bool G::op_run = false;

static int test_main()
{
  {
    boost::thread t0( (G()));
    boost::thread::id id0 = t0.get_id();
    boost::thread t1;
    boost::thread::id id1 = t1.get_id();
    t0.swap(t1);
    BOOST_TEST(t0.get_id() == id1);
    BOOST_TEST(t1.get_id() == id0);
    t1.join();
    return boost::report_errors();
  }
}

}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(threads/thread/members, swap_pass)
{
  common_init();
  std::thread([&]() {
    TEST_ASSERT(test_main() == 0);
  }).join();
}
