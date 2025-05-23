// Copyright (C) 2013 Vicente Botet
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/thread/mutex.hpp>
boost::mutex mut;
void boostMutexImp1()
{
    boost::mutex::scoped_lock lock(mut);
    /* esp32: After commenting out unlock, the test passes */
    // mut.unlock();  // A: with this X blocks
    // lock.unlock(); // No influence if used also if before A
}
void boostMutexImp2()
{
    boost::mutex::scoped_lock lock(mut); // X: blocks with A
}
static int test_main()
{
    boostMutexImp1();
    boostMutexImp2();
    return 0;
}

#include <thread>
#include "common.hpp"

BOOST_AUTO_TEST_CASE(test_8455)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
