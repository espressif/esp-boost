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
#include <boost/math/special_functions/legendre.hpp>
#include "c_policy.hpp"

extern "C" double BOOST_MATH_TR1_DECL boost_legendre BOOST_MATH_PREVENT_MACRO_SUBSTITUTION(unsigned n, double x) BOOST_MATH_C99_THROW_SPEC
{
   return c_policies::legendre_p BOOST_MATH_PREVENT_MACRO_SUBSTITUTION(n, x);
}


