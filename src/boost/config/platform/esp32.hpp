/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//  esp32 specific config options:

#if !defined(ESP_PLATFORM)
    #error "Not using ESP_PLATFORM"
#endif

#define BOOST_PLATFORM "esp32"

#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#define BOOST_HAS_STDINT_H
#define BOOST_HAS_UNISTD_H
#define BOOST_HAS_GETTIMEOFDAY
#define BOOST_HAS_SCHED_YIELD

/**
 * lib: test
 */
#define BOOST_TEST_NO_MAIN

/**
 * lib: thread
 */
#define BOOST_THREAD_HAS_NO_EINTR_BUG
#define BOOST_THREAD_USES_GETPAGESIZE

/**
 * lib: unordered
 */
// Avoid ld warning "orphan section `.debug_gdb_scripts' from `esp-idf/main/libmain.a(test_6170.cpp.obj)' being placed in section `.debug_gdb_scripts'"
#define BOOST_ALL_NO_EMBEDDED_GDB_SCRIPTS

#include <boost/config/detail/posix_features.hpp>

/**
 * Avoid link error for missing `nanosleep()`
 */
// #undef BOOST_HAS_NANOSLEEP

#include "esp_idf_version.h"
#include "sdkconfig.h"

/**
 * When `CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND` is enabled, `boost::atomic` will trigger assert error
 * "Boost.Atomic unsupported target platform: native atomic operations not implemented for function pointers",
 * So, we use `std::atomic` instead of `boost::atomic`
 * (For ESP32-S3, when PSRAM is enabled, `CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND` is enabled)
 */
// #if CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND
    #define BOOST_NO_CXX11_HDR_ATOMIC

    /* lib: atomic */
    #define BOOST_ATOMIC_USE_STD_ATOMIC

    /* lib: smart_ptr */
    // Avoid to include `boost/smart_ptr/detail/sp_has_gcc_intrinsics.hpp`
    #define BOOST_SMART_PTR_DETAIL_SP_HAS_GCC_INTRINSICS_HPP_INCLUDED

    /**
     * lib: thread
     */
    #define BOOST_THREAD_DONT_USE_ATOMIC
    // Avoid assert error in `test_store_value_from_thread`
    #define BOOST_THREAD_DONT_PROVIDE_THREAD_DESTRUCTOR_CALLS_TERMINATE_IF_JOINABLE
// #endif

#if !defined(CONFIG_COMPILER_CXX_RTTI)
    #define BOOST_NO_RTTI
    #define BOOST_NO_TYPEID
#endif

#if !defined(CONFIG_COMPILER_CXX_EXCEPTIONS)
    #define BOOST_NO_EXCEPTIONS
#else
    /**
     * lib: thread
     */
    // Allow `join()` to throw exceptions
    #define BOOST_THREAD_THROW_IF_PRECONDITION_NOT_SATISFIED
#endif
