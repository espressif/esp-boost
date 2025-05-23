// Copyright (C) 2011 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_THREAD_USES_MOVE

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/csbl/vector.hpp>
#include <iostream>
#include <boost/detail/lightweight_test.hpp>
#include <boost/static_assert.hpp>

static int count = 0;
static boost::mutex mutex;

namespace
{
template <typename TC>
static void join_all(TC & tc)
{
  for (typename TC::iterator it = tc.begin(); it != tc.end(); ++it)
  {
    it->join();
  }
}

template <typename TC>
static void interrupt_all(TC & tc)
{
#if defined BOOST_THREAD_PROVIDES_INTERRUPTIONS
  for (typename TC::iterator it = tc.begin(); it != tc.end(); ++it)
  {
    it->interrupt();
  }
#endif
}
}

static void increment_count()
{
  boost::unique_lock<boost::mutex> lock(mutex);
  std::cout << "count = " << ++count << std::endl;
}

#if defined  BOOST_NO_CXX11_RVALUE_REFERENCES && defined BOOST_THREAD_USES_MOVE
BOOST_STATIC_ASSERT(::boost::is_function<boost::rv<boost::rv<boost::thread> >&>::value==false);
#endif

static int test_main()
{
  typedef boost::csbl::vector<boost::thread> thread_vector;
  {
    thread_vector threads;
    threads.reserve(10);
    for (int i = 0; i < 10; ++i)
    {
      boost::thread th(&increment_count);
      threads.push_back(boost::move(th));
    }
    join_all(threads);
  }
  count = 0;
  {
    thread_vector threads;
    threads.reserve(10);
    for (int i = 0; i < 10; ++i)
    {
      threads.push_back(BOOST_THREAD_MAKE_RV_REF(boost::thread(&increment_count)));
    }
    join_all(threads);
  }
  count = 0;
  {
    thread_vector threads;
    threads.reserve(10);
    for (int i = 0; i < 10; ++i)
    {
      threads.emplace_back(&increment_count);
    }
    join_all(threads);
  }
  count = 0;
  {
    thread_vector threads;
    threads.reserve(10);
    for (int i = 0; i < 10; ++i)
    {
      threads.emplace_back(&increment_count);
    }
    interrupt_all(threads);
    join_all(threads);
  }

  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(threads/container, thread_vector_pass)
{
  common_init();
  std::thread([&]() {
    TEST_ASSERT(test_main() == 0);
  }).join();
}
