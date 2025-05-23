//  (C) Copyright 2008-10 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_THREAD_VERSION 2
#define BOOST_TEST_MODULE Boost.Threads: futures test suite

#include <boost/thread/thread_only.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/future.hpp>
#include <utility>
#include <memory>
#include <string>
#include <iostream>
#include <boost/thread/detail/log.hpp>

// #include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include "common.hpp"

#ifdef BOOST_MSVC
# pragma warning(disable: 4267) // conversion from ... to ..., possible loss of data
#endif

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    template<typename T>
    typename boost::remove_reference<T>::type&& cast_to_rval(T&& t)
    {
        return static_cast<typename boost::remove_reference<T>::type&&>(t);
    }
#else
#if defined BOOST_THREAD_USES_MOVE
    template<typename T>
    boost::rv<T>& cast_to_rval(T& t)
    {
        return boost::move(t);
    }
#else
    template<typename T>
    boost::detail::thread_move_t<T> cast_to_rval(T& t)
    {
        return boost::move(t);
    }
#endif
#endif

struct X
{
public:
    int i;

    BOOST_THREAD_MOVABLE_ONLY(X)
    X():
        i(42)
    {}
    X(BOOST_THREAD_RV_REF(X) other):
        i(BOOST_THREAD_RV(other).i)
    {
      BOOST_THREAD_RV(other).i=0;
    }
    X& operator=(BOOST_THREAD_RV_REF(X) other)
    {
      i=BOOST_THREAD_RV(other).i;
      BOOST_THREAD_RV(other).i=0;
      return *this;
    }
    ~X()
    {}
};
namespace boost {
  BOOST_THREAD_DCL_MOVABLE(X)
}

int make_int()
{
    return 42;
}

int throw_runtime_error()
{
    throw std::runtime_error("42");
}

void set_promise_thread(boost::promise<int>* p)
{
    p->set_value(42);
}

struct my_exception
{};

void set_promise_exception_thread(boost::promise<int>* p)
{
    p->set_exception(boost::copy_exception(my_exception()));
}


BOOST_AUTO_TEST_CASE(test_store_value_from_thread)
{
    BOOST_DETAIL_THREAD_LOG;
    try {
    boost::promise<int> pi2;
    BOOST_DETAIL_THREAD_LOG;
    boost::unique_future<int> fi2(BOOST_THREAD_MAKE_RV_REF(pi2.get_future()));
    BOOST_DETAIL_THREAD_LOG;
    boost::thread(set_promise_thread,&pi2);
    BOOST_DETAIL_THREAD_LOG;
    int j=fi2.get();
    BOOST_DETAIL_THREAD_LOG;
    BOOST_CHECK(j==42);
    BOOST_DETAIL_THREAD_LOG;
    BOOST_CHECK(fi2.is_ready());
    BOOST_DETAIL_THREAD_LOG;
    BOOST_CHECK(fi2.has_value());
    BOOST_DETAIL_THREAD_LOG;
    BOOST_CHECK(!fi2.has_exception());
    BOOST_DETAIL_THREAD_LOG;
    BOOST_CHECK(fi2.get_state()==boost::future_state::ready);
    BOOST_DETAIL_THREAD_LOG;
    }
    catch (...)
    {
    BOOST_CHECK(false&&"Exception thrown");
    }
}

BOOST_AUTO_TEST_CASE(test_store_exception)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<int> pi3;
    boost::unique_future<int> fi3(BOOST_THREAD_MAKE_RV_REF(pi3.get_future()));
    boost::thread(set_promise_exception_thread,&pi3);
    try
    {
        fi3.get();
        BOOST_CHECK(false);
    }
    catch(my_exception)
    {
        BOOST_CHECK(true);
    }

    BOOST_CHECK(fi3.is_ready());
    BOOST_CHECK(!fi3.has_value());
    BOOST_CHECK(fi3.has_exception());
    BOOST_CHECK(fi3.get_state()==boost::future_state::ready);
}

BOOST_AUTO_TEST_CASE(test_initial_state)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::unique_future<int> fi;
    BOOST_CHECK(!fi.is_ready());
    BOOST_CHECK(!fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::uninitialized);
    int i;
    try
    {
        i=fi.get();
        (void)i;
        BOOST_CHECK(false);
    }
    catch(boost::future_uninitialized)
    {
        BOOST_CHECK(true);
    }
}

