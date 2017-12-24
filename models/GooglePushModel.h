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


class GooglePushModel {


public:
    GooglePushModel(const std::string &pJson);

    bool sendMessage();

    static void loadApiKey();

private:
    static std::string mApiKey;

    const std::string mApiUrl{"https://fcm.googleapis.com/fcm/send"};

    std::string mTitle;
    std::string mText;
    std::string mDeviceToken;
    std::string mFrom;
    int mBadge{0};

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHMODEL_H
