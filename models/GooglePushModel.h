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

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHMODEL_H
#define ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHMODEL_H

#include <string>
#include <jsoncpp/json/json.h>
#include <chrono>
#include <mutex>
#include <memory>
#include "../models/GatewayNotification.h"


class GooglePushModel {

    typedef struct
    {
        std::string token;
        std::chrono::time_point<std::chrono::system_clock> expires_in;
    } Token;

public:
    [[nodiscard]]static std::string getPayload(const GatewayNotification& pNotification);

    static bool sendMessage(const GatewayNotification& pNotification);

    int returnStatusCode() const;

    static const std::string &getAccessToken();

    static bool requestAccessToken();

private:
    static Token mAccessToken;
    static std::mutex mTokenMutex;

    int mReturnStatusCode;
};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHMODEL_H
