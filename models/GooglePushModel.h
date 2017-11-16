//
// Created by armin on 16.11.17.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHMODEL_H
#define ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHMODEL_H

#include <string>
#include <jsoncpp/json/json.h>


class GooglePushModel {


public:
    GooglePushModel(const std::string &pJson);

    bool sendMessage();

    static void loadApiKey();

private:
    static std::string mApiKey;

    const std::string mApiUrl{"https://fcm.googleapis.com/fcm/send"};

    std::string mTitle;
    std::string mText;
    std::string mDeviceToken;
    std::string mFrom;
    int mBadge{0};

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHMODEL_H
