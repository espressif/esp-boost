//  Copyright John Maddock 2008.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0.  (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/config.hpp>
#ifndef BOOST_MATH_DISABLE_PCH
#  include <pch.hpp>
#endif
#ifndef BOOST_MATH_TR1_SOURCE
#  define BOOST_MATH_TR1_SOURCE
#endif
#include <boost/math/tr1.hpp>
#include <boost/math/special_functions/fpclassify.hpp>
#include "c_policy.hpp"

extern "C" double BOOST_MATH_TR1_DECL boost_fmax BOOST_MATH_PREVENT_MACRO_SUBSTITUTION(double x, double y) BOOST_MATH_C99_THROW_SPEC
{
   if((boost::math::isnan)(x))
      return y;
   if((boost::math::isnan)(y))
      return x;
   return  (std::max)(x, y);
}


