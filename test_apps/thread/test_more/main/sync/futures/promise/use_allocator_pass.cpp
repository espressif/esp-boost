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

// class promise<R>

//   promise(allocator_arg_t, const Allocator& a);

#define BOOST_THREAD_VERSION 3

#include <boost/thread/future.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <boost/static_assert.hpp>

#if defined BOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
#include "../test_allocator.hpp"

static int test_main()
{

  BOOST_STATIC_ASSERT_MSG((boost::csbl::uses_allocator<boost::promise<int>, test_allocator<int> >::value), "");
  BOOST_STATIC_ASSERT_MSG((boost::csbl::uses_allocator<boost::promise<int&>, test_allocator<int&> >::value), "");
  BOOST_STATIC_ASSERT_MSG((boost::csbl::uses_allocator<boost::promise<void>, test_allocator<void> >::value), "");

  return boost::report_errors();
}

#else
static int test_main()
{
  return boost::report_errors();
}
#endif

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/futures/promise, use_allocator_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