BOOST_AUTO_TEST_CASE(test_waiting_future)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<int> pi;
    boost::unique_future<int> fi;
    fi=BOOST_THREAD_MAKE_RV_REF(pi.get_future());

    int i=0;
    BOOST_CHECK(!fi.is_ready());
    BOOST_CHECK(!fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::waiting);
    BOOST_CHECK(i==0);
}

BOOST_AUTO_TEST_CASE(test_cannot_get_future_twice)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<int> pi;
    BOOST_THREAD_MAKE_RV_REF(pi.get_future());

    try
    {
        pi.get_future();
        BOOST_CHECK(false);
    }
    catch(boost::future_already_retrieved&)
    {
        BOOST_CHECK(true);
    }
}

BOOST_AUTO_TEST_CASE(test_set_value_updates_future_state)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<int> pi;
    boost::unique_future<int> fi;
    fi=BOOST_THREAD_MAKE_RV_REF(pi.get_future());

    pi.set_value(42);

    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::ready);
}

BOOST_AUTO_TEST_CASE(test_set_value_can_be_retrieved)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<int> pi;
    boost::unique_future<int> fi;
    fi=BOOST_THREAD_MAKE_RV_REF(pi.get_future());

    pi.set_value(42);

    int i=0;
    BOOST_CHECK(i=fi.get());
    BOOST_CHECK(i==42);
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::ready);
}

BOOST_AUTO_TEST_CASE(test_set_value_can_be_moved)
{
    BOOST_DETAIL_THREAD_LOG;
//     boost::promise<int> pi;
//     boost::unique_future<int> fi;
//     fi=BOOST_THREAD_MAKE_RV_REF(pi.get_future());

//     pi.set_value(42);

//     int i=0;
//     BOOST_CHECK(i=fi.get());
//     BOOST_CHECK(i==42);
//     BOOST_CHECK(fi.is_ready());
//     BOOST_CHECK(fi.has_value());
//     BOOST_CHECK(!fi.has_exception());
//     BOOST_CHECK(fi.get_state()==boost::future_state::ready);
}

BOOST_AUTO_TEST_CASE(test_future_from_packaged_task_is_waiting)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int);
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    int i=0;
    BOOST_CHECK(!fi.is_ready());
    BOOST_CHECK(!fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::waiting);
    BOOST_CHECK(i==0);
}

BOOST_AUTO_TEST_CASE(test_invoking_a_packaged_task_populates_future)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int);
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    pt();

    int i=0;
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::ready);
    BOOST_CHECK(i=fi.get());
    BOOST_CHECK(i==42);
}

BOOST_AUTO_TEST_CASE(test_invoking_a_packaged_task_twice_throws)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int);

    pt();
    try
    {
        pt();
        BOOST_CHECK(false);
    }
    catch(boost::task_already_started)
    {
        BOOST_CHECK(true);
    }
}


BOOST_AUTO_TEST_CASE(test_cannot_get_future_twice_from_task)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int);
    pt.get_future();
    try
    {
        pt.get_future();
        BOOST_CHECK(false);
    }
    catch(boost::future_already_retrieved)
    {
        BOOST_CHECK(true);
    }
}

BOOST_AUTO_TEST_CASE(test_task_stores_exception_if_function_throws)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(throw_runtime_error);
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    pt();

    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(!fi.has_value());
    BOOST_CHECK(fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::ready);
    try
    {
        fi.get();
        BOOST_CHECK(false);
    }
    catch(std::exception&)
    {
        BOOST_CHECK(true);
    }
    catch(...)
    {
        BOOST_CHECK(!"Unknown exception thrown");
    }

}

BOOST_AUTO_TEST_CASE(test_void_promise)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<void> p;
    boost::unique_future<void> f(BOOST_THREAD_MAKE_RV_REF(p.get_future()));
    p.set_value();
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_value());
    BOOST_CHECK(!f.has_exception());
    BOOST_CHECK(f.get_state()==boost::future_state::ready);
    f.get();
}

BOOST_AUTO_TEST_CASE(test_reference_promise)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<int&> p;
    boost::unique_future<int&> f(BOOST_THREAD_MAKE_RV_REF(p.get_future()));
    int i=42;
    p.set_value(i);
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_value());
    BOOST_CHECK(!f.has_exception());
    BOOST_CHECK(f.get_state()==boost::future_state::ready);
    BOOST_CHECK(&f.get()==&i);
}

