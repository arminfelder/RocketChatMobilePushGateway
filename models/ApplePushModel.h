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

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHMODEL_H
#define ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHMODEL_H

#include <curl/curl.h>
#include "PusherCpp/Pusher.h"


class ApplePushModel {

public:
    ApplePushModel(const std::string &pJson);

    bool sendMessage();

private:
    Pusher mPusher;

    std::string mTitle;
    std::string mText;
    std::string mDeviceToken;
    std::string mFrom;
    std::string mPayload;
    std::string mSound;
    std::string mApn;
    std::string mGcm;
    std::string mQuery;
    bool mSent{false};
    int mSendind{0};
    std::string mTopic;

    int mBadge{0};
};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHMODEL_H
