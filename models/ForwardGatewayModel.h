//
// Created by armin on 19.04.19.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_FORWARDGATEWAYMODEL_H
#define ROCKETCHATMOBILEPUSHGATEWAY_FORWARDGATEWAYMODEL_H


#include <unordered_set>
#include <string>
#include <mutex>
#include <memory>
#include <proxygen/lib/http/HTTPMessage.h>

using namespace proxygen;

class ForwardGatewayModel {

public:
    static bool ownsRegistrationId(const std::string &pRegistrationId);
    static void claimRegistrationId(const std::string &pRegistrationId);
    static void unClaimRegistrationId(const std::string &pRegistrationId);

    bool forwardMessage(std::unique_ptr<HTTPMessage> pHeaders, const std::string &pBody);

private:
    static std::mutex mRegistrationIdsMutex;
    static std::unordered_set<std::string> mRegistrationIds;

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_FORWARDGATEWAYMODEL_H
