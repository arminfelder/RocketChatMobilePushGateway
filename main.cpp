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

#include <unistd.h>

#include <drogon/drogon.h>
#include "glog/flags.h"
#include "glog/logging.h"

#include "Settings.h"
#include "handlers/ApplePushHandler.h"
#include "handlers/GooglePushHandler.h"
#include "libs/drogon/trantor/trantor/utils/Logger.h"
#include "models/GooglePushModel.h"
#include "models/ApplePushModel.h"

int main(int argc, char* argv[]) {

    std::ignore = argc;
    FLAGS_logtostderr = true;

    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();

    Settings::init();
    GooglePushModel::init();
    ApplePushModel::init();

    drogon::app()
     .setLogLevel(trantor::Logger::kDebug)
     .addListener("0.0.0.0", 11000)
     .setThreadNum(0)
    .setUploadPath("/tmp/upload");

    try {
        drogon::app().run();
    } catch (const std::exception& ex) {
        LOG(ERROR) << "Application failed to start: " << ex.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
