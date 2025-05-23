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

// <boost/thread/timed_mutex.hpp>

// class timed_mutex;

// timed_mutex(const timed_mutex&) = delete;

#include <boost/thread/mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

static int test_main()
{
  boost::timed_mutex m0;
  /* esp32: The code below is expected to fail compilation, it's commented out to allow compilation to pass */
  // boost::timed_mutex m1(m0);
}
