// (C) Copyright 2012 Howard Hinnant
// (C) Copyright 2012 Vicente Botet
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// adapted from the example given by Howard Hinnant in

#include <boost/config.hpp>

#define BOOST_THREAD_VERSION 4
#define BOOST_THREAD_QUEUE_DEPRECATE_OLD
#if ! defined  BOOST_NO_CXX11_DECLTYPE
#define BOOST_RESULT_OF_USE_DECLTYPE
#endif
//#define XXXX

#include <iostream>
#include <boost/thread/scoped_thread.hpp>
#ifdef XXXX
#include <boost/thread/externally_locked_stream.hpp>
    typedef  boost::externally_locked_stream<std::ostream> the_ostream;
#else
    typedef std::ostream the_ostream;
    typedef std::istream the_istream;
#endif

#include <boost/thread/sync_bounded_queue.hpp>

namespace {

void producer(the_ostream &/*mos*/, boost::sync_bounded_queue<int> & sbq)
{
  using namespace boost;
  try {
    for(int i=0; ;++i)
    {
      sbq.push_back(i);
      //sbq << i;
      //mos << "push_back(" << i << ") "<< sbq.size()<<"\n";
      this_thread::sleep_for(chrono::milliseconds(200));
    }
  }
  catch(sync_queue_is_closed&)
  {
    //mos << "closed !!!\n";
  }
  catch(...)
  {
    //mos << "exception !!!\n";
  }
}

void consumer(the_ostream &/*mos*/, boost::sync_bounded_queue<int> & sbq)
{
  using namespace boost;
  try {
    for(int i=0; ;++i)
    {
      int r;
      sbq.pull_front(r);
      //sbq >> r;
      //mos << i << " pull_front(" << r << ") "<< sbq.size()<<"\n";
      this_thread::sleep_for(chrono::milliseconds(250));
    }
  }
  catch(sync_queue_is_closed&)
  {
    //mos << "closed !!!\n";
  }
  catch(...)
  {
    //mos << "exception !!!\n";
  }
}
void consumer2(the_ostream &/*mos*/,  boost::sync_bounded_queue<int> & sbq)
{
  using namespace boost;
  try {
    for(int i=0; ;++i)
    {
      int r;
      queue_op_status st = sbq.try_pull_front(r);
      if (queue_op_status::closed == st) break;
      if (queue_op_status::success == st) {
        //mos << i << " pull(" << r << ")\n";
      }
      this_thread::sleep_for(chrono::milliseconds(250));
    }
  }
  catch(...)
  {
    //mos << "exception !!!\n";
  }
}
//void consumer3(the_ostream &mos, boost::sync_bounded_queue<int> & sbq)
//{
//  using namespace boost;
//  bool closed=false;
//  try {
//    for(int i=0; ;++i)
//    {
//      int r;
//      queue_op_status res = sbq.wait_and_pull(r);
//      if (res==queue_op_status::closed) break;
//      //mos << i << " wait_and_pull(" << r << ")\n";
//      this_thread::sleep_for(chrono::milliseconds(250));
//    }
//  }
//  catch(...)
//  {
//    //mos << "exception !!!\n";
//  }
//}

static int test_main()
{
  using namespace boost;
#ifdef XXXX
  recursive_mutex terminal_mutex;

  externally_locked_stream<std::ostream> mcerr(std::cerr, terminal_mutex);
  externally_locked_stream<std::ostream> mcout(std::cout, terminal_mutex);
  externally_locked_stream<std::istream> mcin(std::cin, terminal_mutex);
#else
  the_ostream &mcerr = std::cout;
  the_ostream &mcout = std::cerr;
  //the_istream &mcin = std::cin;
#endif

  sync_bounded_queue<int> sbq(10);

  {
    mcout << "begin of main" << std::endl;
    scoped_thread<> t11(boost::thread(producer, boost::ref(mcerr), boost::ref(sbq)));
    scoped_thread<> t12(boost::thread(producer, boost::ref(mcerr), boost::ref(sbq)));
    scoped_thread<> t2(boost::thread(consumer, boost::ref(mcout), boost::ref(sbq)));

    this_thread::sleep_for(chrono::seconds(1));

    sbq.close();
    mcout << "closed()" << std::endl;

  } // all threads joined here.
  mcout << "end of main" << std::endl;
  return 0;
}


} // namespace

#include "common.hpp"

BOOST_AUTO_TEST_CASE(producer_consumer_bounded)
{
    common_init();
    std::thread([&]() {
        TEST_ASSERT(test_main() == 0);
    }).join();
}
