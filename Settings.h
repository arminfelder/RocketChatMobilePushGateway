//
// Created by armin on 21.04.19.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H
#define ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H


class Settings {

public:
    static void init();

    static bool forwardGatewayEnabled();
    static const std::string &forwardGatewayUrl();
    static const std::string &fcmServerKey();
    static const std::string &apnsPrivateKey();
    static const std::string &apnsTeamId();
    static const std::string &apnsAppId();
    static const std::string &apnsKey();

private:
    static void loadApnKeyFile();
    static void loadApnSettingsFile();
    static void loadFcmServerKeyFile();
    static bool mForwardGatewayEnabled;
    static std::string mForwardGatewayUrl;
    static std::string mFcmServerKey;
    static std::string mApnsPrivateKey;
    static std::string mApnsTeamId;
    static std::string mApnsKey;
    static std::string mApnsAppId;
};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H
