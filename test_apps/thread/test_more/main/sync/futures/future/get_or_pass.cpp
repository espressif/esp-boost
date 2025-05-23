// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/future.hpp>

// class future<R>

// R future::get_or(R&&);
// R& future<R&>::get_or(R&);

#define BOOST_THREAD_VERSION 4
//#define BOOST_THREAD_USES_LOG
#define BOOST_THREAD_USES_LOG_THREAD_ID
#include <boost/thread/detail/log.hpp>

#include <boost/thread/future.hpp>
#include <boost/thread/thread.hpp>
#include <boost/core/ref.hpp>
#include <boost/detail/lightweight_test.hpp>

#if defined BOOST_THREAD_USES_CHRONO

#ifdef BOOST_MSVC
#pragma warning(disable: 4127) // conditional expression is constant
#endif

template <typename T>
struct wrap
{
  wrap(T const& v) : value(v){}
  T value;

};

template <typename T>
boost::exception_ptr make_exception_ptr(T v) {
  return boost::copy_exception(wrap<T>(v));
}

static void func1(boost::promise<int> p)
{
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    p.set_value(3);
}

static void func2(boost::promise<int> p)
{
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    p.set_exception(::make_exception_ptr(3));
}

int j = 0;

static void func3(boost::promise<int&> p)
{
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    j = 5;
    p.set_value(j);
}

static void func4(boost::promise<int&> p)
{
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    p.set_exception(::make_exception_ptr(3.5));
}

static void func5(boost::promise<void> p)
{
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    p.set_value();
}

static void func6(boost::promise<void> p)
{
    boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    p.set_exception(::make_exception_ptr(4));
}


static int test_main()
{
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
      typedef int T;
      {
          boost::promise<T> p;
          boost::future<T> f = p.get_future();
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
           boost::thread(func1, boost::move(p)).detach();
#else
           p.set_value(3);
#endif
          BOOST_TEST(f.valid());
          BOOST_TEST(f.get_or(4) == 3);
#ifdef BOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
          BOOST_TEST(!f.valid());
#endif
      }
      BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
      {
          boost::promise<T> p;
          BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
          boost::future<T> f = p.get_future();
          BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          boost::thread(func2, boost::move(p)).detach();
#else
          p.set_exception(::make_exception_ptr(3));
#endif
          BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
          try
          {
            BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
              BOOST_TEST(f.valid());
              BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
              BOOST_TEST(f.get_or(4) == 4);
              BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
          }
          catch (...)
          {
            BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
            BOOST_TEST(false);
          }
#ifdef BOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
          BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
          BOOST_TEST(!f.valid());
#endif
          BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
      }
  }
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
  {
      typedef int& T;
      {
          boost::promise<T> p;
          boost::future<T> f = p.get_future();
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          boost::thread(func3, boost::move(p)).detach();
#else
          int j=5;
          p.set_value(j);
#endif
          BOOST_TEST(f.valid());
          int k=4;
          BOOST_TEST(f.get_or(boost::ref(k)) == 5);
#ifdef BOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
          BOOST_TEST(!f.valid());
#endif
      }
      BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;
      {
          boost::promise<T> p;
          boost::future<T> f = p.get_future();
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          boost::thread(func4, boost::move(p)).detach();
#else
          p.set_exception(::make_exception_ptr(3.5));
#endif
          try
          {
              BOOST_TEST(f.valid());
              int j=4;
              BOOST_TEST(f.get_or(boost::ref(j)) == 4);
          }
          catch (...)
          {
            BOOST_TEST(false);
          }
#ifdef BOOST_THREAD_PROVIDES_FUTURE_INVALID_AFTER_GET
          BOOST_TEST(!f.valid());
#endif
      }
  }
  BOOST_THREAD_LOG << BOOST_THREAD_END_LOG;


  return boost::report_errors();
}

#else
#error "Test not applicable: BOOST_THREAD_USES_CHRONO not defined for this platform as not supported"
#endif
#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/futures/future, get_or_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
