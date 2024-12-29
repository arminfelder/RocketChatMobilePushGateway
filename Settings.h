//
// Created by armin on 21.04.19.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H
#define ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H

#include <string>

class Settings {

    typedef struct
    {
        std::string type;
        std::string project_id;
        std::string private_key_id;
        std::string private_key;
        std::string client_email;
        std::string client_id;
        std::string auth_uri;
        std::string token_uri;
        std::string auth_provider_x509_cert_url;
        std::string client_x509_cert_url;
        std::string universe_domain;
    } GoogleServiceAccount;
public:
    static void init();

    static bool setGoogleCredentialsFromServiceAccountJson(const std::string &pServiceAccountJson);
    static bool forwardGatewayEnabled();
    static const std::string &forwardGatewayUrl();
    static const GoogleServiceAccount &fcmServiceAccount();
    static const std::string &fcmProjectId();
    static const std::string &fcmApiEndpoint();
    static const std::string &apnsPrivateKey();
    static const std::string &apnsTeamId();
    static const std::string &apnsAppId();
    static const std::string &apnsKey();
    static const std::string &apnsPrivateKeyAlgo();

private:
    static void loadApnKeyFile();
    static void loadApnSettingsFile();
    static void loadFcmServiceAccountJson();
    static bool mForwardGatewayEnabled;
    static std::string mForwardGatewayUrl;
    static GoogleServiceAccount mFcmServiceAccount;
    static std::string mFcmProjectId;
    static std::string mFcmApiEndpoint;
    static std::string mApnsPrivateKey;
    static std::string mApnsTeamId;
    static std::string mApnsKey;
    static std::string mApnsAppId;
    static std::string mApnsPrivateKeyAlgo;
    static std::string mConfigDir;
};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H
