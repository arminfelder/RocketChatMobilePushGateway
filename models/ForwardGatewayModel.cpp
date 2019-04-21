//
// Created by armin on 19.04.19.
//

#include <curl/curl.h>
#include <iostream>
#include <cstdlib>

#include "../date.h"
#include "ForwardGatewayModel.h"
#include "../Settings.h"

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

size_t ForwardGatewayModel::curlWriteCallback(void *buffer, size_t size, size_t nmemb, void *this_ptr) {

    if (buffer != nullptr) {
        std::string bufferString(static_cast<char *>(buffer), nmemb);
        LOG(ERROR)<<"forwardgateway error response: "<<bufferString;
        return 0;
    }
    return 1;
}

bool ForwardGatewayModel::forwardMessage(std::unique_ptr<HTTPMessage> pHeaders, const std::string &pBody) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *chunk = nullptr;
        try {

            std::string url = pHeaders->getURL();
            std::string path = pHeaders->getPath();
            auto headers = pHeaders->getHeaders();
            auto host = headers.rawGet("Host");

            url = Settings::forwardGatewayUrl()+path;
            curl_easy_setopt(curl, CURLOPT_VERBOSE, false);
            curl_easy_setopt(curl, CURLOPT_URL,url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pBody.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,pBody.size());
            curl_easy_setopt(curl, CURLOPT_POST, true);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_USE_SSL, true);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, true);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);


            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                if(res == CURLE_WRITE_ERROR){

                }else{
                    LOG(ERROR)<< "\tForwardgateway error: " << curl_easy_strerror(res);
                }
            } else {
                LOG(INFO) << "\tForwardgateway conn status: OK ";
                curl_easy_cleanup(curl);
                curl_slist_free_all(chunk);
                return true;
            }
        } catch (std::exception &e) {
            LOG(ERROR) << e.what();
            curl_easy_cleanup(curl);
            curl_slist_free_all(chunk);
            return false;
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);
    }
    return false;
}
