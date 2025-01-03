//
// Created by armin on 21.04.19.
//

#include <cstdlib>
#include <string>
#include <regex>
#include <jsoncpp/json/json.h>
#include <fstream>

#include "drogon/utils/Utilities.h"

#include "Settings.h"



std::string Settings::mForwardGatewayUrl;
bool Settings::mForwardGatewayEnabled = false;
trantor::Logger::LogLevel Settings::mLogLevel = trantor::Logger::LogLevel::kInfo;
Settings::GoogleServiceAccount Settings::mFcmServiceAccount;
std::string Settings::mFcmProjectId;
std::string Settings::mFcmApiEndpoint;
std::string Settings::mApnsPrivateKey;
std::string Settings::mApnsTeamId;
std::string Settings::mApnsKey;
std::string Settings::mApnsAppId;
std::string Settings::mApnsPrivateKeyAlgo = "ES256";
std::string Settings::mConfigDir = "/usr/local/etc/RocketChatMobilePushGateway";

/**
 * @brief Initialize the given system or component.
 *
 * This function performs the initialization of a specified system or component.
 * It sets up necessary configurations, allocates resources, and ensures that
 * the system is ready for operation. The specifics of the initialization may
 * depend on the provided arguments or context.
 *
 * @param config A configuration object or parameters that define the initialization settings.
 * @return Returns true if the initialization was successful, otherwise false.
 */
void Settings::init() {
    const std::regex newLine("([\\n]+)");

    if(const auto configDir = std::getenv("CONFIG_DIR")){
        mConfigDir = std::string(configDir);
    }
    LOG_INFO << "Settings: Using config dir: " << mConfigDir;

    loadApnKeyFile();
    loadApnSettingsFile();
    loadFcmServiceAccountJson();

    if (const auto logEnv = std::getenv("LOG_LEVEL"))
    {
        std::string loglevel = logEnv;
        std::ranges::transform(loglevel, loglevel.begin(),
                               [](const unsigned char c){ return std::toupper(c); }
        );

        if (loglevel == "INFO")
        {
            mLogLevel = trantor::Logger::LogLevel::kInfo;
        }else if (loglevel == "DEBUG")
        {
            mLogLevel = trantor::Logger::LogLevel::kDebug;
        }else if (loglevel == "WARN")
        {
            mLogLevel = trantor::Logger::LogLevel::kWarn;
        }else if (loglevel == "ERROR")
        {
            mLogLevel = trantor::Logger::LogLevel::kError;
        }else if (loglevel == "FATAL")
        {
            mLogLevel = trantor::Logger::LogLevel::kFatal;
        }else if (loglevel == "TRACE")
        {
            mLogLevel = trantor::Logger::LogLevel::kTrace;
        }

        LOG_INFO << "Settings: Debug Logging Enabled: " << loglevel;
    }

    if(const auto enableForwardGateway = std::getenv("FORWARD_GATEWAY_ENABLE"); enableForwardGateway && std::string(enableForwardGateway) == "TRUE"){
        mForwardGatewayEnabled = true;
    }
    if(const auto forwardGatewayUrl = std::getenv("FORWARD_GATEWAY_URL")){
        mForwardGatewayUrl = std::regex_replace(std::string(forwardGatewayUrl),newLine,"");
        LOG_INFO << "Settings: Custom Forward Gateway enabled";
    }else{
        mForwardGatewayUrl = "https://gateway.rocket.chat";
        LOG_INFO << "Settings: Using default Forward Gateway";
    }

    if(const auto fcmServiceAccountJson = std::getenv("FCM_SERVICE_ACCOUNT_JSON")){
        setGoogleCredentialsFromServiceAccountJson(std::string(fcmServiceAccountJson));
        LOG_INFO << "Settings: Using FCM ServiceAccount from env";
    }else if (mFcmServiceAccount.project_id.empty()
        || mFcmServiceAccount.auth_provider_x509_cert_url.empty()
        || mFcmServiceAccount.auth_uri.empty()
        || mFcmServiceAccount.client_email.empty()
        || mFcmServiceAccount.client_id.empty()
        || mFcmServiceAccount.private_key.empty()
        || mFcmServiceAccount.private_key_id.empty()
        || mFcmServiceAccount.type.empty()
        || mFcmServiceAccount.client_x509_cert_url.empty()
        || mFcmServiceAccount.universe_domain.empty()
    )
    {
        LOG_ERROR << "Settings: FCM ServiceAccount not found, or properties are missing";
        exit(EXIT_FAILURE);
    }

    if(const auto apnsPrivateKey = std::getenv("APNS_PRIVATE_KEY")){
        mApnsPrivateKey = std::string(drogon::utils::base64Decode(apnsPrivateKey));
        LOG_INFO << "Settings: Using APNs Private Key from env";
    }else if (mApnsPrivateKey.empty())
    {
        LOG_ERROR << "Settings: APNs Private Key not found";
        exit(EXIT_FAILURE);
    }

    if(const auto apnsTeamId = std::getenv("APNS_TEAM_ID")){
        mApnsTeamId = std::string(apnsTeamId);
        LOG_INFO << "Settings: Using APNs TeamID from env";
    }else if (mApnsTeamId.empty())
    {
        LOG_ERROR << "Settings: APNs TeamID not found";
        exit(EXIT_FAILURE);
    }

    if(const auto apnsKey = std::getenv("APNS_KEY")){
        mApnsKey = std::string(apnsKey);
        LOG_INFO << "Settings: Using APNs Key from env";
    }else if (mApnsKey.empty())
    {
        LOG_ERROR << "Settings: APNs Key not found";
        exit(EXIT_FAILURE);
    }

    if(const auto apnsAppId = std::getenv("APNS_APPID")){
        mApnsAppId = std::string(apnsAppId);
        LOG_INFO << "Settings: Using APNs AppID from env";
    }else if (mApnsAppId.empty())
    {
        LOG_ERROR << "Settings: APNs AppID not found";
        exit(EXIT_FAILURE);
    }

    if(const auto apnsPrivateKeyAlgo = std::getenv("APNS_PRIVATE_KEY_ALGO")){
        mApnsPrivateKeyAlgo = std::string(apnsPrivateKeyAlgo);
        LOG_INFO << "Settings: Using APNs Private Key Algo from env";
    }else if (mApnsPrivateKeyAlgo.empty())
    {
        LOG_ERROR << "Settings: APNs Private Key Algo not found";
        exit(EXIT_FAILURE);
    }

    mFcmApiEndpoint = "https://fcm.googleapis.com/v1/projects/" + mFcmServiceAccount.project_id + "/messages:send";

}

