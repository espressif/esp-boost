set(COMPONENT_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

set(COMPONENT_SRC_FILES)

list(APPEND COMPONENT_SRC_FILES ${COMPONENT_SRC_DIR}/boost/config/src/esp32.cpp)

# Json
if(CONFIG_BOOST_JSON_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/json/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Json")
endif()

# Chrono
if(CONFIG_BOOST_CHRONO_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/chrono/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Chrono")
endif()

# Container
if(CONFIG_BOOST_CONTAINER_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/container/src/*.cpp
        ${COMPONENT_SRC_DIR}/boost/container/src/*.c
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Container")
endif()

# Exception
if(CONFIG_BOOST_EXCEPTION_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/exception/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Exception")
endif()

# Graph
if(CONFIG_BOOST_GRAPH_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/graph/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Graph")
endif()

# Math
if(CONFIG_BOOST_MATH_ENABLED)
    file(GLOB_RECURSE MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/math/src/*.cpp
        ${COMPONENT_SRC_DIR}/boost/math/src/**/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Math")
endif()

# Random
if(CONFIG_BOOST_RANDOM_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/random/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Random")
endif()

# Regex
if(CONFIG_BOOST_REGEX_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/regex/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Regex")
endif()

# Serialization
if(CONFIG_BOOST_SERIALIZATION_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/serialization/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Serialization")
endif()

# System
if(CONFIG_BOOST_SYSTEM_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/system/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.System")
endif()

# Test
if(CONFIG_BOOST_TEST_ENABLED)
    file(GLOB MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/test/src/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Test")
endif()

# Thread
if(CONFIG_BOOST_THREAD_ENABLED)
    file(GLOB_RECURSE MODULE_SRCS
        ${COMPONENT_SRC_DIR}/boost/thread/src/*.cpp
        ${COMPONENT_SRC_DIR}/boost/thread/src/**/*.cpp
    )
    list(APPEND COMPONENT_SRC_FILES ${MODULE_SRCS})
    message(STATUS "Including Boost.Thread")
endif()

set(COMPONENT_INCLUDE_DIRS ${COMPONENT_SRC_DIR})
set(COMPONENT_PUBLIC_REQUIRES pthread)

#
# Register component
#
idf_component_register(${COMPONENT_LIB}
    SRCS
        ${COMPONENT_SRC_FILES}
    INCLUDE_DIRS
        ${COMPONENT_INCLUDE_DIRS}
    REQUIRES
        ${COMPONENT_PUBLIC_REQUIRES}
)

# esp-boost must be building from ESP platform
if (NOT ESP_PLATFORM)
    message(FATAL_ERROR "ESP_PLATFORM not defined. Please build from the ESP platform.")
endif()

# set necessary macros
if ((NOT DEFINED CONFIG_COMPILER_CXX_RTTI) OR (NOT DEFINED CONFIG_COMPILER_CXX_EXCEPTIONS))
    # Try to detect sdkconfig.h
    set(SDKCONFIG_PATH "${CMAKE_BINARY_DIR}/config/sdkconfig.h")
    if(EXISTS ${SDKCONFIG_PATH})
        message(STATUS "Found sdkconfig.h at ${SDKCONFIG_PATH}")

        file(READ ${SDKCONFIG_PATH} SDKCONFIG_CONTENT)

        string(REGEX MATCH "#define CONFIG_COMPILER_CXX_RTTI 1" _rtti "${SDKCONFIG_CONTENT}")
        string(REGEX MATCH "#define CONFIG_COMPILER_CXX_EXCEPTIONS 1" _exceptions "${SDKCONFIG_CONTENT}")

        set(CONFIG_COMPILER_CXX_RTTI OFF)
        set(CONFIG_COMPILER_CXX_EXCEPTIONS OFF)

        if (_rtti)
            message(STATUS "RTTI is enabled")
            set(CONFIG_COMPILER_CXX_RTTI ON)
        endif()

        if (_exceptions)
            message(STATUS "Exceptions are enabled")
            set(CONFIG_COMPILER_CXX_EXCEPTIONS ON)
        endif()
    else()
        message(FATAL_ERROR "sdkconfig.h not found.")
    endif()

    target_compile_definitions(${COMPONENT_LIB} PUBLIC
        CONFIG_COMPILER_CXX_RTTI=$<BOOL:${CONFIG_COMPILER_CXX_RTTI}>
        CONFIG_COMPILER_CXX_EXCEPTIONS=$<BOOL:${CONFIG_COMPILER_CXX_EXCEPTIONS}>
    )
else()
    message(STATUS "Boost: CONFIG_COMPILER_CXX_RTTI and CONFIG_COMPILER_CXX_EXCEPTIONS are inherited from upper layer.")
endif()

if(CONFIG_BOOST_THREAD_ENABLED)
    # set wrap linker flags for Boost Thread support (optimized)
    set(WRAP_FUNCS read write close fcntl)
    foreach(_func IN LISTS WRAP_FUNCS)
        target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--wrap=${_func}")
    endforeach()
endif()
