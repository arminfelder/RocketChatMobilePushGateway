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

#include <jsoncpp/json/json.h>
#include <iostream>
#include <curl/curl.h>
#include <fstream>
#include <openssl/ssl.h>
#include "ApplePushModel.h"


ApplePushModel::ApplePushModel(const std::string &pJson):mPusher("credentials/apple/cred.pem") {
    Json::Reader reader;
    Json::Value obj;
    reader.parse(pJson, obj);
    std::cout<<obj["token"].asString()<<std::endl;
    if(obj.isMember("token") && obj.isMember("options")){
        std::string token = obj["token"].asString();
        Json::Value options = obj["options"];
        if(options.isMember("from")&&options.isMember("badge")&&options.isMember("title")&&options.isMember("text")){
            std::cout<<"valid json"<<std::endl;
            mTitle = std::move(options["title"].asString());
            mText = std::move(options["text"].asString());
            mFrom = std::move(options["from"].asString());
            mDeviceToken = std::move(obj["token"].asString());
            mBadge = options["badge"].asInt();
        }
    }
}

bool ApplePushModel::sendMessage() {

    Json::FastWriter fast;

    std::vector<std::string> tokens;
    tokens.push_back(mDeviceToken);

    PusherContent content;

    Json::Value obj;
    Json::Value msg;
    msg["title"] = mTitle;
    msg["body"] = mText;

    std::string data = fast.write(msg);


    content.badge = mBadge;
    content.content = mTitle;
    content.userData = "\"ejson\":"+data;


    mPusher.isSandBox = true;
    mPusher.pushNotification(content, tokens);

    //TODO: shoud be fixed

    std::string url{"https://gateway.sandbox.push.apple.com:2195"};

    return false;
}
