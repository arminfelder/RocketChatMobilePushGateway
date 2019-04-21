//
// Created by armin on 19.04.19.
//

#include "ForwardGatewayModel.h"

std::mutex ForwardGatewayModel::mRegistrationIdsMutex;
std::unordered_set<std::string> ForwardGatewayModel::mRegistrationIds;

bool ForwardGatewayModel::ownsRegistrationId(const std::string &pRegistrationId) {
    std::lock_guard<std::mutex> lock(mRegistrationIdsMutex);
    return mRegistrationIds.find(pRegistrationId) != mRegistrationIds.end();
}

void ForwardGatewayModel::claimRegistrationId(const std::string &pRegistrationId) {
    std::lock_guard<std::mutex> lockGuard(mRegistrationIdsMutex);
    mRegistrationIds.insert(pRegistrationId);
}

void ForwardGatewayModel::unClaimRegistrationId(const std::string &pRegistrationId) {
    std::lock_guard<std::mutex> lockGuard(mRegistrationIdsMutex);
    mRegistrationIds.erase(pRegistrationId);
}


void ForwardGatewayModel::forwardMessage() const {

}
