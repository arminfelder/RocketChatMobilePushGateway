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

#include <curl/curl.h>
#include <exception>
#include <jsoncpp/json/json.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <chrono>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <proxygen/lib/utils/CryptUtil.h>

#include "ApplePushModel.h"
#include "../date.h"

#include "../libs/cpp-jwt/include/jwt/jwt.hpp"
#include "../libs/cpp-jwt/include/jwt/algorithm.hpp"
#include "../libs/cpp-jwt/include/jwt/parameters.hpp"
#include "../libs/cpp-base64/base64.h"
#include "ForwardGatewayModel.h"


std::string ApplePushModel::mPem;
std::string ApplePushModel::mTeamId;
std::string ApplePushModel::mAppId;
std::string ApplePushModel::mKey;

ApplePushModel::ApplePushModel(const std::string &pJson) {


    Json::Reader reader;
    Json::Value obj;
    Json::FastWriter fast;

    if (pJson.length()) {
        reader.parse(pJson, obj);

        if (obj.isMember("token") && obj.isMember("options")) {

            Json::Value options = obj["options"];

            Json::Value apn = options["apn"];
            std::string test(fast.write(options));
            if (options.isMember("text")) {
                std::string temp = options["text"].asString();
                unsigned long index = 0;
                while (true) {
                    index = temp.find('\n', index);
                    if (index == std::string::npos) {
                        break;
                    }
                    temp.replace(index, 1, "\\r\\n");
                    index += 4;
                }
                mText = std::move(temp);


            }
            if (options.isMember("title")) {
                mTitle = options["title"].asString();
            }
            /*if (options.isMember("text")) {
                mText = std::move(options["text"].asString());
            }*/
            if (options.isMember("from")) {
                mFrom = options["from"].asString();
            }
            if (options.isMember("badge")) {
                mBadge = options["badge"].asInt();
            }
            if (options.isMember("payload")) {
                mPayload = fast.write(options["payload"]);
            }
            if (options.isMember("topic")) {
                mTopic = options["topic"].asString();
            }
            if (options.isMember("sound")) {
                mSound = options["sound"].asString();
            }
            mDeviceToken = obj["token"].asString();

        }
    }

}

size_t ApplePushModel::curlWriteCallback(void *buffer, size_t size, size_t nmemb,
                                         void *this_ptr) {
    auto thiz = static_cast<ApplePushModel *>(this_ptr);
    Json::Reader reader;
    Json::Value obj;
    if (buffer != nullptr) {
        std::string bufferString(static_cast<char *>(buffer), nmemb);
        reader.parse(bufferString, obj);
        if (obj.isMember("reason") && obj["reason"].asString() == "DeviceTokenNotForTopic") {
            ForwardGatewayModel::claimRegistrationId(thiz->mDeviceToken);
            return 0;
        }
    }
    return 1;
}

bool ApplePushModel::sendMessage() {

    if (ForwardGatewayModel::ownsRegistrationId(mDeviceToken)) {
        return false;
    }

    Json::Value obj;
    Json::FastWriter fast;

    Json::Value alert;

    alert["body"] = mText;
    alert["title"] = mTitle;

    Json::Value aps;
    aps["category"] = "MESSAGE";
    aps["badge"] = mBadge;
    aps["sound"] = mSound;
    aps["alert"] = alert;

    obj["aps"] = aps;
    obj["ejson"] = mPayload;

    std::string json = fast.write(obj);

    using namespace date;
    using namespace std::chrono;

    boost::uuids::uuid uuidObj = boost::uuids::random_generator()();
    std::string uuidString = boost::lexical_cast<std::string>(uuidObj);

    LOG(INFO) << uuidString << "\tApple push data\t" << json << std::endl;

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *chunk = nullptr;

        try {

            jwt::jwt_object obj{jwt::params::algorithm("ES256"),
                                jwt::params::headers({
                                                             {"alg", "ES256"},
                                                             {"kid", mKey}
                                                     }),
                                jwt::params::secret(mPem)
            };

            auto n = system_clock::now();
            auto in_time_t = system_clock::to_time_t(n);

            obj.add_claim("iss", mTeamId).add_claim("iat", in_time_t);

            std::string encoded_jwt = obj.signature();

            chunk = curl_slist_append(chunk, std::string("Authorization: Bearer " + obj.signature()).c_str());
            chunk = curl_slist_append(chunk, std::string("apns-id: " + uuidString).c_str());
            chunk = curl_slist_append(chunk, std::string("apns-expiration: 0").c_str());
            chunk = curl_slist_append(chunk, std::string("apns-priority: 10").c_str());
            chunk = curl_slist_append(chunk, std::string("apns-topic: " + mAppId).c_str());


            std::string url = mApiUrl + mDeviceToken;

            //  curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, trace );
            curl_easy_setopt(curl, CURLOPT_VERBOSE, false);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json.size());
            curl_easy_setopt(curl, CURLOPT_POST, true);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_USE_SSL, true);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, true);
            curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                if (res == CURLE_WRITE_ERROR) {
                    LOG(INFO) << "\tApple push device token rejected, forward message to forwardgateway";
                } else {
                    LOG(ERROR) << "\tApple push conn error: " << curl_easy_strerror(res);
                }
            } else {
                LOG(INFO) << uuidString << "\tApple push conn status: OK";
                curl_easy_cleanup(curl);
                curl_slist_free_all(chunk);
                return true;
            }
        } catch (std::exception &e) {
            LOG(INFO) << e.what();
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
            return false;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);

    }
    return false;
}

void ApplePushModel::loadApiKey() {
    std::ifstream ifsPem("/certs/apple/key.pem");
    std::ifstream ifsSettings("/certs/apple/settings.json");
    std::string pemContent((std::istreambuf_iterator<char>(ifsPem)), (std::istreambuf_iterator<char>()));
    std::string settingsContent((std::istreambuf_iterator<char>(ifsSettings)), (std::istreambuf_iterator<char>()));
    if (pemContent.length() && settingsContent.length()) {
        mPem = pemContent;
        Json::Reader reader;
        Json::Value obj;
        reader.parse(settingsContent, obj);
        if (obj.isMember("appId") && obj.isMember("teamId") && obj.isMember("key")) {
            std::string appId = std::move(obj["appId"].asString());
            std::string teamId = std::move(obj["teamId"].asString());
            std::string key = std::move(obj["key"].asString());
            if (appId.length() && teamId.length() && key.length()) {
                mAppId = std::move(appId);
                mTeamId = std::move(teamId);
                mKey = std::move(key);
            } else {
                LOG(ERROR) << "Error JSON data invalid";
                exit(EXIT_FAILURE);
            }
        }

    } else {
        LOG(ERROR) << "Error loading APNS credentials, check if the settings.json, and key.pem exists";
        exit(EXIT_FAILURE);
    }
}
