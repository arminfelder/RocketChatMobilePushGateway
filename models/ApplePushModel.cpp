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
#include <exception>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <jwt-cpp/jwt.h>
#include <cpr/api.h>
#include <glog/logging.h>

// Project-specific headers
#include "../Settings.h"
#include "ApplePushModel.h"
#include "ForwardGatewayModel.h"


std::string ApplePushModel::mPem;
std::string ApplePushModel::mTeamId;
std::string ApplePushModel::mAppId;
std::string ApplePushModel::mKey;
std::string ApplePushModel::mPrivateKeyAlgo;

ApplePushModel::ApplePushModel(const std::shared_ptr<Json::Value>& pJson): mReturnStatusCode(0)
{
    if (pJson)
    {
        if (pJson->isMember("token") && pJson->isMember("options"))
        {
            Json::Value options = (*pJson)["options"];

            if (options.isMember("text"))
            {
                std::string temp = options["text"].asString();
                unsigned long index = 0;
                while (true)
                {
                    index = temp.find('\n', index);
                    if (index == std::string::npos)
                    {
                        break;
                    }
                    temp.replace(index, 1, "\\r\\n");
                    index += 4;
                }
                mText = std::move(temp);
            }
            if (options.isMember("title"))
            {
                mTitle = options["title"].asString();
            }
            if (options.isMember("from"))
            {
                mFrom = options["from"].asString();
            }
            if (options.isMember("badge"))
            {
                mBadge = options["badge"].asInt();
            }
            if (options.isMember("payload"))
            {
                Json::FastWriter fast;
                mPayload = fast.write(options["payload"]);
            }
            if (options.isMember("topic"))
            {
                mTopic = options["topic"].asString();
            }
            if (options.isMember("sound"))
            {
                mSound = options["sound"].asString();
            }
            mDeviceToken = (*pJson)["token"].asString();
        }
    }
}

std::string ApplePushModel::getPayload( const uuid_t &uuid) const
{
    Json::Value obj, aps, alert;
    Json::FastWriter fast;

    alert["body"] = mText;
    alert["title"] = mTitle;

    aps["category"] = "MESSAGE";
    aps["badge"] = mBadge;
    aps["sound"] = mSound;
    aps["alert"] = alert;

    obj["aps"] = aps;

    std::string payload = fast.write(obj);



    auto cleanedObj = obj;
    cleanedObj["alert"] = "---removed from log---";
    cleanedObj["ejson"] = "---removed from log---";
    LOG(INFO) << uuid << "\tApple push data\t" << cleanedObj;
    LOG(INFO) << uuid << "\tAPNS token:\t" << mDeviceToken;

    return payload;
}

bool ApplePushModel::sendMessage()
{
    if (ForwardGatewayModel::ownsRegistrationId(mDeviceToken))
    {
        return false;
    }

    const auto now = std::chrono::system_clock::now();
    const auto expiry = now + std::chrono::hours(1);

    const auto token = jwt::create().set_type("JWT")
                              .set_issued_at(now)
                              .set_expires_at(expiry)
                              .set_key_id(mKey)
                              .set_issuer(mTeamId)
                              .sign(jwt::algorithm::hs256{mPem});

    uuid_t uuid;
    uuid_generate(uuid);

    const auto response = cpr::Post(cpr::Url{mApiUrl + mDeviceToken},
                                    cpr::Header{
                                        {"Content-Type", "application/json"},
                                        {"Authorization", "Bearer " + token},
                                        {"apns-id", std::string(reinterpret_cast<char*>(uuid))},
                                        {"apns-expiration", "0"},
                                        {"apns-priority", "10"},
                                        {"apns-topic", mTopic},
                                        {"apns-push-type", "alert"},
                                        {"apns-collapse-id", mFrom}
                                    },
                                    cpr::HttpVersion{cpr::HttpVersionCode::VERSION_2_0_TLS},
                                    cpr::Body{getPayload(uuid)});

    if (response.status_code == 200)
    {
        Json::Reader reader;
        Json::Value responseObj;

        reader.parse(response.text, responseObj);
        if (responseObj.isMember("reason") && responseObj["reason"].asString() == "DeviceTokenNotForTopic")
        {
            ForwardGatewayModel::claimRegistrationId(mDeviceToken);
            LOG(INFO) << "MismatchSenderId mark if for forward gateway";
            mReturnStatusCode = 406;
            return false;
        }
        mReturnStatusCode = 200;
        return true;
    }
    LOG(ERROR) << "failed to send notification, Error:" << response.text;
    mReturnStatusCode = 500;
    return false;
}

void ApplePushModel::init()
{
    mAppId = Settings::apnsAppId();
    mPem = Settings::apnsPrivateKey();
    mTeamId = Settings::apnsTeamId();
    mKey = Settings::apnsKey();
    mPrivateKeyAlgo = Settings::apnsPrivateKeyAlgo();
}

int ApplePushModel::returnStatusCode() const
{
    return mReturnStatusCode;
}