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
#include <fstream>
#include <algorithm>
#include "ApplePushModel.h"


ApplePushModel::ApplePushModel(const std::string &pJson):mPusher("/certs/apple/cred.pem") {


    Json::Reader reader;
    Json::Value obj;
    Json::FastWriter fast;

    if(pJson.length()) {
        reader.parse(pJson, obj);

        if (obj.isMember("token") && obj.isMember("options")) {

            Json::Value options = obj["options"];

            Json::Value apn = options["apn"];
            std::string test(fast.write(options));
            if (apn.isMember("text")) {
               std::string temp = std::move(apn["text"].asString());
                unsigned long index = 0;
                while(true){
                    index = temp.find('\n',index);
                    if(index == std::string::npos){
                        break;
                    }
                    temp.replace(index, 1, "\\r\\n");
                    index += 4;
                }
                mApn = std::move(temp);


            }
            if (options.isMember("title")) {
                mTitle = std::move(options["title"].asString());
            }
            if (options.isMember("text")) {
                mText = std::move(options["text"].asString());
            }
            if (options.isMember("from")) {
                mFrom = std::move(options["from"].asString());
            }
            if (options.isMember("badge")) {
                mBadge = options["badge"].asInt();
            }
            if (options.isMember("payload")) {
                mPayload = std::move(fast.write(options["payload"]));
            }
            if (options.isMember("topic")) {
                mTopic = std::move(options["topic"].asString());
            }
            if (options.isMember("sound")) {
                mSound = std::move(options["sound"].asString());
            }
            mDeviceToken = std::move(obj["token"].asString());

        }
    }

}

bool ApplePushModel::sendMessage() {

    std::vector<std::string> tokens;

    Json::FastWriter fast;

    tokens.push_back(mDeviceToken);

    PusherContent content;

    if(mPayload.length()){
        content.userData = "\"ejson\":"+mPayload+"";
    }else{
        content.userData = "";
    }

    content.badge = mBadge;
    content.content = mApn;
    content.sound = mSound;


    mPusher.isSandBox = true;
    mPusher.pushNotification(content, tokens);


    //TODO: shoud be fixed, pusherCPP does not give a return value
    return true;
}
