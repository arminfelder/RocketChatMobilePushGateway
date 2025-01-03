//
// Created by armin on 12/31/24.
//

#ifndef GATEWAYNOTIFICATION_H
#define GATEWAYNOTIFICATION_H

#include <optional>
#include <json/json.h>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>

class GatewayNotification
{
    typedef struct
    {
        std::optional<std::string> from;
        std::optional<std::string> title;
        std::optional<std::string> text;
        std::optional<int> badge;
        std::optional<std::string> sound;
        std::optional<int> notId;
        std::optional<std::string> category;
    } APN;

    typedef struct
    {
        std::optional<std::string> from;
        std::optional<std::string> title;
        std::optional<std::string> text;
        std::optional<std::string> image;
        std::optional<std::string> style;
        std::optional<std::string> summaryText;
        std::optional<std::string> picture;
        std::optional<int> badge;
        std::optional<std::string> sound;
        std::optional<int> notId;
        std::optional<std::vector<std::map<std::string, std::string>>> actions;
    } GCM;

    typedef struct
    {
        std::optional<std::string> query;
    } Query;

public:
    explicit GatewayNotification();
    static const std::pair<std::optional<GatewayNotification>, std::optional<std::vector<std::string>>> fromJson(
        const std::shared_ptr<Json::Value>& pJson);



private:
    std::string mUniqueId;
    std::string mFrom;
    std::string mTitle;
    std::string mText;
    std::optional<int> mBadge;
    std::optional<std::string> mSound;
    std::optional<int> mNotId;
    std::optional<bool> mContentAvailable;
    std::optional<int> mForceStart;
    std::optional<std::string> mTopic;
    std::optional<APN> mApn; // Pointer to nested APN object. Can be nullptr.
    std::optional<GCM> mGcm; // Pointer to nested GCM object. Can be nullptr.
    std::optional<Query> mQuery; // Assuming the `query.userId` is a key-value map
    std::optional<std::string> mToken;
    std::optional<std::vector<std::string>> mTokens;
    std::optional<std::string> mEjson;
    std::optional<std::string> mDelayUntil; // Replacing Date with chrono
    std::string mCreatedAt; // Mandatory
    std::optional<std::string> mCreatedBy;

public:
    [[nodiscard]] const std::string& getUniqueId() const { return mUniqueId; }
    [[nodiscard]] const std::string& getFrom() const { return mFrom; }
    [[nodiscard]] const std::string& getTitle() const { return mTitle; }
    [[nodiscard]] const std::string& getText() const { return mText; }
    [[nodiscard]] const std::optional<int>& getBadge() const { return mBadge; }
    [[nodiscard]] const std::optional<std::string>& getSound() const { return mSound; }
    [[nodiscard]] const std::optional<int>& getNotId() const { return mNotId; }
    [[nodiscard]] const std::optional<bool>& getContentAvailable() const { return mContentAvailable; }
    [[nodiscard]] const std::optional<int>& getForceStart() const { return mForceStart; }
    [[nodiscard]] const std::optional<std::string>& getTopic() const { return mTopic; }
    [[nodiscard]] const std::optional<APN>& getApn() const { return mApn; }
    [[nodiscard]] const std::optional<GCM>& getGcm() const { return mGcm; }
    [[nodiscard]] const std::optional<Query>& getQuery() const { return mQuery; }
    [[nodiscard]] const std::optional<std::string>& getToken() const { return mToken; }
    [[nodiscard]] const std::optional<std::vector<std::string>>& getTokens() const { return mTokens; }
    [[nodiscard]] const std::optional<std::string>& getEJson() const { return mEjson; }
    [[nodiscard]] const std::optional<std::string>& getDelayUntil() const { return mDelayUntil; }
    [[nodiscard]] const std::string& getCreatedAt() const { return mCreatedAt; }
    [[nodiscard]] const std::optional<std::string>& getCreatedBy() const { return mCreatedBy; }
};


#endif //GATEWAYNOTIFICATION_H
