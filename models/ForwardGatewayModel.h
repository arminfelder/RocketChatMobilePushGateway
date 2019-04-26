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

    static size_t curlWriteCallback(void *buffer, size_t size, size_t nmemb,
                                    void *this_ptr);
    int returnStatusCode() const;
private:
    int mReturnStatusCode;
    static std::mutex mRegistrationIdsMutex;
    static std::unordered_set<std::string> mRegistrationIds;

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_FORWARDGATEWAYMODEL_H
