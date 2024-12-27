//
// Created by armin on 21.04.19.
//

#include <cstdlib>
#include <string>
#include <regex>
#include <proxygen/lib/utils/UtilInl.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <glog/logging.h>

#include "Settings.h"

std::string Settings::mForwardGatewayUrl;
bool Settings::mForwardGatewayEnabled = false;
Settings::GoogleServiceAccount Settings::mFcmServiceAccount;
std::string Settings::mFcmProjectId;
std::string Settings::mFcmApiEndpoint;
std::string Settings::mApnsPrivateKey;
std::string Settings::mApnsTeamId;
std::string Settings::mApnsKey;
std::string Settings::mApnsAppId;
std::string Settings::mApnsPrivateKeyAlgo = "ES256";

void Settings::init() {
    const std::regex newLine("([\\n]+)");

    loadApnKeyFile();
    loadApnSettingsFile();
    loadFcmServiceAccountJson();

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

    auto fcmServiceAccountJson = std::getenv("FCM_SERVICE_ACCOUNT_JSON");
    if(fcmServiceAccountJson){
        setGoogleCredentialsFromServicAccountJson(std::string(fcmServiceAccountJson));
        LOG(INFO) << "Settings: Using FCM ServiceAssount from env";
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

    auto apnsPrivateKeyAlgo = std::getenv("APNS_PRIVATE_KEY_ALGO");
    if(apnsPrivateKeyAlgo){
        mApnsPrivateKeyAlgo = std::string(apnsPrivateKeyAlgo);
        LOG(INFO) << "Settings: Using APNs Private Key Algo from env";
    }

    mFcmApiEndpoint = "https://fcm.googleapis.com/v1/projects/" + mFcmServiceAccount.project_id + "/messages:send";
}

bool Settings::setGoogleCredentialsFromServicAccountJson(const std::string& pServiceAccountJson)
{
    Json::Reader reader;
    Json::Value obj;
    reader.parse(pServiceAccountJson, obj);
    if (obj.isMember("type") && obj.isMember("project_id") && obj.isMember("private_key_id") && obj.isMember("private_key") && obj.isMember("client_email") && obj.isMember("client_id") && obj.isMember("auth_uri") && obj.isMember("token_uri") && obj.isMember("auth_provider_x509_cert_url") && obj.isMember("client_x509_cert_url") && obj.isMember("universe_domain")) {
        std::string type = obj["type"].asString();
        std::string projectId = obj["project_id"].asString();
        std::string privateKeyId = obj["private_key_id"].asString();
        std::string privateKey = obj["private_key"].asString();
        std::string clientEmail = obj["client_email"].asString();
        std::string clientId = obj["client_id"].asString();
        std::string authUri = obj["auth_uri"].asString();
        std::string tokenUri = obj["token_uri"].asString();
        std::string authProviderX509CertUrl = obj["auth_provider_x509_cert_url"].asString();
        std::string clientX509CertUrl = obj["client_x509_cert_url"].asString();
        std::string universeDomain = obj["universe_domain"].asString();
        if (type.length() && projectId.length() && privateKeyId.length() && privateKey.length() && clientEmail.length() && clientId.length() && authUri.length() && tokenUri.length() && authProviderX509CertUrl.length() && clientX509CertUrl.length()) {
            mFcmServiceAccount.project_id = std::move(projectId);
            mFcmServiceAccount.type = std::move(type);
            mFcmServiceAccount.private_key_id = std::move(privateKeyId);
            mFcmServiceAccount.private_key = std::move(privateKey);
            mFcmServiceAccount.client_email = std::move(clientEmail);
            mFcmServiceAccount.client_id = std::move(clientId);
            mFcmServiceAccount.auth_uri = std::move(authUri);
            mFcmServiceAccount.token_uri = std::move(tokenUri);
            mFcmServiceAccount.auth_provider_x509_cert_url = std::move(authProviderX509CertUrl);
            mFcmServiceAccount.client_x509_cert_url = std::move(clientX509CertUrl);
            mFcmServiceAccount.universe_domain = std::move(universeDomain);
            return true;
        }
        LOG(ERROR) << "failed to parse ServiceAccount JSON, keys missing";
    }

    return false;
}

bool Settings::forwardGatewayEnabled() {
    return mForwardGatewayEnabled;
}

const std::string &Settings::forwardGatewayUrl() {
    return mForwardGatewayUrl;
}

const Settings::GoogleServiceAccount& Settings::fcmServiceAccount()
{
    return mFcmServiceAccount;
}

const std::string& Settings::fcmProjectId() {
    return mFcmProjectId;
}

const std::string& Settings::fcmApiEndpoint()
{
    return mFcmApiEndpoint;
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

const std::string &Settings::apnsPrivateKeyAlgo() {
    return mApnsPrivateKeyAlgo;
}

void Settings::loadApnKeyFile() {
    std::ifstream ifsPem("/certs/apple/key.pem");
    std::string content((std::istreambuf_iterator<char>(ifsPem)), 
                        (std::istreambuf_iterator<char>()));
    if (content.length()) {
        LOG(INFO) << "Settings: Using APN key file";
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
        LOG(INFO) << "Settings: Using APN settings file";
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

void Settings::loadFcmServiceAccountJson() {
    std::ifstream ifs("/certs/google/serviceAccount.json");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    if (content.length()) {
        LOG(INFO) << "Settings: Using FCM ServiceAccount JSON";
        if (!setGoogleCredentialsFromServicAccountJson(content))
        {
            LOG(ERROR) << "failed to parse ServiceAccount JSON";
        }

    } else {
        LOG(INFO) << "Settings: No FCM ServiceAccount JSON file found at /certs/google/serviceAccount.json";
    }
}
