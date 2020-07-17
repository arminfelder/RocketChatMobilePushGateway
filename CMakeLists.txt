cmake_minimum_required(VERSION 3.0)
project(RocketChatMobilePushGateway)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

include_directories(/usr/local/include/r3/)
include_directories(libs/cpp-jwt/include)
include_directories(libs/cpp-base64)

link_directories(/usr/local/lib/proxygen)

set(SOURCE_FILES
        main.cpp
        HandlerFactory.cpp
        handlers/NotFoundHandler.cpp
        utils.cpp
        date.h
        Settings.cpp
        handlers/ApplePushHandler.cpp
        handlers/GooglePushHandler.cpp
        handlers/GooglePushHandler.h
        models/GooglePushModel.cpp
        models/ApplePushModel.cpp
        models/ForwardGatewayModel.cpp
        libs/cpp-base64/base64.cpp
        )
add_executable(rocketChatMobilePushGateway ${SOURCE_FILES})

add_library(fizz SHARED IMPORTED)
add_library(proxygenhttpserver SHARED IMPORTED)

target_link_libraries(rocketChatMobilePushGateway -lfizz -lwangle -lfolly -lz -lssl -lcrypto -levent -lgflags -lglog  -L/usr/lib64 -lpthread -pthread -lfolly -lglog -ldouble-conversion -lboost_system -lboost_context -lboost_thread -lproxygen -lproxygenhttpserver -ljsoncpp -lcurl )
