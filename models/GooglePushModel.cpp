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
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <regex>

#include <jsoncpp/json/json.h>

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
#include "ForwardGatewayModel.h"
#include "../Settings.h"

GooglePushModel::Token GooglePushModel::mAccessToken;
std::mutex GooglePushModel::mTokenMutex;

GooglePushModel::GooglePushModel(const std::string &pJson) {
    if (!pJson.empty()) {
        Json::Value obj;
        Json::Reader reader;
        reader.parse(pJson, obj);

        if (obj.isMember("token") && obj.isMember("options")) {
            Json::FastWriter fast;

            const Json::Value& options = obj["options"];

            const Json::Value& apn = options["gcm"];
            std::string test(fast.write(options));
            if (apn.isMember("text")) {
                std::string temp = apn["text"].asString();
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
                mTitle = options["title"].asString();
            }
            if (options.isMember("text")) {
                mText = options["text"].asString();
            }
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

int GooglePushModel::trace(CURL *handle, curl_infotype type,
                                  char *data, size_t size,
                                  void *userp){

    std::ignore = size;

    const char *text;
    (void)handle; /* prevent compiler warning */
    (void)userp;

    switch (type) {
        case CURLINFO_TEXT:
            LOG(ERROR) << "== Info:" <<  data << std::endl;
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

size_t GooglePushModel::curlFcmWriteCallback(void *buffer, size_t size, size_t nmemb,
                        void* this_ptr)
{
    std::ignore = size;
    const auto thiz= static_cast<GooglePushModel*>(this_ptr);
    if(buffer != nullptr) {
        Json::Value obj;
        Json::Reader reader;
        const std::string bufferString(static_cast<char*>(buffer),nmemb);
        reader.parse(bufferString, obj);
        if (obj.isMember("failure") && obj.isMember("success") && obj.isMember("results") && obj["failure"].asBool()) {
            auto results = obj["results"];
            for(const auto &elem: results){
                if(elem.isMember("error") && elem["error"].asString() == "MismatchSenderId"){
                    ForwardGatewayModel::claimRegistrationId(thiz->mDeviceToken);
                    thiz->mReturnStatusCode = 406;
                    return 0;
                }
            }
        }
    }
    return nmemb;
}

bool GooglePushModel::sendMessage() {

    if(ForwardGatewayModel::ownsRegistrationId(mDeviceToken)){
        return false;
    }

    Json::Value root, messageData, notificationData, androidData, androidNotification;

    // Construct message data
    messageData["title"] = mTitle;
    messageData["body"] = mText;
    messageData["message"] = mText;
    messageData["ejson"] = mPayload;

    // Construct notification data
    notificationData["title"] = mTitle;
    notificationData["body"] = mText;

    // Construct Android-specific notification data
    androidNotification["notification_count"] = mBadge;
    androidNotification["sound"] = mSound;

    androidData["collapseKey"] = mFrom;
    androidData["priority"] = 10;
    androidData["notification"] = androidNotification;

    // Combine all data into the final object
    root["notification"] = notificationData;
    root["data"] = messageData;
    root["token"] = mDeviceToken;
    root["android"] = androidData;

    Json::Value finalMessage;
    finalMessage["message"] = root;

    Json::FastWriter fastWriter;
    const std::string serializedMessage = fastWriter.write(finalMessage);

    using namespace date;
    using namespace std::chrono;
    const auto uuidObj = boost::uuids::random_generator()();
    const auto uuidString = boost::lexical_cast<std::string>(uuidObj);


    auto cleanedObj = finalMessage;
    cleanedObj["data"] = "---removed from log---";
    LOG(INFO) << uuidString << "\tGoogle push data\t" << cleanedObj;


    std::shared_ptr<CURL> curl(curl_easy_init(),curl_easy_cleanup);
    if (curl) {

        auto chunk = curl_slist_append(nullptr, std::string("Authorization: Bearer " + getAccessToken()).c_str());
        chunk = curl_slist_append(chunk, "Content-Type: application/json; UTF-8");

        curl_easy_setopt(curl.get(), CURLOPT_DEBUGFUNCTION, trace );
        curl_easy_setopt(curl.get(), CURLOPT_VERBOSE, false);
        curl_easy_setopt(curl.get(), CURLOPT_URL,Settings::fcmApiEndpoint().c_str());
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, serializedMessage.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE,serializedMessage.size());
        curl_easy_setopt(curl.get(), CURLOPT_POST, true);
        curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl.get(), CURLOPT_USE_SSL, true);
        curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl.get(), CURLOPT_TCP_KEEPALIVE, true);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, curlFcmWriteCallback );
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, this);

        const CURLcode res = curl_easy_perform(curl.get());
        long http_code;
        curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code < 200 || http_code >= 300) {
            if(res == CURLE_WRITE_ERROR){
                LOG(INFO) << uuidString << "\tGoogle push device token rejected, forward message to forwardgateway";
            }else{
                LOG(ERROR) << uuidString << "\tGoogle push conn error: " << curl_easy_strerror(res);
                mReturnStatusCode = 500;
            }
        } else {
            DLOG(ERROR) << uuidString << "\tGoogle push conn status: OK";
            curl_slist_free_all(chunk);
            mReturnStatusCode = 200;
            return true;
        }
        curl_slist_free_all(chunk);

    }else{
        mReturnStatusCode = 500;
    }
    return false;
}

