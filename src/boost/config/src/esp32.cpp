#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <cstring>
#include <errno.h>
#include <map>
#include <mutex>
#include <atomic>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "rom/ets_sys.h"

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

#if CONFIG_BOOST_THREAD_ENABLED
/**
 * @brief Virtual pipe implementation using FreeRTOS queue
 *
 * This provides a high-performance pipe implementation for ESP32
 * without any network overhead.
 */
namespace {

constexpr size_t VIRTUAL_PIPE_QUEUE_SIZE = 1024;
constexpr int    VIRTUAL_PIPE_FD_MIN = 10000;
constexpr int    VIRTUAL_PIPE_REF_COUNT = 2;

struct VirtualPipe {
    QueueHandle_t queue;           // FreeRTOS queue for data transfer
    SemaphoreHandle_t read_sem;    // Semaphore to wake up readers
    int read_fd;                   // Virtual file descriptor for reading
    int write_fd;                  // Virtual file descriptor for writing
    std::atomic<bool> closed;      // Whether pipe is closed
    std::atomic<int> ref_count;    // Reference count (2 initially: read + write)
};

// Global state for virtual pipes
static std::mutex& get_virtual_pipe_mutex() {
    static std::mutex mutex;
    return mutex;
}

static std::map<int, VirtualPipe*>& get_virtual_pipes() {
    static std::map<int, VirtualPipe*> pipes;
    return pipes;
}

static int get_next_virtual_fd() {
    static std::atomic<int> next_fd(VIRTUAL_PIPE_FD_MIN);
    return next_fd.fetch_add(1);
}

// Check if a file descriptor is a virtual pipe
static VirtualPipe* get_virtual_pipe(int fd) {
    std::lock_guard<std::mutex> lock(get_virtual_pipe_mutex());
    auto it = get_virtual_pipes().find(fd);
    return (it != get_virtual_pipes().end()) ? it->second : nullptr;
}

// Register a virtual pipe
static void register_virtual_pipe(int fd, VirtualPipe* pipe) {
    std::lock_guard<std::mutex> lock(get_virtual_pipe_mutex());
    get_virtual_pipes()[fd] = pipe;
}

// Unregister and cleanup virtual pipe if reference count reaches 0
static void unregister_virtual_pipe(int fd) {
    VirtualPipe* pipe = nullptr;
    {
        std::lock_guard<std::mutex> lock(get_virtual_pipe_mutex());
        auto it = get_virtual_pipes().find(fd);
        if (it == get_virtual_pipes().end()) return;

        pipe = it->second;
        get_virtual_pipes().erase(it);
    }

    // Decrement reference count
    if (pipe && pipe->ref_count.fetch_sub(1) == 1) {
        // Last reference, cleanup
        if (pipe->queue) vQueueDelete(pipe->queue);
        if (pipe->read_sem) vSemaphoreDelete(pipe->read_sem);
        delete pipe;
    }
}

} // anonymous namespace

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

/**
 * @brief Implementation of pipe function for ESP32 using FreeRTOS queue
 *
 * Creates a virtual pipe using FreeRTOS queue for data transfer.
 * This provides zero network overhead and optimal performance.
 *
 * @param pipefd Array to store the two file descriptors: pipefd[0] for reading, pipefd[1] for writing
 * @return 0 on success, -1 on failure
 */
__attribute__((weak))
int pipe(int pipefd[2]) {
    // Create FreeRTOS queue (VIRTUAL_PIPE_QUEUE_SIZE bytes capacity, 1 byte per item)
    QueueHandle_t queue = xQueueCreate(VIRTUAL_PIPE_QUEUE_SIZE, sizeof(uint8_t));
    if (!queue) {
        errno = ENOMEM;
        return -1;
    }

    // Create semaphore for blocking reads
    SemaphoreHandle_t read_sem = xSemaphoreCreateBinary();
    if (!read_sem) {
        vQueueDelete(queue);
        errno = ENOMEM;
        return -1;
    }

    // Allocate virtual pipe structure
    VirtualPipe* vpipe = new (std::nothrow) VirtualPipe();
    if (!vpipe) {
        vQueueDelete(queue);
        vSemaphoreDelete(read_sem);
        errno = ENOMEM;
        return -1;
    }

    // Initialize virtual pipe
    vpipe->queue = queue;
    vpipe->read_sem = read_sem;
    vpipe->closed = false;
    vpipe->ref_count = VIRTUAL_PIPE_REF_COUNT;  // Read end + Write end

    // Assign virtual file descriptors
    {
        std::lock_guard<std::mutex> lock(get_virtual_pipe_mutex());
        vpipe->read_fd = get_next_virtual_fd();
        vpipe->write_fd = get_next_virtual_fd();
    }

    // Register both ends
    register_virtual_pipe(vpipe->read_fd, vpipe);
    register_virtual_pipe(vpipe->write_fd, vpipe);

    pipefd[0] = vpipe->read_fd;   // Read end
    pipefd[1] = vpipe->write_fd;  // Write end

    return 0;
}

