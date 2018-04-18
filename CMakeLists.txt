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
set(SOURCE_FILES main.cpp HandlerFactory.cpp handlers/NorFoundHandler.cpp utils.cpp utils.h handlers/ApplePushHandler.cpp handlers/ApplePushHandler.h handlers/GooglePushHandler.cpp handlers/GooglePushHandler.h models/GooglePushModel.cpp models/GooglePushModel.h models/ApplePushModel.cpp models/ApplePushModel.h
         models/PusherCpp/Pusher.cpp models/PusherCpp/Pusher.h)
add_executable(rocketChatMobilePushGateway ${SOURCE_FILES} utils.cpp utils.h models/GooglePushModel.cpp models/GooglePushModel.h)

target_link_libraries(rocketChatMobilePushGateway -lwangle -lfolly -lz -lssl -lcrypto -levent -lgflags -lglog  -L/usr/lib64 -lpthread -pthread -lfolly -lglog -ldouble-conversion -lboost_system -lboost_thread -lproxygenhttpserver -ljsoncpp -lcurl)
