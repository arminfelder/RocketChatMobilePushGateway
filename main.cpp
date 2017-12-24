/********************************************************************************************************************
 *                                                                                                                  *
 * Copyright (C) 2017 Armin Felder                                                                                  *
 * This file is part of RocketChatMobilePushGateway <https://git.fairkom.net/chat/RocketChatMobilePushGateway>.     *
 *                                                                                                                  *
 * RocketChatMobilePushGateway is free software: you can redistribute it and/or modify                              *
 * it under the terms of the GNU General Public License as published by                                             *
 * the Free Software Foundation, either version 3 of the License, or                                                *
 * (at your option) any later version.                                                                              *
 *                                                                                                                  *
 * RocketChatMobilePushGateway is distributed in the hope that it will be useful,                                   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                                                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                                    *
 * GNU General Public License for more details.                                                                     *
 *                                                                                                                  *
 * You should have received a copy of the GNU General Public License                                                *
 * along with RocketChatMobilePushGateway. If not, see <http://www.gnu.org/licenses/>.                              *
 *                                                                                                                  *
 ********************************************************************************************************************/

#include <gflags/gflags.h>
#include <folly/Memory.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <unistd.h>
#include "HandlerFactory.h"
#include "models/GooglePushModel.h"

using namespace proxygen;

using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;

using Protocol = HTTPServer::Protocol;

DEFINE_int32(http_port, 11000, "Port to listen on with HTTP protocol");
DEFINE_int32(spdy_port, 11001, "Port to listen on with SPDY protocol");
DEFINE_int32(h2_port, 11002, "Port to listen on with HTTP/2 protocol");
DEFINE_string(ip, "localhost", "IP/Hostname to bind to");
DEFINE_int32(threads, 0, "Number of threads to listen on. Numbers <= 0 "
        "will use the number of cores on this machine.");


int main(int argc, char* argv[]) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    GooglePushModel::loadApiKey();

    std::vector<HTTPServer::IPConfig> IPs = {
            {
                    SocketAddress(FLAGS_ip, FLAGS_http_port, true), Protocol::HTTP
            }
    };

    if(FLAGS_threads <= 0){
        FLAGS_threads = sysconf(_SC_NPROCESSORS_ONLN);
        CHECK(FLAGS_threads > 0);
    }
    HTTPServerOptions options;
    options.threads = static_cast<size_t>(FLAGS_threads);
    options.idleTimeout = std::chrono::milliseconds(60000);
    options.shutdownOn = {SIGINT, SIGTERM};
    options.enableContentCompression = false;
    options.h2cEnabled = true;
    options.handlerFactories = RequestHandlerChain()
            .addThen<HandlerFactory>()
            .build();

    HTTPServer server(std::move(options));
    server.bind(IPs);

    // Start HTTPServer mainloop in a separate thread
    std::thread t([&] () {
        server.start();
    });

    t.join();

    return EXIT_SUCCESS;
}