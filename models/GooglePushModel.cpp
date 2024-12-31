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
#include <fstream>     // For file handling
#include <iostream>    // For standard input/output

// Third-party libraries
#include <jwt-cpp/jwt.h>     // For JSON Web Token functionality
#include "cpr/api.h"         // For HTTP requests
#include "cpr/response.h"
#include "drogon/HttpClient.h" // For HTTP client handling in Drogon
#include "glog/logging.h"      // For Google logging

// Project-specific includes
#include "GooglePushModel.h"

#include <uuid/uuid.h>

#include "ForwardGatewayModel.h"
#include "../Settings.h"

GooglePushModel::Token GooglePushModel::mAccessToken;
std::mutex GooglePushModel::mTokenMutex;

GooglePushModel::GooglePushModel(const std::shared_ptr<Json::Value>& pJson): mReturnStatusCode(0)
{
    if (pJson)
    {
        if (pJson->isMember("token") && pJson->isMember("options"))
        {
            Json::FastWriter fast;

            const Json::Value& options = (*pJson)["options"];

            const Json::Value& apn = options["gcm"];
            std::string test(fast.write(options));
            if (apn.isMember("text"))
            {
                std::string temp = apn["text"].asString();
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
                mApn = std::move(temp);
            }
            if (options.isMember("title"))
            {
                mTitle = options["title"].asString();
            }
            if (options.isMember("text"))
            {
                mText = options["text"].asString();
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

/**
 * Retrieves the payload data associated with a specific operation or request.
 *
 * This method is used to extract or access the payload content, which typically contains
 * the main data or message to be processed or transmitted.
 *
 * @return A string containing the payload data if available and successfully retrieved.
 *         Returns an empty string or an error message in case the payload is unavailable or retrieval fails.
 */
std::string GooglePushModel::getPayload() const
{
    Json::Value message, messageData, notificationData, android, androidNotification;

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

    android["collapseKey"] = mFrom;
    android["priority"] = "HIGH";
    android["notification"] = androidNotification;

    // Combine all data into the final object
    message["notification"] = notificationData;
    message["data"] = messageData;
    message["token"] = mDeviceToken;
    message["android"] = android;
    message["badge"] = mBadge;

    Json::Value payloadJson;
    payloadJson["message"] = message;

    Json::FastWriter fastWriter;
    std::string payload = fastWriter.write(payloadJson);

    uuid_t uuid;
    uuid_generate(uuid);

    auto cleanedObj = payloadJson;
    cleanedObj["message"]["data"] = "---removed from log---";
    cleanedObj["message"]["notification"] = "---removed from log---";
    cleanedObj["message"]["token"] = "---removed from log---";

    LOG(INFO) << uuid << "\tGoogle push data\t" << cleanedObj;
    return payloadJson;
}

bool GooglePushModel::sendMessage() {

    if(ForwardGatewayModel::ownsRegistrationId(mDeviceToken)){
        return false;
    }

    const auto response = cpr::Post(cpr::Url{Settings::fcmApiEndpoint()},
                                    cpr::Header{
                                        {"Content-Type", "application/json; UTF-8"},
                                        {"Authorization", "Bearer " + getAccessToken()}
                                    },
                                    cpr::Body{getPayload()});

    if (response.status_code == 200)
    {
        Json::Value obj;
        Json::Reader reader;
        reader.parse(response.text, obj);
        if (obj.isMember("failure") && obj.isMember("success") && obj.isMember("results") && obj["failure"].asBool())
        {
            LOG(ERROR) << "failed to send notification, Error:" << response.text;
            for (auto results = obj["results"]; const auto& elem : results)
            {
                if (elem.isMember("error") && elem["error"].asString() == "MismatchSenderId")
                {
                    ForwardGatewayModel::claimRegistrationId(mDeviceToken);
                    LOG(INFO) << "MismatchSenderId mark if for forward gateway";
                    mReturnStatusCode = 406;
                    break;
                }
            }
            return false;
        }
        mReturnStatusCode = 200;
        return true;
    }
    LOG(ERROR) << "failed to send notification, Error:" << response.text;
    mReturnStatusCode = 500;
    return false;
}

void GooglePushModel::init()
{
}

int GooglePushModel::returnStatusCode() const
{
    return mReturnStatusCode;
}

/**
 * Obtains an access token required for authenticating requests to a specified service.
 *
 * This method generates or retrieves an access token used to access secure resources.
 * The token serves as a credential for verifying the identity and permissions of the client.
 *
 * @return A string containing the access token if successfully retrieved or generated.
 *         Returns an empty string or an error message in case of failure.
 */
const std::string& GooglePushModel::getAccessToken()
{
    if (std::chrono::system_clock::now() >= mAccessToken.expires_in)
    {
        std::lock_guard lock(mTokenMutex);
        if (std::chrono::system_clock::now() >= mAccessToken.expires_in)
        {
            requestAccessToken();
        }
    }
    return mAccessToken.token;
}

/**
 * Retrieves a Google authentication JSON Web Token (JWT) based on the required credentials.
 *
 * This method generates a signed JWT that is used for authenticating requests
 * to Google APIs. The token is typically used for service-to-service authentication
 * or other mechanisms requiring a secure, verifiable signature.
 *
 * @return A string containing the Google authentication JWT on successful generation.
 *         Returns an empty string or an error message if the process fails.
 */
std::string getGoogleAuthJWT()
{
    const auto now = std::chrono::system_clock::now();
    const auto expiry = now + std::chrono::hours(1);

    auto token = jwt::create().set_type("JWT")
                              .set_issuer(Settings::fcmServiceAccount().client_email)
                              .set_algorithm("RS256")
                              .set_payload_claim(
                                  "scope", picojson::value("https://www.googleapis.com/auth/firebase.messaging"))
                              .set_key_id(Settings::fcmServiceAccount().private_key_id)
                              .set_audience(Settings::fcmServiceAccount().token_uri)
                              .set_issued_at(now)
                              .set_expires_at(expiry)
                              .sign(jwt::algorithm::rs256{"", Settings::fcmServiceAccount().private_key});

    return token;
}

/**
 * Generates and retrieves an access token based on the provided authentication credentials.
 *
 * The method performs the necessary authentication process and returns a token
 * that can be used for accessing secured resources or APIs. The token typically
 * has a limited validity period based on the server's configuration or policies.
 *
 * @return A string containing the access token if authentication succeeds.
 *         Returns an empty string or relevant error code/message in case of failure.
 */

constexpr char CONTENT_TYPE[] = "application/x-www-form-urlencoded";
constexpr char GRANT_TYPE[] = "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=";

cpr::Response sendAccessTokenRequest(const std::string& endpoint, const std::string& jwtToken)
{
    return cpr::Post(
        cpr::Url{endpoint},
        cpr::Header{{"Content-Type", CONTENT_TYPE}},
        cpr::Body{GRANT_TYPE + jwtToken}
    );
}

bool GooglePushModel::requestAccessToken()
{
    const auto jwtToken = getGoogleAuthJWT();

    if (const auto httpResponse = sendAccessTokenRequest(Settings::fcmServiceAccount().token_uri, jwtToken); httpResponse.status_code == 200)
    {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(httpResponse.text, obj);

        if (obj.isMember("access_token") && obj.isMember("expires_in"))
        {
            mAccessToken.token = obj["access_token"].asString();
            const auto now = std::chrono::system_clock::now();
            const auto expiry = now + std::chrono::seconds(obj["expires_in"].asInt()) - std::chrono::seconds(10);
            mAccessToken.expires_in = expiry;

            return true;
        }
    }
    return false;

}
