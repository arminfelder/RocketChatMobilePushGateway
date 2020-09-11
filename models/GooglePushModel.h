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
#include <curl/curl.h>

#include <proxygen/lib/http/HTTPMessage.h>


class GooglePushModel {

public:
    explicit GooglePushModel(const std::string &pJson);

    bool sendMessage();

    static void loadApiKey();
    static int trace(CURL *handle, curl_infotype type,
                        char *data, size_t size,
                        void *userp);

    static void initFromSettings();

    static size_t curlWriteCallback(void *buffer, size_t size, size_t nmemb,
                                    void *this_ptr);

    int returnStatusCode() const;

private:
    static std::string mApiKey;

    const std::string mApiUrl{"https://fcm.googleapis.com/fcm/send"};

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

    int mReturnStatusCode;

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHMODEL_H
