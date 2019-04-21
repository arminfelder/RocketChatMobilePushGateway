//
// Created by armin on 19.04.19.
//

#include <curl/curl.h>
#include <iostream>
#include <chrono>
#include <regex>
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


bool ForwardGatewayModel::forwardMessage(std::unique_ptr<HTTPMessage> pHeaders, const std::string &pBody) const {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *chunk = nullptr;
        try {

            std::string url = pHeaders->getURL();
            curl_easy_setopt(curl, CURLOPT_VERBOSE, false);
            curl_easy_setopt(curl, CURLOPT_URL,pHeaders->getURL().c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pBody.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,pBody.size());
            curl_easy_setopt(curl, CURLOPT_POST, true);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_USE_SSL, true);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, true);

            std::cout << "[" << std::chrono::system_clock::now() << "]\t" << "\tForwardgateway result\t";
            res = curl_easy_perform(curl);
            std::cout << std::endl;

            if (res != CURLE_OK) {
                std::cerr<< "[" << std::chrono::system_clock::now() << "]\t" << "\tForwardgateway conn error: " << curl_easy_strerror(res) << std::endl;
            } else {
                std::cout << "[" << std::chrono::system_clock::now() << "]\t" << "\tForwardgateway conn status: OK " << std::endl;
                curl_easy_cleanup(curl);
                curl_slist_free_all(chunk);
                return true;
            }
        } catch (std::exception &e) {
            std::cout << "[" << std::chrono::system_clock::now() << "] " << e.what() << std::endl;
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
            return false;
        }
    }
}
