// Copyright (C) 2013,2015 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/concurrent_queues/sync_deque.hpp>

// class sync_deque<T>

//    sync_deque();

#define BOOST_THREAD_VERSION 4

#include <boost/thread/concurrent_queues/sync_deque.hpp>

#include <boost/detail/lightweight_test.hpp>

class non_copyable
{
  BOOST_THREAD_MOVABLE_ONLY(non_copyable)
  int val;
public:
  non_copyable(int v) : val(v){}
  non_copyable(BOOST_RV_REF(non_copyable) x): val(x.val) {}
  non_copyable& operator=(BOOST_RV_REF(non_copyable) x) { val=x.val; return *this; }
  bool operator==(non_copyable const& x) const {return val==x.val;}
  template <typename OSTREAM>
  friend OSTREAM& operator <<(OSTREAM& os, non_copyable const&x )
  {
    os << x.val;
    return os;
  }

};



static int test_main()
{

  {
    // default queue invariants
      boost::sync_deque<int> q;
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // empty queue try_pull fails
      boost::sync_deque<int> q;
      int i;
      BOOST_TEST( boost::queue_op_status::empty == q.try_pull_front(i));
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // empty queue push rvalue/copyable succeeds
      boost::sync_deque<int> q;
      q.push_back(1);
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }
  {
    // empty queue push lvalue/copyable succeeds
      boost::sync_deque<int> q;
      int i;
      q.push_back(i);
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
  {
    // empty queue push rvalue/non_copyable succeeds
      boost::sync_deque<non_copyable> q;
      q.push_back(non_copyable(1));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }
#endif
  {
    // empty queue push rvalue/non_copyable succeeds
      boost::sync_deque<non_copyable> q;
      non_copyable nc(1);
      q.push_back(boost::move(nc));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }

  {
    // empty queue push rvalue succeeds
      boost::sync_deque<int> q;
      q.push_back(1);
      q.push_back(2);
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 2u);
      BOOST_TEST(! q.closed());
  }
  {
    // empty queue push lvalue succeeds
      boost::sync_deque<int> q;
      int i;
      q.push_back(i);
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }
  {
    // empty queue try_push rvalue/copyable succeeds
      boost::sync_deque<int> q;
      BOOST_TEST(boost::queue_op_status::success == q.try_push_back(1));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }
  {
    // empty queue try_push rvalue/copyable succeeds
      boost::sync_deque<int> q;
      BOOST_TEST(boost::queue_op_status::success == q.try_push_back(1));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
  {
    // empty queue try_push rvalue/non-copyable succeeds
      boost::sync_deque<non_copyable> q;
      BOOST_TEST(boost::queue_op_status::success ==q.try_push_back(non_copyable(1)));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }
#endif
  {
    // empty queue try_push rvalue/non-copyable succeeds
      boost::sync_deque<non_copyable> q;
      non_copyable nc(1);
      BOOST_TEST(boost::queue_op_status::success == q.try_push_back(boost::move(nc)));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }

  {
    // empty queue try_push lvalue succeeds
      boost::sync_deque<int> q;
      int i=1;
      BOOST_TEST(boost::queue_op_status::success == q.try_push_back(i));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }
  {
    // empty queue try_push rvalue succeeds
      boost::sync_deque<int> q;
      BOOST_TEST(boost::queue_op_status::success == q.nonblocking_push_back(1));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
  {
    // empty queue nonblocking_push_back rvalue/non-copyable succeeds
      boost::sync_deque<non_copyable> q;
      BOOST_TEST(boost::queue_op_status::success == q.nonblocking_push_back(non_copyable(1)));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }
#endif
  {
    // empty queue nonblocking_push_back rvalue/non-copyable succeeds
      boost::sync_deque<non_copyable> q;
      non_copyable nc(1);
      BOOST_TEST(boost::queue_op_status::success == q.nonblocking_push_back(boost::move(nc)));
      BOOST_TEST(! q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 1u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue pull_front succeed
      boost::sync_deque<int> q;
      q.push_back(1);
      int i;
      q.pull_front(i);
      BOOST_TEST_EQ(i, 1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue pull_front succeed
      boost::sync_deque<non_copyable> q;
      non_copyable nc1(1);
      q.push_back(boost::move(nc1));
      non_copyable nc2(2);
      q.pull_front(nc2);
      BOOST_TEST_EQ(nc1, nc2);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue pull_front succeed
      boost::sync_deque<int> q;
      q.push_back(1);
      int i = q.pull_front();
      BOOST_TEST_EQ(i, 1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue pull_front succeed
      boost::sync_deque<non_copyable> q;
      non_copyable nc1(1);
      q.push_back(boost::move(nc1));
      non_copyable nc = q.pull_front();
      BOOST_TEST_EQ(nc, nc1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue try_pull_front succeed
      boost::sync_deque<int> q;
      q.push_back(1);
      int i;
      BOOST_TEST(boost::queue_op_status::success == q.try_pull_front(i));
      BOOST_TEST_EQ(i, 1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue try_pull_front succeed
      boost::sync_deque<non_copyable> q;
      non_copyable nc1(1);
      q.push_back(boost::move(nc1));
      non_copyable nc(2);
      BOOST_TEST(boost::queue_op_status::success == q.try_pull_front(nc));
      BOOST_TEST_EQ(nc, nc1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue nonblocking_pull_front succeed
      boost::sync_deque<int> q;
      q.push_back(1);
      int i;
      BOOST_TEST(boost::queue_op_status::success == q.nonblocking_pull_front(i));
      BOOST_TEST_EQ(i, 1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue nonblocking_pull_front succeed
      boost::sync_deque<non_copyable> q;
      non_copyable nc1(1);
      q.push_back(boost::move(nc1));
      non_copyable nc(2);
      BOOST_TEST(boost::queue_op_status::success == q.nonblocking_pull_front(nc));
      BOOST_TEST_EQ(nc, nc1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue wait_pull_front succeed
      boost::sync_deque<non_copyable> q;
      non_copyable nc1(1);
      q.push_back(boost::move(nc1));
      non_copyable nc(2);
      BOOST_TEST(boost::queue_op_status::success == q.wait_pull_front(nc));
      BOOST_TEST_EQ(nc, nc1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue wait_pull_front succeed
      boost::sync_deque<int> q;
      q.push_back(1);
      int i;
      BOOST_TEST(boost::queue_op_status::success == q.wait_pull_front(i));
      BOOST_TEST_EQ(i, 1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }
  {
    // 1-element queue wait_pull_front succeed
      boost::sync_deque<non_copyable> q;
      non_copyable nc1(1);
      q.push_back(boost::move(nc1));
      non_copyable nc(2);
      BOOST_TEST(boost::queue_op_status::success == q.wait_pull_front(nc));
      BOOST_TEST_EQ(nc, nc1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(! q.closed());
  }

  {
    // closed invariants
      boost::sync_deque<int> q;
      q.close();
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(q.closed());
  }
  {
    // closed queue push fails
      boost::sync_deque<int> q;
      q.close();
      try {
        q.push_back(1);
        BOOST_TEST(false);
      } catch (...) {
        BOOST_TEST(q.empty());
        BOOST_TEST(! q.full());
        BOOST_TEST_EQ(q.size(), 0u);
        BOOST_TEST(q.closed());
      }
  }
  {
    // 1-element closed queue pull succeed
      boost::sync_deque<int> q;
      q.push_back(1);
      q.close();
      int i;
      q.pull_front(i);
      BOOST_TEST_EQ(i, 1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(q.closed());
  }
  {
    // 1-element closed queue wait_pull_front succeed
      boost::sync_deque<int> q;
      q.push_back(1);
      q.close();
      int i;
      BOOST_TEST(boost::queue_op_status::success == q.wait_pull_front(i));
      BOOST_TEST_EQ(i, 1);
      BOOST_TEST(q.empty());
      BOOST_TEST(! q.full());
      BOOST_TEST_EQ(q.size(), 0u);
      BOOST_TEST(q.closed());
  }
  {
    // closed empty queue wait_pull_front fails
      boost::sync_deque<int> q;
      q.close();
      BOOST_TEST(q.empty());
      BOOST_TEST(q.closed());
      int i;
      BOOST_TEST(boost::queue_op_status::closed == q.wait_pull_front(i));
      BOOST_TEST(q.empty());
      BOOST_TEST(q.closed());
  }

  return boost::report_errors();
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(sync/mutual_exclusion/sync_deque, single_thread_pass)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
