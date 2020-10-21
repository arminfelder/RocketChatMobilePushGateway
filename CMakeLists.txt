cmake_minimum_required(VERSION 3.8)
project(RocketChatMobilePushGateway)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(wangle_DIR /opt/proxygen/proxygen/_build/deps/lib/cmake/wangle)
set(Fizz_DIR /opt/proxygen/proxygen/_build/deps/lib/cmake/fizz)
set(fmt_DIR /opt/proxygen/proxygen/_build/deps/lib/cmake/fmt)
set(folly_DIR /opt/proxygen/proxygen/_build/deps/lib/cmake/folly)
set(proxygen_DIR /opt/proxygen/proxygen/_build/lib/cmake/proxygen)

find_package(Boost COMPONENTS system filesystem context thread program_options regex iostreams REQUIRED)
find_package(proxygen CONFIG REQUIRED)
find_package(gflags COMPONENTS shared)

include_directories(/usr/local/include/r3)
include_directories(libs/cpp-jwt/include)
include_directories(libs/cpp-base64)
include_directories(/opt/proxygen/proxygen/_build/deps/include)
include_directories(/opt/proxygen/proxygen/_build/include)
include_directories(proxygen_INCLUDE_DIRS)

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



target_link_libraries(rocketChatMobilePushGateway
        -lglog
        -pthread
        -ljsoncpp
        -lcurl
        -lcrypto
        /opt/proxygen/proxygen/_build/proxygen/httpserver/libproxygenhttpserver.a
        proxygen::proxygen
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_CONTEXT_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_REGEX_LIBRARY}
        ${CMAKE_DL_LIBS} )
