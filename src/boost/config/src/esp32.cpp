#include <time.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "boost/config.hpp"

/**
 * esp32: This file provides some standard functions required by libraries that may be declared in ESP-IDF but not
 *        implemented, so they need weak definitions
 */

/**
 * @brief Implementation of nanosleep for ESP32
 *
 * Provides a compatible implementation of the standard nanosleep function
 * for the ESP32 platform where it may not be available.
 *
 * @param req Time to sleep (seconds + nanoseconds)
 * @param rem Remaining unslept time (can be NULL)
 * @return 0 on success, -1 on failure
 */
__attribute__((weak))
int nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!req || req->tv_sec < 0 || req->tv_nsec < 0 || req->tv_nsec >= 1000000000) {
        return -1; // Invalid parameters
    }

    int64_t total_us = req->tv_sec * 1000000LL + req->tv_nsec / 1000;  // Convert to microseconds

    usleep((total_us + 1000 - 1) / 1000);

    return 0;  // Success
}

/**
 * @brief Implementation of getpagesize for ESP32
 *
 * Used when BOOST_THREAD_USES_GETPAGESIZE is defined.
 * Returns a minimal value as ESP32 doesn't use traditional memory pages.
 *
 * @return A constant value representing page size
 */
__attribute__((weak))
int getpagesize() {
    return 1;
}

#if CONFIG_BOOST_ASIO_ENABLED
/**
 * @brief Implementation of pause function for ESP32
 *
 * Suspends the calling thread until a signal is received.
 * In ESP32 implementation, uses vTaskDelay with maximum delay.
 *
 * @return Always returns -1
 */
__attribute__((weak))
int pause(void)
{
    vTaskDelay(portMAX_DELAY);
    return -1;
}

#endif // CONFIG_BOOST_ASIO_ENABLED

#if !CONFIG_LWIP_NETIF_API
/**
 * @brief Implementation of if_nametoindex for ESP32
 *
 * Converts a network interface name to its corresponding index.
 * Optimized for common ESP32 interface names.
 *
 * @param ifname Interface name to convert
 * @return Interface index on success, 0 on failure
 */
__attribute__((weak))
unsigned int if_nametoindex(const char* ifname) {
    // Fast path for null check
    if (!ifname) return 0;

    // Use string comparison for common interface names (faster than multiple strcmp calls)
    // ESP32 typically has these standard interfaces
    if (ifname[0] == 'l' && ifname[1] == 'o' && ifname[2] == '\0') return 1;
    if (ifname[0] == 'e' && ifname[1] == 't' && ifname[2] == 'h' &&
        ifname[3] == '0' && ifname[4] == '\0') return 2;
    if (ifname[0] == 'w' && ifname[1] == 'l' && ifname[2] == 'a' &&
        ifname[3] == 'n' && ifname[4] == '0' && ifname[5] == '\0') return 3;

    // Handle numeric interface index directly
    char* end;
    unsigned long index = strtoul(ifname, &end, 10);
    if (*end == '\0' && index > 0 && index < UINT_MAX)
        return static_cast<unsigned int>(index);

    return 0;
}

/**
 * @brief Implementation of if_indextoname for ESP32
 *
 * Converts a network interface index to its corresponding name.
 * Optimized for common ESP32 interface indices.
 *
 * @param ifindex Interface index to convert
 * @param ifname Buffer to store the interface name (must be at least IF_NAMESIZE bytes)
 * @return Pointer to ifname on success, NULL on failure
 */
__attribute__((weak))
char* if_indextoname(unsigned int ifindex, char* ifname) {
    if (!ifname) return NULL;

    // Map common interface indices to their names
    switch (ifindex) {
        case 1:
            strcpy(ifname, "lo");
            return ifname;
        case 2:
            strcpy(ifname, "eth0");
            return ifname;
        case 3:
            strcpy(ifname, "wlan0");
            return ifname;
        default:
            // For other indices, just convert to string
            if (ifindex > 0 && ifindex < 100) {  // Reasonable limit
                sprintf(ifname, "%u", ifindex);
                return ifname;
            }
            break;
    }

    // Interface not found
    errno = ENXIO;
    return NULL;
}
#endif // CONFIG_LWIP_NETIF_API