/**
 * @brief Configures the Google credentials using a service account JSON string.
 *
 * This function parses the given service account JSON string, extracts all the
 * required fields, and sets up the internal configuration for Firebase Cloud Messaging (FCM)
 * service account. If any mandatory fields are missing or the JSON is improperly formatted,
 * the operation fails and logs an error.
 *
 * @param pServiceAccountJson A string containing the service account JSON.
 * @return Returns true if the JSON contains all required fields and the credentials
 *         are successfully configured; otherwise, false.
 */
bool Settings::setGoogleCredentialsFromServiceAccountJson(const std::string& pServiceAccountJson)
{
    Json::Reader reader;
    Json::Value obj;
    reader.parse(pServiceAccountJson, obj);
    if (obj.isMember("type") && obj.isMember("project_id") && obj.isMember("private_key_id") && obj.isMember("private_key") && obj.isMember("client_email") && obj.isMember("client_id") && obj.isMember("auth_uri") && obj.isMember("token_uri") && obj.isMember("auth_provider_x509_cert_url") && obj.isMember("client_x509_cert_url") && obj.isMember("universe_domain")) {
        std::string type = obj["type"].asString();
        std::string projectId = obj["project_id"].asString();
        std::string privateKeyId = obj["private_key_id"].asString();
        std::string privateKey = obj["private_key"].asString();
        std::string clientEmail = obj["client_email"].asString();
        std::string clientId = obj["client_id"].asString();
        std::string authUri = obj["auth_uri"].asString();
        std::string tokenUri = obj["token_uri"].asString();
        std::string authProviderX509CertUrl = obj["auth_provider_x509_cert_url"].asString();
        std::string clientX509CertUrl = obj["client_x509_cert_url"].asString();
        std::string universeDomain = obj["universe_domain"].asString();
        if (!type.empty()
            && !projectId.empty()
            && !privateKeyId.empty()
            && !privateKey.empty()
            && !clientEmail.empty()
            && !clientId.empty()
            && !authUri.empty()
            && !tokenUri.empty()
            && !authProviderX509CertUrl.empty()
            && !clientX509CertUrl.empty()
            ) {
            mFcmServiceAccount.project_id = std::move(projectId);
            mFcmServiceAccount.type = std::move(type);
            mFcmServiceAccount.private_key_id = std::move(privateKeyId);
            mFcmServiceAccount.private_key = std::move(privateKey);
            mFcmServiceAccount.client_email = std::move(clientEmail);
            mFcmServiceAccount.client_id = std::move(clientId);
            mFcmServiceAccount.auth_uri = std::move(authUri);
            mFcmServiceAccount.token_uri = std::move(tokenUri);
            mFcmServiceAccount.auth_provider_x509_cert_url = std::move(authProviderX509CertUrl);
            mFcmServiceAccount.client_x509_cert_url = std::move(clientX509CertUrl);
            mFcmServiceAccount.universe_domain = std::move(universeDomain);
            return true;
        }
        LOG_ERROR << "failed to parse ServiceAccount JSON, keys missing";
    }

    return false;
}

