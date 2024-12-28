cmake_minimum_required(VERSION 3.10)
project(distributed_computing)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## load in pkg-config support
find_package(PkgConfig)
## use pkg-config to get hints for 0mq locations
pkg_check_modules(PC_ZeroMQ QUIET zmq)

## use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
)

## use the hint from above to find the location of libzmq
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
)

include_directories(src/include)

# Create library from common source files
add_library(common_lib
        src/nodes.cpp
        src/utils.cpp
        src/zmq_operations.cpp
)

# Set header include directories for the library
target_include_directories(common_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ZeroMQ_INCLUDE_DIR})

# Create executables
add_executable(control src/control.cpp)
add_executable(computing src/child.cpp)

# Link libraries
target_link_libraries(control
        PRIVATE
        common_lib
        ${ZeroMQ_LIBRARY}
)

target_link_libraries(computing
        PRIVATE
        common_lib
        ${ZeroMQ_LIBRARY}
)

# Set compile options if needed
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(control PRIVATE -Wall -Wextra)
    target_compile_options(computing PRIVATE -Wall -Wextra)
endif()
