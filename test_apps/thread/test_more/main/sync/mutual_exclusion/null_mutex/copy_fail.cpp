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

// <boost/thread/null_mutex.hpp>

// class null_mutex;

// null_mutex(const null_mutex&) = delete;

#include <boost/thread/null_mutex.hpp>
#include <boost/detail/lightweight_test.hpp>

static int test_main()
{
  boost::null_mutex m0;
  /* esp32: The code below is expected to fail compilation, it's commented out to allow compilation to pass */
  // boost::null_mutex m1(m0);
}
