//
// Created by armin on 21.04.19.
//

#include <cstdlib>
#include <string>
#include <regex>
#include <proxygen/lib/utils/UtilInl.h>
#include "Settings.h"
#include <jsoncpp/json/json.h>
#include <fstream>
#include <glog/logging.h>

std::string Settings::mForwardGatewayUrl;
bool Settings::mForwardGatewayEnabled = false;
std::string Settings::mFcmServerKey;
std::string Settings::mApnsPrivateKey;
std::string Settings::mApnsTeamId;
std::string Settings::mApnsKey;
std::string Settings::mApnsAppId;

void Settings::init() {
    std::regex newLine("([\\n]+)");

    loadApnKeyFile();
    loadApnSettingsFile();
    loadFcmServerKeyFile();

    auto enableForwardGateway = std::getenv("FORWARD_GATEWAY_ENABLE");
    if(enableForwardGateway && std::string(enableForwardGateway) == "TRUE"){
        mForwardGatewayEnabled = true;
    }
    auto forwardGatewayUrl = std::getenv("FORWARD_GATEWAY_URL");
    if(forwardGatewayUrl){
        mForwardGatewayUrl = std::regex_replace(std::string(forwardGatewayUrl),newLine,"");
        LOG(INFO) << "Settings: Custom Forward Gateway enabled";
    }else{
        mForwardGatewayUrl = "https://gateway.rocket.chat";
        LOG(INFO) << "Settings: Using default Forward Gateway";
    }

    auto fcmServerKey = std::getenv("FCM_SERVER_KEY");
    if(fcmServerKey){
        mFcmServerKey = std::regex_replace(std::string(fcmServerKey),newLine,"");
        LOG(INFO) << "Settings: Using FCM Server Key from env";
    }

    auto apnsPrivateKey = std::getenv("APNS_PRIVATE_KEY");
    if(apnsPrivateKey){
        mApnsPrivateKey = std::string(apnsPrivateKey);
        LOG(INFO) << "Settings: Using APNs Private Key from env";
    }

    auto apnsTeamId = std::getenv("APNS_TEAM_ID");
    if(apnsTeamId){
        mApnsTeamId = std::string(apnsTeamId);
        LOG(INFO) << "Settings: Using APNs TeamID from env";
    }

    auto apnsKey = std::getenv("APNS_KEY");
    if(apnsKey){
        mApnsKey = std::string(apnsKey);
        LOG(INFO) << "Settings: Using APNs Key from env";
    }

    auto apnsAppId = std::getenv("APNS_APPID");
    if(apnsAppId){
        mApnsAppId = std::string(apnsAppId);
        LOG(INFO) << "Settings: Using APNs AppID from env";
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

void Settings::loadApnKeyFile() {
    std::ifstream ifsPem("/certs/apple/key.pem");
    std::string content((std::istreambuf_iterator<char>(ifsPem)), 
                        (std::istreambuf_iterator<char>()));
    if (content.length()) {
        mApnsPrivateKey = content;
    } else {
        LOG(INFO) << "Settings: No APN key file found at /certs/apple/key.pem";
    }
}

void Settings::loadApnSettingsFile() {
    std::ifstream ifsSettings("/certs/apple/settings.json");
    std::string content((std::istreambuf_iterator<char>(ifsSettings)), 
                        (std::istreambuf_iterator<char>()));
    if(content.length()) {
        Json::Reader reader;
        Json::Value obj;
        reader.parse(content, obj);
        if (obj.isMember("appId") && obj.isMember("teamId") && obj.isMember("key")) {
            std::string appId = obj["appId"].asString();
            std::string teamId = obj["teamId"].asString();
            std::string key = obj["key"].asString();
            if (appId.length() && teamId.length() && key.length()) {
                mApnsAppId = std::move(appId);
                mApnsTeamId = std::move(teamId);
                mApnsKey = std::move(key);
            } else {
                LOG(ERROR) << "Error JSON data invalid";
                exit(EXIT_FAILURE);
            }
        }
    } else {
        LOG(INFO) << "Settings: No APN settings file found at /certs/apple/settings.json";
    }
}

void Settings::loadFcmServerKeyFile() {
    std::ifstream ifs("/certs/google/serverKey.txt");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    if (content.length()) {
        std::regex newLine("([\\n]+)");
        mFcmServerKey = std::regex_replace(content,newLine,"");
    } else {
        LOG(INFO) << "Settings: No Google Push Key file found at /certs/google/serverKey.txt";
    }
}