/**
 * @brief Custom read implementation for virtual pipes
 *
 * Uses syscall() to call the original read when needed, avoiding __real_read dependency.
 */
extern "C" ssize_t __real_read(int fd, void* buf, size_t count);

extern "C" ssize_t __wrap_read(int fd, void* buf, size_t count) {
    VirtualPipe* vpipe = get_virtual_pipe(fd);
    if (!vpipe) {
        // Not a virtual pipe, use system read
        // Call __real_read directly to avoid recursion
        return __real_read(fd, buf, count);
    }

    // Check if this is the read end
    if (fd != vpipe->read_fd) {
        errno = EBADF;  // Wrong end of pipe
        return -1;
    }

    if (vpipe->closed) {
        return 0;  // EOF
    }

    uint8_t* cbuf = static_cast<uint8_t*>(buf);
    size_t bytes_read = 0;

    // Try to read available data (non-blocking)
    while (bytes_read < count) {
        uint8_t byte;
        if (xQueueReceive(vpipe->queue, &byte, 0) == pdTRUE) {
            cbuf[bytes_read++] = byte;
        } else {
            break;  // No more data available
        }
    }

    if (bytes_read > 0) {
        return bytes_read;
    }

    // No data available, check if blocking
    // For non-blocking, return immediately
    errno = EWOULDBLOCK;
    return -1;
}

/**
 * @brief Custom write implementation for virtual pipes
 */
extern "C" ssize_t __real_write(int fd, const void* buf, size_t count);

extern "C" ssize_t __wrap_write(int fd, const void* buf, size_t count) {
    VirtualPipe* vpipe = get_virtual_pipe(fd);
    if (!vpipe) {
        // Not a virtual pipe, use system write
        return __real_write(fd, buf, count);
    }

    // Check if this is the write end
    if (fd != vpipe->write_fd) {
        errno = EBADF;  // Wrong end of pipe
        return -1;
    }

    if (vpipe->closed) {
        errno = EPIPE;
        return -1;
    }

    const uint8_t* cbuf = static_cast<const uint8_t*>(buf);
    size_t bytes_written = 0;

    // Write data to queue (non-blocking for now)
    while (bytes_written < count) {
        if (xQueueSend(vpipe->queue, &cbuf[bytes_written], 0) == pdTRUE) {
            bytes_written++;
            // Signal reader that data is available
            xSemaphoreGive(vpipe->read_sem);
        } else {
            break;  // Queue full
        }
    }

    if (bytes_written > 0) {
        return bytes_written;
    }

    // Queue full
    errno = EWOULDBLOCK;
    return -1;
}

/**
 * @brief Custom close implementation for virtual pipes
 */
extern "C" int __real_close(int fd);

extern "C" int __wrap_close(int fd) {
    VirtualPipe* vpipe = get_virtual_pipe(fd);
    if (!vpipe) {
        // Not a virtual pipe, use system close
        return __real_close(fd);
    }

    // Mark as closed if closing write end
    if (fd == vpipe->write_fd) {
        vpipe->closed = true;
        // Wake up any blocked readers
        xSemaphoreGive(vpipe->read_sem);
    }

    // Unregister this end
    unregister_virtual_pipe(fd);
    return 0;
}

/**
 * @brief Custom fcntl implementation for virtual pipes
 *
 * Currently supports F_GETFL and F_SETFL for O_NONBLOCK flag.
 * Virtual pipes are always non-blocking in this implementation.
 */
extern "C" int __real_fcntl(int fd, int cmd, ...);

extern "C" int __wrap_fcntl(int fd, int cmd, ...) {
    VirtualPipe* vpipe = get_virtual_pipe(fd);
    if (!vpipe) {
        // Not a virtual pipe, use system fcntl
        va_list args;
        va_start(args, cmd);
        int arg = va_arg(args, int);
        va_end(args);
        return __real_fcntl(fd, cmd, arg);
    }

    // Handle virtual pipe fcntl
    switch (cmd) {
        case F_GETFL:
            // Virtual pipes are always non-blocking
            return O_NONBLOCK;

        case F_SETFL:
            // Accept flags but ignore (always non-blocking)
            return 0;

        default:
            errno = EINVAL;
            return -1;
    }
}
#endif // CONFIG_BOOST_THREAD_ENABLED

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
