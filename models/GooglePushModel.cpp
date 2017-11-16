//
// Created by armin on 16.11.17.
//

#include <fstream>
#include <iostream>
#include <fstream>

#include <folly/FileUtil.h>
#include <folly/String.h>
#include <folly/io/async/SSLContext.h>
#include <folly/io/async/SSLOptions.h>
#include <folly/portability/GFlags.h>
#include <proxygen/lib/http/HTTPMessage.h>
#include <proxygen/lib/http/session/HTTPUpstreamSession.h>
#include <proxygen/lib/http/codec/HTTP2Codec.h>
#include <curl/curl.h>
#include "GooglePushModel.h"

std::string GooglePushModel::mApiKey;

void GooglePushModel::loadApiKey() {
    std::ifstream ifs("credentials/google/serverKey.txt");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    GooglePushModel::mApiKey = content;
}

GooglePushModel::GooglePushModel(const std::string &pJson) {
    Json::Reader reader;
    Json::Value obj;
    reader.parse(pJson, obj);
    std::cout<<obj["token"].asString()<<std::endl;
    if(obj.isMember("token") && obj.isMember("options")){
        std::string token = obj["token"].asString();
        Json::Value options = obj["options"];
        if(options.isMember("from")&&options.isMember("badge")&&options.isMember("title")&&options.isMember("text")&&options.isMember("badge")){
            std::cout<<"valid json"<<std::endl;
            mTitle = std::move(options["title"].asString());
            mText = std::move(options["text"].asString());
            mFrom = std::move(options["from"].asString());
            mDeviceToken = std::move(obj["token"].asString());
            mBadge = options["badge"].asInt();
        }
    }
}

bool GooglePushModel::sendMessage() {


    Json::Value obj;
    Json::Value msg;
    msg["title"] = mTitle;
    msg["body"] = mText;

    obj["to"] = mDeviceToken;
    obj["notification"] = msg;

    Json::FastWriter fast;

    std::string data = fast.write(obj);

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl){

        struct curl_slist *chunk = nullptr;

        std::cout<<std::string("Authorization: key="+mApiKey)<<std::endl;
        chunk = curl_slist_append(chunk, std::string("Authorization: key="+mApiKey).c_str());
        chunk = curl_slist_append(chunk, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL,mApiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,data.size());
        curl_easy_setopt(curl, CURLOPT_POST, true);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, true);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);


        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            std::cerr<<"curl error: "<<curl_easy_strerror(res)<<std::endl;
        }else{
            return true;
        }
    }

    return false;
}
