// Copyright (C) 2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/mutex.hpp>

// <mutex>

// template <class Mutex>
// class strict_lock
// {
// public:
//     typedef Mutex mutex_type;
//     ...
// };


#include <boost/thread/strict_lock.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/detail/lightweight_test.hpp>

static int test_main()
{
  BOOST_STATIC_ASSERT_MSG((boost::is_same<boost::strict_lock<boost::mutex>::mutex_type,
      boost::mutex>::value), "");

  BOOST_STATIC_ASSERT_MSG((boost::is_strict_lock<boost::strict_lock<boost::mutex> >::value), "");

  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/locks/strict_lock, types_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