/**
 * @brief Check if the forwarding gateway feature is enabled.
 *
 * This function determines whether the forwarding gateway functionality
 * is active based on the current configuration or system state. It is
 * used to verify the availability of this feature before performing
 * related operations.
 *
 * @return Returns true if the forwarding gateway feature is enabled, otherwise false.
 */
bool Settings::forwardGatewayEnabled() {
    return mForwardGatewayEnabled;
}

trantor::Logger::LogLevel Settings::getLogLevel()
{
    return mLogLevel;
}

const std::string &Settings::forwardGatewayUrl() {
    return mForwardGatewayUrl;
}

/**
 * @brief Retrieve the Firebase Cloud Messaging (FCM) service account details.
 *
 * This function provides access to the service account information used for Firebase
 * Cloud Messaging authentication. The returned account details are typically required
 * to authenticate and send notifications through the FCM service.
 *
 * @return Returns the FCM service account details as an object or configuration,
 * or a null value if the service account is not configured.
 */
const Settings::GoogleServiceAccount& Settings::fcmServiceAccount()
{
    return mFcmServiceAccount;
}

/**
 * @brief Retrieves the Firebase Cloud Messaging (FCM) project ID.
 *
 * This function provides access to the stored FCM project ID associated with
 * the settings instance. The project ID is used for identification purposes
 * in FCM-related operations.
 *
 * @return A constant reference to the string containing the FCM project ID.
 */
const std::string& Settings::fcmProjectId() {
    return mFcmProjectId;
}

/**
 * @brief Retrieve the FCM API endpoint.
 *
 * This function provides the configured URL or endpoint for the Firebase Cloud
 * Messaging (FCM) API. The endpoint is typically used for sending messages or
 * interacting with FCM services.
 *
 * @return Returns a string containing the FCM API endpoint URL.
 */
const std::string& Settings::fcmApiEndpoint()
{
    return mFcmApiEndpoint;
}

/**
 * @brief Retrieve the APNs private key used for authentication.
 *
 * This function provides access to the private key required for establishing
 * secure communication with the Apple Push Notification Service (APNs). The key
 * is typically used for signing authentication tokens during the connection process.
 *
 * @return Returns the APNs private key as a string or object, depending on implementation.
 */
const std::string &Settings::apnsPrivateKey() {
    return mApnsPrivateKey;
}

/**
 * @brief Retrieve the APNs Team ID.
 *
 * This function returns the Apple Push Notification service (APNs) Team ID
 * associated with the application. The Team ID is used to authenticate and
 * authorize notifications sent through APNs.
 *
 * @return The APNs Team ID as a string.
 */
const std::string &Settings::apnsTeamId() {
    return mApnsTeamId;
}

/**
 * @brief Retrieve the APNs application identifier.
 *
 * This function provides the Apple Push Notification Service (APNs) application
 * identifier associated with the system or component. The application ID generally
 * identifies the app for push notifications in Apple's ecosystem.
 *
 * @return Returns the APNs application identifier as a string.
 */
const std::string &Settings::apnsAppId() {
    return mApnsAppId;
}

/**
 * @brief Represents the APNs (Apple Push Notification Service) key.
 *
 * This variable is used to store the key required for authenticating and sending
 * push notifications using the Apple Push Notification Service. The key is typically
 * generated in the Apple Developer portal and is associated with the application's
 * push notification configuration.
 */
