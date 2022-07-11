project(rocketmq)
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

SET(CMAKE_C_COMPILER_WORKS TRUE)

include_directories(${CMAKE_JS_INC})
include_directories(deps/rocketmq/include)
#include_directories(/Users/wuteng/Desktop/github/node/src)
#include_directories(/Users/wuteng/Desktop/github/node/deps/uv/include)

file(GLOB SOURCE_FILES "*.cc")
add_library(${PROJECT_NAME}
        SHARED
        ${SOURCE_FILES}
        ${CMAKE_JS_SRC}
        src/producer.h
        src/producer.cpp
        src/rocketmq.cpp
        src/rocketmq.h
        src/push_consumer.cpp
        src/push_consumer.h
        src/consumer_ack.cpp
        src/consumer_ack.h
        src/consumer_ack_inner.cpp
        src/consumer_ack_inner.h
        src/workers/producer/send_message.h
        src/workers/producer/start_or_shutdown.h
        src/workers/push_consumer/start_or_shutdown.h)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})


# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})
if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif (CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif (CMAKE_HOST_UNIX)
    set(LINUX 1)
endif ()

if (MACOS)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${PROJECT_SOURCE_DIR}/deps/lib/librocketmq.dylib ${PROJECT_SOURCE_DIR}/deps/lib/librocketmq.a)
elseif (WINDOWS)
elseif (LINUX)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${PROJECT_SOURCE_DIR}/deps/lib/librocketmq.so ${PROJECT_SOURCE_DIR}/deps/lib/librocketmq.a)
endif ()

#CMAKE_CXX_FLAGS
# Define NAPI_VERSION
add_definitions(-D NAPI_VERSION=6)

# -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_C_COMPILER=$(which gcc)