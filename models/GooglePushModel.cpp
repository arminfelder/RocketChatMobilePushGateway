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

#define UNUSED(x) (void)x;

#include <fstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <regex>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <folly/FileUtil.h>
#include <folly/String.h>
#include <folly/io/async/SSLContext.h>
#include <folly/io/async/SSLOptions.h>
#include <folly/portability/GFlags.h>
#include <proxygen/lib/http/HTTPMessage.h>
#include <proxygen/lib/http/session/HTTPUpstreamSession.h>
#include <proxygen/lib/http/codec/HTTP2Codec.h>

#include "GooglePushModel.h"
#include "../date.h"

std::string GooglePushModel::mApiKey;

void GooglePushModel::loadApiKey() {
    std::ifstream ifs("/certs/google/serverKey.txt");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    if (content.length()) {
        std::regex newLine("([\\n]+)");
        GooglePushModel::mApiKey = std::regex_replace(content,newLine,"");
    } else {
        std::cout << "Error loading Google Push Key: file: /certs/google/serverKey.txt is empty or does not exist";
        exit(EXIT_FAILURE);
    }
}

GooglePushModel::GooglePushModel(const std::string &pJson) {
    Json::Reader reader;
    Json::Value obj;
    Json::FastWriter fast;

    if (pJson.length()) {
        reader.parse(pJson, obj);

        if (obj.isMember("token") && obj.isMember("options")) {

            Json::Value options = obj["options"];

            Json::Value apn = options["gcm"];
            std::string test(fast.write(options));
            if (apn.isMember("text")) {
                std::string temp = std::move(apn["text"].asString());
                unsigned long index = 0;
                while (true){
                    index = temp.find('\n',index);
                    if (index == std::string::npos) {
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

int GooglePushModel::trace(CURL *handle, curl_infotype type,
                                  char *data, size_t size,
                                  void *userp){

    UNUSED(size);

    const char *text;
    (void)handle; /* prevent compiler warning */
    (void)userp;

    switch (type) {
        case CURLINFO_TEXT:
        std::cerr << "== Info:" <<  data << std::endl;
        default: /* in case a new one is introduced to shock us */
            return 0;

        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
    }

    std::cerr << text << ": " << data << std::endl;

    return 0;
}

bool GooglePushModel::sendMessage() {


    Json::Value obj;
    Json::Value msg;
    msg["title"] = mTitle;
    msg["body"] = mText;
    msg["message"] = mText;
    msg["ejson"] = mPayload;
    msg["msgcnt"] = mBadge;

    obj["to"] = mDeviceToken;
    obj["data"] = msg;
    obj["priority"] = 10;
    obj["collapseKey"] = mFrom;

    Json::FastWriter fast;

    std::string data = fast.write(obj);

    CURL *curl;
    CURLcode res;

    using namespace date;
    using namespace std::chrono;
    boost::uuids::uuid uuidObj = boost::uuids::random_generator()();
    std::string uuidString = boost::lexical_cast<std::string>(uuidObj);

    std::cout << "[" << system_clock::now() << "]\t" << uuidString << "\tGoogle push data\t" << data << std::endl;

    curl = curl_easy_init();
    if (curl) {

        struct curl_slist *chunk = nullptr;

        chunk = curl_slist_append(chunk, std::string("Authorization: key="+mApiKey).c_str());
        chunk = curl_slist_append(chunk, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, trace );
        curl_easy_setopt(curl, CURLOPT_VERBOSE, false);
        curl_easy_setopt(curl, CURLOPT_URL,mApiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,data.size());
        curl_easy_setopt(curl, CURLOPT_POST, true);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, true);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, true);

        std::cout << "[" << system_clock::now() << "]\t" << uuidString << "\tGoogle push result\t";
        res = curl_easy_perform(curl);
        std::cout << std::endl;

        if (res != CURLE_OK) {
            std::cerr << "[" << system_clock::now() << "]\t" << uuidString << "\tGoogle push conn error: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "[" << system_clock::now() << "]\t" << uuidString << "\tGoogle push conn status: OK" << std::endl;
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
            return true;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);

    }
    return false;
}
