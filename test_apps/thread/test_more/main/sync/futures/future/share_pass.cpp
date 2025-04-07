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

// <boost/thread/future.hpp>

// class future<R>

// shared_future<R> share() &&;

#define BOOST_THREAD_VERSION 3

#include <boost/thread/future.hpp>
#include <boost/detail/lightweight_test.hpp>

static int test_main()
{

  {
      typedef int T;
      boost::promise<T> p;
      boost::future<T> f0 = BOOST_THREAD_MAKE_RV_REF(p.get_future());
      boost::shared_future<T> sf = f0.share();
      boost::shared_future<T> f = sf;
      BOOST_TEST(!f0.valid());
      BOOST_TEST(f.valid());
  }
  {
      typedef int T;
      boost::future<T> f0;
      boost::shared_future<T> sf = f0.share();
      boost::shared_future<T> f = sf;
      BOOST_TEST(!f0.valid());
      BOOST_TEST(!f.valid());
  }
  {
      typedef int& T;
      boost::promise<T> p;
      boost::future<T> f0 = BOOST_THREAD_MAKE_RV_REF(p.get_future());
      boost::shared_future<T> sf = f0.share();
      boost::shared_future<T> f = sf;
      BOOST_TEST(!f0.valid());
      BOOST_TEST(f.valid());
  }
  {
      typedef int& T;
      boost::future<T> f0;
      boost::shared_future<T> sf = f0.share();
      boost::shared_future<T> f = sf;
      BOOST_TEST(!f0.valid());
      BOOST_TEST(!f.valid());
  }
  {
      typedef void T;
      boost::promise<T> p;
      boost::future<T> f0 = BOOST_THREAD_MAKE_RV_REF(p.get_future());
      boost::shared_future<T> sf = f0.share();
      boost::shared_future<T> f = sf;
      BOOST_TEST(!f0.valid());
      BOOST_TEST(f.valid());
  }
  {
      typedef void T;
      boost::future<T> f0;
      boost::shared_future<T> sf = f0.share();
      boost::shared_future<T> f = sf;
      BOOST_TEST(!f0.valid());
      BOOST_TEST(!f.valid());
  }


  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/futures/future, share_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
