//
// Created by armin on 19.04.19.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_FORWARDGATEWAYMODEL_H
#define ROCKETCHATMOBILEPUSHGATEWAY_FORWARDGATEWAYMODEL_H


#include <unordered_set>
#include <string>
#include <mutex>

class ForwardGatewayModel {

public:
    static bool ownsRegistrationId(const std::string &pRegistrationId);
    static void claimRegistrationId(const std::string &pRegistrationId);
    static void unClaimRegistrationId(const std::string &pRegistrationId);

    bool forwardMessage( std::string &&pJson, std::string &&pPath);

    int returnStatusCode() const;
private:
    int mReturnStatusCode;
    static std::mutex mRegistrationIdsMutex;
    static std::unordered_set<std::string> mRegistrationIds;

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_FORWARDGATEWAYMODEL_H
