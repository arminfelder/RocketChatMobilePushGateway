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

// Project-specific headers
#include "../Settings.h"
#include "ApplePushModel.h"
#include "ForwardGatewayModel.h"

const std::string ApplePushModel::mApiUrl{"https://api.push.apple.com/3/device/"};
ApplePushModel::Token ApplePushModel::mAuthToken;
std::mutex ApplePushModel::mTokenMutex;

std::string ApplePushModel::getPayload(const GatewayNotification& pNotification)
{
    Json::Value payloadJson, payloadField, aps, alert;
    Json::FastWriter fast;

    alert["body"] = pNotification.getText();
    alert["title"] = pNotification.getTitle();

    if (pNotification.getApn() && pNotification.getApn()->category)
    {
        aps["category"] = pNotification.getApn()->category.value();
    }

    if (pNotification.getBadge())
    {
        aps["badge"] = pNotification.getBadge().value();
    }

    if (pNotification.getSound())
    {
        aps["sound"] = pNotification.getSound().value();
    }
    aps["alert"] = alert;

    if (pNotification.getEJson())
    {
        payloadJson["ejson"] = fast.write(pNotification.getEJson().value());
    }

    if (pNotification.getNotId())
    {
        aps["thread-id"] = pNotification.getNotId().value();
    }

    aps["mutable-content"] = 1;

    payloadJson["messageFrom"] = pNotification.getFrom();

    payloadJson["aps"] = aps;

    std::string payload = fast.write(payloadJson);

    return payload;
}


std::string& ApplePushModel::getAppleAuthToken()
{
    if (std::chrono::system_clock::now() >= mAuthToken.expires_in)
    {
        std::lock_guard lock(mTokenMutex);
        if (std::chrono::system_clock::now() >= mAuthToken.expires_in)
        {
            try
            {
                const auto now = std::chrono::system_clock::now();
                const auto tokenExpiry = std::chrono::system_clock::now() + std::chrono::hours(1);
                mAuthToken.token = jwt::create().set_type("JWT")
                                                .set_issued_at(now)
                                                .set_expires_at(tokenExpiry)
                                                .set_key_id(Settings::apnsKey())
                                                .set_issuer(Settings::apnsTeamId())
                                                .sign(jwt::algorithm::es256{"", Settings::apnsPrivateKey()});
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }
    return mAuthToken.token;
}

bool ApplePushModel::sendMessage(const GatewayNotification& pNotification)
{
    if (ForwardGatewayModel::ownsRegistrationId(pNotification.getToken().value()))
    {
        return false;
    }

    auto const payload = getPayload(pNotification);

    auto token = getAppleAuthToken();
    if (token.empty())
    {
        return false;
    }

    uuid_t uuid;
    char uuid_str[37];
    uuid_generate(uuid);
    uuid_unparse(uuid, uuid_str);

    const auto messageExpiry = std::chrono::duration_cast<std::chrono::seconds>(
        (std::chrono::system_clock::now() + std::chrono::hours(1)).time_since_epoch());

    const auto& deviceToken = pNotification.getToken().value();
    const auto url = cpr::Url{mApiUrl + deviceToken};

    cpr::Session session;
    session.SetUrl(url);
    session.SetHeader(cpr::Header{
        {"Content-Type", "application/json"},
        {"Authorization", "Bearer " + token},
        {"apns-id", uuid_str},
        {"apns-expiration", std::to_string(messageExpiry.count())},
        {"apns-priority", "10"},
        {"apns-topic", Settings::apnsAppId()},
        {"apns-push-type", "alert"},
    });
    session.SetBody(payload);
    session.SetHttpVersion(cpr::HttpVersion{cpr::HttpVersionCode::VERSION_2_0_PRIOR_KNOWLEDGE});
    session.SetVerbose(Settings::getLogLevel() == trantor::Logger::LogLevel::kTrace ? true : false);

    auto response = session.Post();

    if (response.error.code == cpr::ErrorCode::OK)
    {
        if (response.status_code == 200)
        {
            Json::Reader reader;
            Json::Value responseObj;

            reader.parse(response.text, responseObj);
            if (responseObj.isMember("reason") && responseObj["reason"].asString() == "DeviceTokenNotForTopic")
            {
                ForwardGatewayModel::claimRegistrationId(pNotification.getToken().value());

                LOG_INFO << "MismatchSenderId mark if for forward gateway";
                return false;
            }
            return true;
        }
        else
        {
            LOG_ERROR << "failed to send notification, Error:" << response.text;
            return false;
        }
    }
    else
    {
        LOG_ERROR << "failed to send notification, Error:" << response.error.message;
        return false;;
    }
    LOG_ERROR << "failed to send notification, Error:" << response.text;
    return false;
}
