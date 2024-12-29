cmake_minimum_required(VERSION 3.25)
project(RocketChatMobilePushGateway)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -flto")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(BUILD_SHARED_LIBS OFF)

set(BUILD_CTL OFF CACHE BOOL "")
set(BUILD_EXAMPLES OFF CACHE BOOL "")
set(BUILD_ORM OFF CACHE BOOL "")
set(BUILD_DOC OFF CACHE BOOL "")
set(BUILD_YAML_CONFIG OFF CACHE BOOL "")
add_subdirectory(libs/drogon)
unset(BUILD_CTL CACHE)
unset(BUILD_EXAMPLES CACHE)
unset(BUILD_ORM CACHE)
unset(BUILD_DOC CACHE)
unset(BUILD_YAML_CONFIG CACHE)

set(BUILD_EXAMPLES OFF CACHE BOOL "")
add_subdirectory(libs/glog)
unset(BUILD_EXAMPLES CACHE)

set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "")
add_subdirectory(libs/jwt-cpp)
unset(JWT_BUILD_EXAMPLES CACHE)

set(BUILD_EXAMPLES OFF CACHE BOOL "")
add_subdirectory(libs/cpr)
unset(BUILD_EXAMPLES CACHE)

include_directories(libs/jwt-cpp/include)
include_directories(libs/drogon/lib/inc)
include_directories(libs/cpr/include)

set(SOURCE_FILES
        main.cpp
        Settings.cpp
        models/GooglePushModel.cpp
        models/ApplePushModel.cpp
        handlers/ApplePushHandler.cpp
        handlers/GooglePushHandler.cpp
        models/ForwardGatewayModel.cpp
        )

add_executable(rocketChatMobilePushGateway ${SOURCE_FILES})

target_compile_features(rocketChatMobilePushGateway PUBLIC cxx_std_23)

target_link_libraries(rocketChatMobilePushGateway
        PRIVATE glog::glog
        PRIVATE drogon
        PRIVATE cpr::cpr
        ${CMAKE_DL_LIBS} )