void GooglePushModel::init() {

}

int GooglePushModel::returnStatusCode() const {
    return mReturnStatusCode;
}

const std::string& GooglePushModel::getAccessToken()
{
    const auto now = std::chrono::system_clock::now();
    const bool isTokenExpired = (now >= mAccessToken.expires_in);

    if (isTokenExpired)
    {
        std::lock_guard<std::mutex> lock(mTokenMutex);

        if (now >= mAccessToken.expires_in)
        {
            requestAccessToken();
        }
    }

    return mAccessToken.token;
}

std::string getGoogleAuthJWT()
{
    auto now = std::chrono::system_clock::now();
    auto now_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();

    auto expiry = now + std::chrono::hours(1);
    auto expiry_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
    expiry.time_since_epoch()
).count();


    auto test =std::chrono::system_clock::now() + std::chrono::seconds(3600);
    jwt::jwt_object jwtObj{
        jwt::params::algorithm("RS256"),
        jwt::params::headers({
            {
                "alg", "RS256"
            },
            {
                "kid", Settings::fcmServiceAccount().private_key_id
            }
        }),
        jwt::params::secret(Settings::fcmServiceAccount().private_key),
    };

    jwtObj.add_claim("iss", Settings::fcmServiceAccount().client_email);
    jwtObj.add_claim("scope", "https://www.googleapis.com/auth/firebase.messaging");
    jwtObj.add_claim("aud", Settings::fcmServiceAccount().token_uri);
    jwtObj.add_claim("exp", expiry_timestamp);
    jwtObj.add_claim("iat", now_timestamp);

    auto signedJwt = jwtObj.signature();

    return signedJwt;
}

bool GooglePushModel::requestAccessToken()
{
    const auto token = getGoogleAuthJWT();

    const std::shared_ptr<CURL> curl(curl_easy_init(),curl_easy_cleanup);
    if (curl) {
        curl_slist *chunk = nullptr;


        std::string buffer;
        std::string postfields = "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion="+token;
        curl_easy_setopt(curl.get(), CURLOPT_DEBUGFUNCTION, trace );
        curl_easy_setopt(curl.get(), CURLOPT_VERBOSE, false);
        curl_easy_setopt(curl.get(), CURLOPT_URL,Settings::fcmServiceAccount().token_uri.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_POST, true);
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, postfields.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(postfields.size()));
        curl_easy_setopt(curl.get(), CURLOPT_USE_SSL, true);
        curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl.get(), CURLOPT_TCP_KEEPALIVE, true);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &buffer );
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION,  +[](char *buffer, size_t size, size_t nmemb, void *userdata) {
                    // invoke the member function via userdata
                    const auto responseBuffer = static_cast<std::string*>(userdata);
                    responseBuffer->append(buffer,size * nmemb);
            return nmemb;
                 });


        CURLcode res = curl_easy_perform(curl.get());
        long http_code;
        curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code < 200 || http_code >= 300){
                LOG(INFO) << "\tGoogle push auth rejected";
        } else {
            if(!buffer.empty()) {
                Json::Reader reader;
                Json::Value obj;
                reader.parse(buffer, obj);

                if ( obj.isMember("access_token") && obj.isMember("expires_in") )
                {
                    mAccessToken.token = obj["access_token"].asString();
                    auto now = std::chrono::system_clock::now();

                    auto expiry = now + std::chrono::seconds(obj["expires_in"].asInt()) - std::chrono::seconds(10);
                    mAccessToken.expires_in = expiry;
                }
            }

            curl_slist_free_all(chunk);
            mReturnStatusCode = 200;
            return true;
        }
        curl_slist_free_all(chunk);

    }else{
        DLOG(ERROR) << "\tFailed to initialize CURL";
    }
    return false;

}
