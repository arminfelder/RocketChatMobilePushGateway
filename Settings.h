//
// Created by armin on 21.04.19.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H
#define ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H


class Settings {

public:
    static void init();

    bool forwardGatewayEnabaled() const;
    const std::string &forwardGatewayUrl() const;

private:
    static bool mForwardGatewayEnabled;
    static std::string mForwardGatewayUrl;
};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_SETTINGS_H
