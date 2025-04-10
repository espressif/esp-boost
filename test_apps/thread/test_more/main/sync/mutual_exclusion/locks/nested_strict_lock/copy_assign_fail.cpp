// Copyright (C) 2011 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/locks.hpp>

// template <class Mutex> class nested_strict_lock;

// nested_strict_lock& operator=(nested_strict_lock const&) = delete;

#include <boost/thread/lock_types.hpp>
#include <boost/thread/strict_lock.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

static boost::mutex m0;
static boost::mutex m1;

static int test_main()
{
  boost::unique_lock<boost::mutex> lk0(m0);
  boost::unique_lock<boost::mutex> lk1(m1);
  boost::nested_strict_lock<boost::unique_lock<boost::mutex> > nlk0(lk0);
  boost::nested_strict_lock<boost::unique_lock<boost::mutex> > nlk1(lk1);
  /* esp32: The code below is expected to fail compilation, it's commented out to allow compilation to pass */
  // lk1 = lk0;

}
