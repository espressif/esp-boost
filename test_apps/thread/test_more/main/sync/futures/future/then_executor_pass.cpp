// Copyright (C) 2014 Vicente Botet
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/future.hpp>

// class future<R>

// template<typename F>
// auto then(F&& func) -> future<decltype(func(*this))>;

#define BOOST_THREAD_VERSION 5
//#define BOOST_THREAD_USES_LOG
#define BOOST_THREAD_USES_LOG_THREAD_ID
#include <boost/thread/detail/log.hpp>

#include <boost/thread/future.hpp>
#include <boost/thread/executors/basic_thread_pool.hpp>
#include <boost/thread/executor.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <cassert>

#if defined BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION

#ifdef BOOST_MSVC
#pragma warning(disable: 4127) // conditional expression is constant
#endif

static int p1()
{
  BOOST_THREAD_LOG << "p1 < " << BOOST_THREAD_END_LOG;
  boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
  BOOST_THREAD_LOG << "p1 >"  << BOOST_THREAD_END_LOG;
  return 1;
}

static int p2(boost::future<int> f)
{
  assert(f.is_ready());
  BOOST_THREAD_LOG << "p2 <" << &f << BOOST_THREAD_END_LOG;
  BOOST_TEST(f.valid());
  int i = f.get();
  boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
  BOOST_THREAD_LOG << "p2 <" << &f << BOOST_THREAD_END_LOG;
  return 2 * i;
}

static void p3(boost::future<int> f)
{
  assert(f.is_ready());
  BOOST_THREAD_LOG << "p3 <" << &f << BOOST_THREAD_END_LOG;
  BOOST_TEST(f.valid());
  int i = f.get();
  boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
  BOOST_THREAD_LOG << "p3 <" << &f << " " <<i << BOOST_THREAD_END_LOG;
  return;
}

static int test_main()
{
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
    boost::basic_thread_pool ex(1);
    boost::future<int> f1 = boost::async(boost::launch::async, &p1);
    BOOST_TEST(f1.valid());
    boost::future<int> f2 = f1.then(ex, &p2);
    BOOST_TEST(f2.valid());
    BOOST_TEST(! f1.valid());
    try
    {
      BOOST_TEST(f2.get()==2);
    }
    catch (std::exception& ex)
    {
      BOOST_THREAD_LOG << "ERRORRRRR "<<ex.what() << "" << BOOST_THREAD_END_LOG;
      BOOST_TEST(false);
    }
    catch (...)
    {
      BOOST_THREAD_LOG << " ERRORRRRR exception thrown" << BOOST_THREAD_END_LOG;
      BOOST_TEST(false);
    }
  }
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
    boost::basic_thread_pool ex(1);
    boost::future<int> f1 = boost::async(boost::launch::async, &p1);
    BOOST_TEST(f1.valid());
    boost::future<void> f2 = f1.then(ex, &p3);
    BOOST_TEST(f2.valid());
    try
    {
      f2.wait();
    }
    catch (std::exception& ex)
    {
      BOOST_THREAD_LOG << "ERRORRRRR "<<ex.what() << "" << BOOST_THREAD_END_LOG;
      BOOST_TEST(false);
    }
    catch (...)
    {
      BOOST_THREAD_LOG << " ERRORRRRR exception thrown" << BOOST_THREAD_END_LOG;
      BOOST_TEST(false);
    }
  }
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
    boost::basic_thread_pool ex(1);
    boost::future<int> f2 = boost::async(p1).then(ex, &p2);
    BOOST_TEST(f2.get()==2);
  }
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
    boost::basic_thread_pool ex(1);
    boost::future<int> f1 = boost::async(p1);
    boost::future<int> f21 = f1.then(ex, &p2);
    boost::future<int> f2= f21.then(ex, &p2);
    BOOST_TEST(f2.get()==4);
  }
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
    boost::basic_thread_pool ex(1);
    boost::future<int> f1 = boost::async(p1);
    boost::future<int> f21 = f1.then(ex, &p2);
    boost::future<int> f2= f21.then(&p2);
    BOOST_TEST(f2.get()==4);
  }
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
    boost::basic_thread_pool ex(1);
    boost::future<int> f1 = boost::async(p1);
    boost::future<int> f2= f1.then(&p2).then(ex, &p2);
    BOOST_TEST(f2.get()==4);
  }
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
    boost::basic_thread_pool ex(1);
    boost::future<int> f2 = boost::async(p1).then(ex, &p2).then(ex, &p2);
    BOOST_TEST(f2.get()==4);
  }

  return boost::report_errors();
}

#else

static int test_main()
{
  return 0;
}
#endif
#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/futures/future, then_executor_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