static void do_nothing()
{}

BOOST_AUTO_TEST_CASE(test_task_returning_void)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<void> pt(do_nothing);
    boost::unique_future<void> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    pt();

    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::ready);
}

int global_ref_target=0;

int& return_ref()
{
    return global_ref_target;
}

BOOST_AUTO_TEST_CASE(test_task_returning_reference)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int&> pt(return_ref);
    boost::unique_future<int&> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    pt();

    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(fi.has_value());
    BOOST_CHECK(!fi.has_exception());
    BOOST_CHECK(fi.get_state()==boost::future_state::ready);
    int& i=fi.get();
    BOOST_CHECK(&i==&global_ref_target);
}

BOOST_AUTO_TEST_CASE(test_shared_future)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int);
    boost::unique_future<int> fi=pt.get_future();

    boost::shared_future<int> sf(::cast_to_rval(fi));
    BOOST_CHECK(fi.get_state()==boost::future_state::uninitialized);

    pt();

    int i=0;
    BOOST_CHECK(sf.is_ready());
    BOOST_CHECK(sf.has_value());
    BOOST_CHECK(!sf.has_exception());
    BOOST_CHECK(sf.get_state()==boost::future_state::ready);
    BOOST_CHECK(i=sf.get());
    BOOST_CHECK(i==42);
}

BOOST_AUTO_TEST_CASE(test_copies_of_shared_future_become_ready_together)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int);
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    boost::shared_future<int> sf(::cast_to_rval(fi));
    boost::shared_future<int> sf2(sf);
    boost::shared_future<int> sf3;
    sf3=sf;
    BOOST_CHECK(sf.get_state()==boost::future_state::waiting);
    BOOST_CHECK(sf2.get_state()==boost::future_state::waiting);
    BOOST_CHECK(sf3.get_state()==boost::future_state::waiting);

    pt();

    int i=0;
    BOOST_CHECK(sf.is_ready());
    BOOST_CHECK(sf.has_value());
    BOOST_CHECK(!sf.has_exception());
    BOOST_CHECK(sf.get_state()==boost::future_state::ready);
    BOOST_CHECK(i=sf.get());
    BOOST_CHECK(i==42);
    i=0;
    BOOST_CHECK(sf2.is_ready());
    BOOST_CHECK(sf2.has_value());
    BOOST_CHECK(!sf2.has_exception());
    BOOST_CHECK(sf2.get_state()==boost::future_state::ready);
    BOOST_CHECK(i=sf2.get());
    BOOST_CHECK(i==42);
    i=0;
    BOOST_CHECK(sf3.is_ready());
    BOOST_CHECK(sf3.has_value());
    BOOST_CHECK(!sf3.has_exception());
    BOOST_CHECK(sf3.get_state()==boost::future_state::ready);
    BOOST_CHECK(i=sf3.get());
    BOOST_CHECK(i==42);
}

BOOST_AUTO_TEST_CASE(test_shared_future_can_be_move_assigned_from_unique_future)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int);
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    boost::shared_future<int> sf;
    sf=::cast_to_rval(fi);
    BOOST_CHECK(fi.get_state()==boost::future_state::uninitialized);

    BOOST_CHECK(!sf.is_ready());
    BOOST_CHECK(!sf.has_value());
    BOOST_CHECK(!sf.has_exception());
    BOOST_CHECK(sf.get_state()==boost::future_state::waiting);
}

BOOST_AUTO_TEST_CASE(test_shared_future_void)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<void> pt(do_nothing);
    boost::unique_future<void> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    boost::shared_future<void> sf(::cast_to_rval(fi));
    BOOST_CHECK(fi.get_state()==boost::future_state::uninitialized);

    pt();

    BOOST_CHECK(sf.is_ready());
    BOOST_CHECK(sf.has_value());
    BOOST_CHECK(!sf.has_exception());
    BOOST_CHECK(sf.get_state()==boost::future_state::ready);
    sf.get();
}

