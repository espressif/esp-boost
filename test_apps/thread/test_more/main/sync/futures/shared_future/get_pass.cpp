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

// <boost/thread/shared_future.hpp>

// class shared_future<R>

// const R& shared_future::get();
// R& shared_future<R&>::get();
// void shared_future<void>::get();
//#define BOOST_THREAD_VERSION 3
#define BOOST_THREAD_VERSION 4

#include <boost/thread/future.hpp>
#include <boost/thread/thread.hpp>
#include <boost/detail/lightweight_test.hpp>

#if defined BOOST_THREAD_USES_CHRONO

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

static int j = 0;

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
  {
      typedef int T;
      {
          boost::promise<T> p;
          boost::shared_future<T> f((p.get_future()));
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
           boost::thread(func1, boost::move(p)).detach();
#else
           p.set_value(3);
#endif
          BOOST_TEST(f.valid());
          BOOST_TEST(f.get() == 3);
          BOOST_TEST(f.valid());
      }
      {
          boost::promise<T> p;
          boost::shared_future<T> f((p.get_future()));
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          boost::thread(func2, boost::move(p)).detach();
#else
          p.set_exception(::make_exception_ptr(3));
#endif
          try
          {
              BOOST_TEST(f.valid());
              BOOST_TEST(f.get() == 3);
              BOOST_TEST(false);
          }
          catch (::wrap<int> const& i)
          {
              BOOST_TEST(i.value == 3);
          }
          catch (...)
          {
              BOOST_TEST(false);
          }
          BOOST_TEST(f.valid());
      }
  }
  {
      typedef int& T;
      {
          boost::promise<T> p;
          boost::shared_future<T> f((p.get_future()));
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          boost::thread(func3, boost::move(p)).detach();
#else
          int j=5;
          p.set_value(j);
#endif
          BOOST_TEST(f.valid());
          BOOST_TEST(f.get() == 5);
          BOOST_TEST(f.valid());
      }
      {
          boost::promise<T> p;
          boost::shared_future<T> f((p.get_future()));
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
          boost::thread(func4, boost::move(p)).detach();
#else
          p.set_exception(::make_exception_ptr(3.5));
#endif
          try
          {
              BOOST_TEST(f.valid());
              BOOST_TEST(f.get() == 3);
              BOOST_TEST(false);
          }
          catch (::wrap<double> const& i)
          {
              BOOST_TEST(i.value == 3.5);
          }
          BOOST_TEST(f.valid());
      }
  }

  typedef void T;
  {
      boost::promise<T> p;
      boost::shared_future<T> f((p.get_future()));
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
       boost::thread(func5, boost::move(p)).detach();
#else
       p.set_value();
#endif
      BOOST_TEST(f.valid());
      f.get();
      BOOST_TEST(f.valid());
  }
  {
      boost::promise<T> p;
      boost::shared_future<T> f((p.get_future()));
#if defined BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK && defined(BOOST_THREAD_PROVIDES_VARIADIC_THREAD)
      boost::thread(func6, boost::move(p)).detach();
#else
      p.set_exception(::make_exception_ptr(4));
#endif
      try
      {
          BOOST_TEST(f.valid());
          f.get();
          BOOST_TEST(false);
      }
      catch (::wrap<int> const& i)
      {
          BOOST_TEST(i.value == 4);
      }
      catch (...)
      {
          BOOST_TEST(false);
      }
      BOOST_TEST(f.valid());
  }

  return boost::report_errors();
}

#else
#error "Test not applicable: BOOST_THREAD_USES_CHRONO not defined for this platform as not supported"
#endif
#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/futures/shared_future, get_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
