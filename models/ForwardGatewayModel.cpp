//
// Created by armin on 19.04.19.
//

#include "../Settings.h"
#include "ForwardGatewayModel.h"
#include "cpr/api.h"

std::mutex ForwardGatewayModel::mRegistrationIdsMutex;
std::unordered_set<std::string> ForwardGatewayModel::mRegistrationIds;

bool ForwardGatewayModel::ownsRegistrationId(const std::string& pRegistrationId)
{
    std::lock_guard lock(mRegistrationIdsMutex);
    return mRegistrationIds.contains(pRegistrationId);
}

void ForwardGatewayModel::claimRegistrationId(const std::string& pRegistrationId)
{
    std::lock_guard lockGuard(mRegistrationIdsMutex);
    mRegistrationIds.insert(pRegistrationId);
}

void ForwardGatewayModel::unClaimRegistrationId(const std::string& pRegistrationId)
{
    std::lock_guard lockGuard(mRegistrationIdsMutex);
    mRegistrationIds.erase(pRegistrationId);
}

bool ForwardGatewayModel::forwardMessage(std::string&& pJson, std::string&& pPath)
{
    const auto response = cpr::Post(cpr::Url{Settings::forwardGatewayUrl() + pPath},
                                    cpr::Header{{"Content-Type", "application/json"}},
                                    cpr::HttpVersion{cpr::HttpVersionCode::VERSION_2_0_TLS},
                                    cpr::Body{pJson});

    if (response.status_code == 200)
    {
        mReturnStatusCode = 200;
        return true;
    }
    mReturnStatusCode = 500;
    return false;
}

int ForwardGatewayModel::returnStatusCode() const
{
    return mReturnStatusCode;
}