BOOST_AUTO_TEST_CASE(test_shared_future_ref)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<int&> p;
    boost::shared_future<int&> f(BOOST_THREAD_MAKE_RV_REF(p.get_future()));
    int i=42;
    p.set_value(i);
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_value());
    BOOST_CHECK(!f.has_exception());
    BOOST_CHECK(f.get_state()==boost::future_state::ready);
    BOOST_CHECK(&f.get()==&i);
}

BOOST_AUTO_TEST_CASE(test_can_get_a_second_future_from_a_moved_promise)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<int> pi;
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pi.get_future()));

    boost::promise<int> pi2(::cast_to_rval(pi));
    boost::unique_future<int> fi2(BOOST_THREAD_MAKE_RV_REF(pi.get_future()));

    pi2.set_value(3);
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(!fi2.is_ready());
    BOOST_CHECK(fi.get()==3);
    pi.set_value(42);
    BOOST_CHECK(fi2.is_ready());
    BOOST_CHECK(fi2.get()==42);
}

BOOST_AUTO_TEST_CASE(test_can_get_a_second_future_from_a_moved_void_promise)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<void> pi;
    boost::unique_future<void> fi(BOOST_THREAD_MAKE_RV_REF(pi.get_future()));

    boost::promise<void> pi2(::cast_to_rval(pi));
    boost::unique_future<void> fi2(BOOST_THREAD_MAKE_RV_REF(pi.get_future()));

    pi2.set_value();
    BOOST_CHECK(fi.is_ready());
    BOOST_CHECK(!fi2.is_ready());
    pi.set_value();
    BOOST_CHECK(fi2.is_ready());
}

BOOST_AUTO_TEST_CASE(test_unique_future_for_move_only_udt)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<X> pt;
    boost::unique_future<X> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    pt.set_value(X());
    X res(fi.get());
    BOOST_CHECK(res.i==42);
}

BOOST_AUTO_TEST_CASE(test_unique_future_for_string)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::promise<std::string> pt;
    boost::unique_future<std::string> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    pt.set_value(std::string("hello"));
    std::string res(fi.get());
    BOOST_CHECK(res=="hello");

    boost::promise<std::string> pt2;
    fi=BOOST_THREAD_MAKE_RV_REF(pt2.get_future());

    std::string const s="goodbye";

    pt2.set_value(s);
    res=fi.get();
    BOOST_CHECK(res=="goodbye");

    boost::promise<std::string> pt3;
    fi=BOOST_THREAD_MAKE_RV_REF(pt3.get_future());

    std::string s2="foo";

    pt3.set_value(s2);
    res=fi.get();
    BOOST_CHECK(res=="foo");
}

boost::mutex callback_mutex;
unsigned callback_called=0;

void wait_callback(boost::promise<int>& pi)
{
    std::cout << "wait_callback" << std::endl;
    boost::lock_guard<boost::mutex> lk(callback_mutex);
    ++callback_called;
    try
    {
        pi.set_value(42);
    }
    catch(...)
    {
    }
}

void do_nothing_callback(boost::promise<int>& /*pi*/)
{
    boost::lock_guard<boost::mutex> lk(callback_mutex);
    ++callback_called;
}

/* esp32: Using `TEST_CASE` causes the program to get stuck, but normal program execution works fine, temporarily disabled here */
// BOOST_AUTO_TEST_CASE(test_wait_callback)
// {
//     BOOST_DETAIL_THREAD_LOG;
//     callback_called=0;
//     boost::promise<int> pi;
//     boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pi.get_future()));
//     pi.set_wait_callback(wait_callback);
//     fi.wait();
//     BOOST_CHECK(callback_called);
//     BOOST_CHECK(fi.get()==42);
//     fi.wait();
//     fi.wait();
//     BOOST_CHECK(callback_called==1);
// }

BOOST_AUTO_TEST_CASE(test_wait_callback_with_timed_wait)
{
    BOOST_DETAIL_THREAD_LOG;
    callback_called=0;
    boost::promise<int> pi;
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pi.get_future()));
    pi.set_wait_callback(do_nothing_callback);
    bool success=fi.timed_wait(boost::posix_time::milliseconds(10));
    BOOST_CHECK(callback_called);
    BOOST_CHECK(!success);
    success=fi.timed_wait(boost::posix_time::milliseconds(10));
    BOOST_CHECK(!success);
    success=fi.timed_wait(boost::posix_time::milliseconds(10));
    BOOST_CHECK(!success);
    BOOST_CHECK(callback_called==3);
    pi.set_value(42);
    success=fi.timed_wait(boost::posix_time::milliseconds(10));
    BOOST_CHECK(success);
    BOOST_CHECK(callback_called==3);
    BOOST_CHECK(fi.get()==42);
    BOOST_CHECK(callback_called==3);
}


