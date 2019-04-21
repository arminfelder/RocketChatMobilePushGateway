//
// Created by armin on 21.04.19.
//

#include <cstdlib>
#include <string>
#include <proxygen/lib/utils/UtilInl.h>
#include "Settings.h"

std::string Settings::mForwardGatewayUrl;
bool Settings::mForwardGatewayEnabled = false;

void Settings::init() {
    auto enableForwardGateway = std::getenv("FORWARD_GATEWAY_ENABLE");
    if(enableForwardGateway && std::string(enableForwardGateway) == "TRUE"){
        mForwardGatewayEnabled = true;
    }
    auto forwardGatewayUrl = std::getenv("FORWARD_GATEWAY_URL");
    if(forwardGatewayUrl){
        mForwardGatewayUrl = std::string(forwardGatewayUrl);
    }else{
        mForwardGatewayUrl = "https://gateway.rocket.chat";
    }

}

bool Settings::forwardGatewayEnabled() {
    return mForwardGatewayEnabled;
}

const std::string &Settings::forwardGatewayUrl() {
    return mForwardGatewayUrl;
}
