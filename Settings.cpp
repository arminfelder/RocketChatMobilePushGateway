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
    std::string enableForwardGateway(std::getenv("FORWARD_GATEWAY_ENABLE"));
    if(enableForwardGateway == "TRUE"){
        mForwardGatewayEnabled = true;
    }
    std::string forwardGatewayUrl(std::getenv("FORWARD_GATEWAY_URL"));
    if(!forwardGatewayUrl.empty()){
        mForwardGatewayUrl = forwardGatewayUrl;
    }else{
        mForwardGatewayUrl = "https://gateway.rocket.chat";
    }

}

bool Settings::forwardGatewayEnabaled() {
    return mForwardGatewayEnabled;
}

const std::string &Settings::forwardGatewayUrl() {
    return mForwardGatewayUrl;
}