void wait_callback_for_task(boost::packaged_task<int>& pt)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::lock_guard<boost::mutex> lk(callback_mutex);
    ++callback_called;
    try
    {
        pt();
    }
    catch(...)
    {
    }
}


BOOST_AUTO_TEST_CASE(test_wait_callback_for_packaged_task)
{
    BOOST_DETAIL_THREAD_LOG;
    callback_called=0;
    boost::packaged_task<int> pt(make_int);
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    pt.set_wait_callback(wait_callback_for_task);
    fi.wait();
    BOOST_CHECK(callback_called);
    BOOST_CHECK(fi.get()==42);
    fi.wait();
    fi.wait();
    BOOST_CHECK(callback_called==1);
}

BOOST_AUTO_TEST_CASE(test_packaged_task_can_be_moved)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int);

    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));

    BOOST_CHECK(!fi.is_ready());

    boost::packaged_task<int> pt2(::cast_to_rval(pt));

    BOOST_CHECK(!fi.is_ready());
    try
    {
        pt();
        BOOST_CHECK(!"Can invoke moved task!");
    }
    catch(boost::task_moved&)
    {
    }

    BOOST_CHECK(!fi.is_ready());

    pt2();

    BOOST_CHECK(fi.is_ready());
}

BOOST_AUTO_TEST_CASE(test_destroying_a_promise_stores_broken_promise)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::unique_future<int> f;

    {
        boost::promise<int> p;
        f=BOOST_THREAD_MAKE_RV_REF(p.get_future());
    }
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_exception());
    try
    {
        f.get();
    }
    catch(boost::broken_promise&)
    {
    }
}

BOOST_AUTO_TEST_CASE(test_destroying_a_packaged_task_stores_broken_promise)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::unique_future<int> f;

    {
        boost::packaged_task<int> p(make_int);
        f=BOOST_THREAD_MAKE_RV_REF(p.get_future());
    }
    BOOST_CHECK(f.is_ready());
    BOOST_CHECK(f.has_exception());
    try
    {
        f.get();
    }
    catch(boost::broken_promise&)
    {
    }
}

