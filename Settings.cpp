//
// Created by armin on 21.04.19.
//

#include <cstdlib>
#include <string>
#include <regex>
#include <proxygen/lib/utils/UtilInl.h>
#include "Settings.h"

std::string Settings::mForwardGatewayUrl;
bool Settings::mForwardGatewayEnabled = false;
std::string Settings::mFcmServerKey;
std::string Settings::mApnsPrivateKey;
std::string Settings::mApnsTeamId;
std::string Settings::mApnsKey;
std::string Settings::mApnsAppId;

void Settings::init() {
    std::regex newLine("([\\n]+)");

    auto enableForwardGateway = std::getenv("FORWARD_GATEWAY_ENABLE");
    if(enableForwardGateway && std::string(enableForwardGateway) == "TRUE"){
        mForwardGatewayEnabled = true;
    }
    auto forwardGatewayUrl = std::getenv("FORWARD_GATEWAY_URL");
    if(forwardGatewayUrl){
        mForwardGatewayUrl = std::regex_replace(std::string(forwardGatewayUrl),newLine,"");
    }else{
        mForwardGatewayUrl = "https://gateway.rocket.chat";
    }

    auto fcmServerKey = std::getenv("FCM_SERVER_KEY");
    if(fcmServerKey){
        mFcmServerKey = std::regex_replace(std::string(fcmServerKey),newLine,"");
    }

    auto apnsPrivateKey = std::getenv("APNS_PRIVATE_KEY");
    if(apnsPrivateKey){
        mApnsPrivateKey = std::string(apnsPrivateKey);
    }

    auto apnsTeamId = std::getenv("APNS_TEAM_ID");
    if(apnsTeamId){
        mApnsTeamId = std::string(apnsTeamId);
    }

    auto apnsKey = std::getenv("APNS_KEY");
    if(apnsKey){
        mApnsKey = std::string(apnsKey);
    }

    auto apnsAppId = std::getenv("APNS_APPID");
    if(apnsAppId){
        mApnsPrivateKey = std::string(apnsAppId);
    }
}

bool Settings::forwardGatewayEnabled() {
    return mForwardGatewayEnabled;
}

const std::string &Settings::forwardGatewayUrl() {
    return mForwardGatewayUrl;
}

const std::string &Settings::fcmServerKey() {
    return mFcmServerKey;
}

const std::string &Settings::apnsPrivateKey() {
    return mApnsPrivateKey;
}

const std::string &Settings::apnsTeamId() {
    return mApnsTeamId;
}

const std::string &Settings::apnsAppId() {
    return mApnsAppId;
}

const std::string &Settings::apnsKey() {
    return mApnsKey;
}