const std::string &Settings::apnsKey() {
    return mApnsKey;
}

/**
 * @brief Retrieve the algorithm used for the APNs private key.
 *
 * This function returns the cryptographic algorithm associated with the APNs
 * private key. The algorithm specifies the method that will be used for
 * signing or decrypting data when interacting with the APNs service.
 *
 * @return A string representing the algorithm name for the APNs private key.
 */
const std::string &Settings::apnsPrivateKeyAlgo() {
    return mApnsPrivateKeyAlgo;
}

void Settings::loadApnKeyFile() {
    std::ifstream ifsPem(mConfigDir+"/credentials/apple/key.pem");
    const std::string content((std::istreambuf_iterator<char>(ifsPem)),
                        (std::istreambuf_iterator<char>()));
    if (!content.empty()) {
        LOG_INFO << "Settings: Using APN key file";
        mApnsPrivateKey = content;
    } else {
        LOG_INFO << "Settings: No APN key file found at "+mConfigDir+"/credentials/apple/key.pem";
    }
}

/**
 * @brief Loads APN (Apple Push Notification) settings from a configuration file.
 *
 * This function reads the APN settings from a JSON file located at "/certs/apple/settings.json".
 * It parses the JSON content to extract necessary data such as `appId`, `teamId`, and `key`.
 * If the file is found and contains valid settings, the extracted values are stored in
 * the respective member variables. In case the values are incomplete or invalid, the function
 * logs an error and terminates the program. If the file is not found, it logs an informational
 * message indicating the absence of the settings file.
 *
 * Function behavior:
 * - When valid APN settings are available:
 *     - Stored locally for use during the application's runtime.
 * - When settings are missing or invalid:
 *     - Logs error details.
 *     - Stops execution to prevent misconfiguration.
 * - When the file is absent:
 *     - Logs a message to notify that no settings file was found.
 */
void Settings::loadApnSettingsFile() {
    std::ifstream ifsSettings(mConfigDir+"/credentials/apple/settings.json");
    const std::string content((std::istreambuf_iterator<char>(ifsSettings)),
                        (std::istreambuf_iterator<char>()));
    if(!content.empty()) {
        LOG_INFO << "Settings: Using APN settings file";
        Json::Reader reader;
        Json::Value obj;
        reader.parse(content, obj);
        if (obj.isMember("appId") && obj.isMember("teamId") && obj.isMember("key")) {
            std::string appId = obj["appId"].asString();
            std::string teamId = obj["teamId"].asString();
            std::string key = obj["key"].asString();
            if (!appId.empty() && !teamId.empty() && !key.empty()) {
                mApnsAppId = std::move(appId);
                mApnsTeamId = std::move(teamId);
                mApnsKey = std::move(key);
            } else {
                LOG_ERROR << "Error JSON data invalid";
                exit(EXIT_FAILURE);
            }
        }
    } else {
        LOG_INFO << "Settings: No APN settings file found at "<<mConfigDir<<"/credentials/apple/settings.json";
    }
}

/**
 * @brief Load and process the Firebase Cloud Messaging (FCM) Service Account JSON file.
 *
 * This function attempts to load the FCM Service Account JSON file from the specified
 * path. If the file is found and contains valid content, it parses the JSON and sets
 * the Google service account credentials. If the file is missing, empty, or contains
 * invalid data, appropriate log messages are generated to indicate the issue.
 *
 * The method checks for the existence and readability of the file at
 * "/certs/google/serviceAccount.json" and extracts its contents. Successful parsing
 * of valid JSON content results in configuring the necessary credentials for FCM.
 * Errors during this process are logged for diagnostic purposes.
 */
void Settings::loadFcmServiceAccountJson() {
    std::ifstream ifs(mConfigDir+"/credentials/google/serviceAccount.json");
    const std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    if (!content.empty()) {
        LOG_INFO << "Settings: Using FCM ServiceAccount JSON";
        if (!setGoogleCredentialsFromServiceAccountJson(content))
        {
            LOG_ERROR << "failed to parse ServiceAccount JSON";
        }

    } else {
        LOG_INFO << "Settings: No FCM ServiceAccount JSON file found at "<<mConfigDir<<"/credentials/google/serviceAccount.json";
    }
}
