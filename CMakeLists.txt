cmake_minimum_required(VERSION 3.0)
project(RocketChatMobilePushGateway)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/local/include/r3/)
set(SOURCE_FILES main.cpp HandlerFactory.cpp handlers/TestHandler.cpp utils.cpp utils.h handlers/ApplePushHandler.cpp handlers/ApplePushHandler.h handlers/GooglePushHandler.cpp handlers/GooglePushHandler.h models/GooglePushModel.cpp models/GooglePushModel.h models/ApplePushModel.cpp models/ApplePushModel.h)
add_executable(rocketChatMobilePushGateway ${SOURCE_FILES} utils.cpp utils.h models/GooglePushModel.cpp models/GooglePushModel.h)

target_link_libraries(rocketChatMobilePushGateway -lwangle -lfolly -lz -lssl -lcrypto -levent -lgflags -lglog  -L/usr/lib64 -lpthread -pthread -lfolly -lglog -ldouble-conversion -lboost_system -lboost_thread -lproxygenhttpserver -ljsoncpp -lcurl)