int make_int_slowly()
{
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    return 42;
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_two_futures_1)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));

    boost::thread(::cast_to_rval(pt));

    unsigned const future=boost::wait_for_any(f1,f2);

    BOOST_CHECK(future==0);
    BOOST_CHECK(f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(f1.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_two_futures_2)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));

    boost::thread(::cast_to_rval(pt2));

    unsigned const future=boost::wait_for_any(f1,f2);

    BOOST_CHECK(future==1);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(f2.is_ready());
    BOOST_CHECK(f2.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_three_futures_1)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));

    boost::thread(::cast_to_rval(pt));

    unsigned const future=boost::wait_for_any(f1,f2,f3);

    BOOST_CHECK(future==0);
    BOOST_CHECK(f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(f1.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_three_futures_2)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));

    boost::thread(::cast_to_rval(pt2));

    unsigned const future=boost::wait_for_any(f1,f2,f3);

    BOOST_CHECK(future==1);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(f2.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_three_futures_3)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));

    boost::thread(::cast_to_rval(pt3));

    unsigned const future=boost::wait_for_any(f1,f2,f3);

    BOOST_CHECK(future==2);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(f3.is_ready());
    BOOST_CHECK(f3.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_four_futures_1)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));

    boost::thread(::cast_to_rval(pt));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4);

    BOOST_CHECK(future==0);
    BOOST_CHECK(f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(!f4.is_ready());
    BOOST_CHECK(f1.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_four_futures_2)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));

    boost::thread(::cast_to_rval(pt2));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4);

    BOOST_CHECK(future==1);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(!f4.is_ready());
    BOOST_CHECK(f2.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_four_futures_3)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));

    boost::thread(::cast_to_rval(pt3));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4);

    BOOST_CHECK(future==2);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(f3.is_ready());
    BOOST_CHECK(!f4.is_ready());
    BOOST_CHECK(f3.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_four_futures_4)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));

    boost::thread(::cast_to_rval(pt4));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4);

    BOOST_CHECK(future==3);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(f4.is_ready());
    BOOST_CHECK(f4.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_five_futures_1)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));
    boost::packaged_task<int> pt5(make_int_slowly);
    boost::unique_future<int> f5(BOOST_THREAD_MAKE_RV_REF(pt5.get_future()));

    boost::thread(::cast_to_rval(pt));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4,f5);

    BOOST_CHECK(future==0);
    BOOST_CHECK(f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(!f4.is_ready());
    BOOST_CHECK(!f5.is_ready());
    BOOST_CHECK(f1.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_of_five_futures_2)
{
    BOOST_DETAIL_THREAD_LOG;
   boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));
    boost::packaged_task<int> pt5(make_int_slowly);
    boost::unique_future<int> f5(BOOST_THREAD_MAKE_RV_REF(pt5.get_future()));

    boost::thread(::cast_to_rval(pt2));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4,f5);

    BOOST_CHECK(future==1);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(!f4.is_ready());
    BOOST_CHECK(!f5.is_ready());
    BOOST_CHECK(f2.get()==42);
}
BOOST_AUTO_TEST_CASE(test_wait_for_either_of_five_futures_3)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));
    boost::packaged_task<int> pt5(make_int_slowly);
    boost::unique_future<int> f5(BOOST_THREAD_MAKE_RV_REF(pt5.get_future()));

    boost::thread(::cast_to_rval(pt3));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4,f5);

    BOOST_CHECK(future==2);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(f3.is_ready());
    BOOST_CHECK(!f4.is_ready());
    BOOST_CHECK(!f5.is_ready());
    BOOST_CHECK(f3.get()==42);
}
BOOST_AUTO_TEST_CASE(test_wait_for_either_of_five_futures_4)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));
    boost::packaged_task<int> pt5(make_int_slowly);
    boost::unique_future<int> f5(BOOST_THREAD_MAKE_RV_REF(pt5.get_future()));

    boost::thread(::cast_to_rval(pt4));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4,f5);

    BOOST_CHECK(future==3);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(f4.is_ready());
    BOOST_CHECK(!f5.is_ready());
    BOOST_CHECK(f4.get()==42);
}
BOOST_AUTO_TEST_CASE(test_wait_for_either_of_five_futures_5)
{
    BOOST_DETAIL_THREAD_LOG;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> f1(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> f2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    boost::packaged_task<int> pt3(make_int_slowly);
    boost::unique_future<int> f3(BOOST_THREAD_MAKE_RV_REF(pt3.get_future()));
    boost::packaged_task<int> pt4(make_int_slowly);
    boost::unique_future<int> f4(BOOST_THREAD_MAKE_RV_REF(pt4.get_future()));
    boost::packaged_task<int> pt5(make_int_slowly);
    boost::unique_future<int> f5(BOOST_THREAD_MAKE_RV_REF(pt5.get_future()));

    boost::thread(::cast_to_rval(pt5));

    unsigned const future=boost::wait_for_any(f1,f2,f3,f4,f5);

    BOOST_CHECK(future==4);
    BOOST_CHECK(!f1.is_ready());
    BOOST_CHECK(!f2.is_ready());
    BOOST_CHECK(!f3.is_ready());
    BOOST_CHECK(!f4.is_ready());
    BOOST_CHECK(f5.is_ready());
    BOOST_CHECK(f5.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_either_invokes_callbacks)
{
    BOOST_DETAIL_THREAD_LOG;
    callback_called=0;
    boost::packaged_task<int> pt(make_int_slowly);
    boost::unique_future<int> fi(BOOST_THREAD_MAKE_RV_REF(pt.get_future()));
    boost::packaged_task<int> pt2(make_int_slowly);
    boost::unique_future<int> fi2(BOOST_THREAD_MAKE_RV_REF(pt2.get_future()));
    pt.set_wait_callback(wait_callback_for_task);

    boost::thread(::cast_to_rval(pt));

    boost::wait_for_any(fi,fi2);
    BOOST_CHECK(callback_called==1);
    BOOST_CHECK(fi.get()==42);
}

BOOST_AUTO_TEST_CASE(test_wait_for_any_from_range)
{
    BOOST_DETAIL_THREAD_LOG;
    unsigned const count=10;
    for(unsigned i=0;i<count;++i)
    {
        boost::packaged_task<int> tasks[count];
        boost::unique_future<int> futures[count];
        for(unsigned j=0;j<count;++j)
        {
            tasks[j]=boost::packaged_task<int>(make_int_slowly);
            futures[j]=BOOST_THREAD_MAKE_RV_REF(tasks[j].get_future());
        }
        boost::thread(::cast_to_rval(tasks[i]));

        BOOST_CHECK(boost::wait_for_any(futures,futures)==futures);

        boost::unique_future<int>* const future=boost::wait_for_any(futures,futures+count);

        BOOST_CHECK(future==(futures+i));
        for(unsigned j=0;j<count;++j)
        {
            if(j!=i)
            {
                BOOST_CHECK(!futures[j].is_ready());
            }
            else
            {
                BOOST_CHECK(futures[j].is_ready());
            }
        }
        BOOST_CHECK(futures[i].get()==42);
    }
}

BOOST_AUTO_TEST_CASE(test_wait_for_all_from_range)
{
    BOOST_DETAIL_THREAD_LOG;
    unsigned const count=10;
    boost::unique_future<int> futures[count];
    for(unsigned j=0;j<count;++j)
    {
        boost::packaged_task<int> task(make_int_slowly);
        futures[j]=BOOST_THREAD_MAKE_RV_REF(task.get_future());
        boost::thread(::cast_to_rval(task));
    }

    boost::wait_for_all(futures,futures+count);

    for(unsigned j=0;j<count;++j)
    {
        BOOST_CHECK(futures[j].is_ready());
    }
}

BOOST_AUTO_TEST_CASE(test_wait_for_all_two_futures)
{
    BOOST_DETAIL_THREAD_LOG;
    unsigned const count=2;
    boost::unique_future<int> futures[count];
    for(unsigned j=0;j<count;++j)
    {
        boost::packaged_task<int> task(make_int_slowly);
        futures[j]=BOOST_THREAD_MAKE_RV_REF(task.get_future());
        boost::thread(::cast_to_rval(task));
    }

    boost::wait_for_all(futures[0],futures[1]);

    for(unsigned j=0;j<count;++j)
    {
        BOOST_CHECK(futures[j].is_ready());
    }
}

BOOST_AUTO_TEST_CASE(test_wait_for_all_three_futures)
{
    BOOST_DETAIL_THREAD_LOG;
    unsigned const count=3;
    boost::unique_future<int> futures[count];
    for(unsigned j=0;j<count;++j)
    {
        boost::packaged_task<int> task(make_int_slowly);
        futures[j]=BOOST_THREAD_MAKE_RV_REF(task.get_future());
        boost::thread(::cast_to_rval(task));
    }

    boost::wait_for_all(futures[0],futures[1],futures[2]);

    for(unsigned j=0;j<count;++j)
    {
        BOOST_CHECK(futures[j].is_ready());
    }
}

BOOST_AUTO_TEST_CASE(test_wait_for_all_four_futures)
{
    BOOST_DETAIL_THREAD_LOG;
    unsigned const count=4;
    boost::unique_future<int> futures[count];
    for(unsigned j=0;j<count;++j)
    {
        boost::packaged_task<int> task(make_int_slowly);
        futures[j]=BOOST_THREAD_MAKE_RV_REF(task.get_future());
        boost::thread(::cast_to_rval(task));
    }

    boost::wait_for_all(futures[0],futures[1],futures[2],futures[3]);

    for(unsigned j=0;j<count;++j)
    {
        BOOST_CHECK(futures[j].is_ready());
    }
}

BOOST_AUTO_TEST_CASE(test_wait_for_all_five_futures)
{
    BOOST_DETAIL_THREAD_LOG;
    unsigned const count=5;
    boost::unique_future<int> futures[count];
    for(unsigned j=0;j<count;++j)
    {
        boost::packaged_task<int> task(make_int_slowly);
        futures[j]=BOOST_THREAD_MAKE_RV_REF(task.get_future());
        boost::thread(::cast_to_rval(task));
    }

    boost::wait_for_all(futures[0],futures[1],futures[2],futures[3],futures[4]);

    for(unsigned j=0;j<count;++j)
    {
        BOOST_CHECK(futures[j].is_ready());
    }
}
