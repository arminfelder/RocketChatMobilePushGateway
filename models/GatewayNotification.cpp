//
// Created by armin on 12/31/24.
//

#include "GatewayNotification.h"
#include <unordered_map>
#include <functional>

#include "trantor/utils/Logger.h"

GatewayNotification::GatewayNotification(){

}

const std::pair<std::optional<GatewayNotification>, std::optional<std::vector<std::string>>> GatewayNotification::fromJson(const std::shared_ptr<Json::Value>& pJson) {
  std::vector<std::string> errors;
  GatewayNotification notification;

  if(pJson->isMember("token")){
    if(!(*pJson)["token"].isString()){
      std::string error = "field \"token\" is not a string";
      LOG_ERROR << error;
      errors.push_back(error);
    }else{
      notification.mToken = (*pJson)["token"].asString();
    }
  }

  const auto &options = (*pJson)["options"];

  if(!(options.isMember("uniqueId") && options["uniqueId"].isString())){
    std::string error = "field \"uniqueId\" is not a string";
    LOG_ERROR <<error;
    errors.push_back(error);
  }else{
    notification.mUniqueId = options["uniqueId"].asString();
  }

  if(!(options.isMember("from") && options["from"].isString())){
    std::string error = "field \"from\" is not a string";
    LOG_ERROR <<error;
    errors.push_back(error);
  }
  else{
    notification.mFrom = options["from"].asString();
  }

  if(!(options.isMember("title") && options["title"].isString())){
    std::string error = "field \"title\" is not a string";
    LOG_ERROR <<error;
    errors.push_back(error);
  }else{
    notification.mTitle = options["title"].asString();
  }

  if(!(options.isMember("text") && options["text"].isString())){
    std::string error = "field \"text\" is not a string";
    LOG_ERROR <<error;
    errors.push_back(error);
  }else{
    notification.mText = options["text"].asString();
  }

  if(!(options.isMember("createdAt") && options["createdAt"].isString())){
    std::string error = "field \"createdAt\" is not a string";
    LOG_ERROR <<error;
    errors.push_back(error);
  }else{
    notification.mCreatedAt = options["createdAt"].asString();
  }

  if(options.isMember("badge")){
    if(!options["badge"].isInt()){
      std::string error = "field \"badge\" is not an integer";
      LOG_ERROR <<error;
      errors.push_back(error);
    }else {
      notification.mBadge = options["badge"].asInt();
    }
  }

  if(options.isMember("sound")){
    if(!options["sound"].isString()){
      std::string error = "field \"sound\" is not a string";
      LOG_ERROR <<error;
      errors.push_back(error);
    }else {
      notification.mSound = options["sound"].asString();
    }
  }

  if(options.isMember("notId")){
    if (!options["notId"].isInt()){
      std::string error = "field \"notId\" is not an integer";
      LOG_ERROR <<error;
      errors.push_back(error);
    }else{
      notification.mNotId = options["notId"].asInt();
    }
  }

  if(options.isMember("contentAvailable")){
    if(!options["contentAvailable"].isBool()){
      std::string error = "field \"contentAvailable\" is not a boolean";
      LOG_ERROR <<error;
      errors.push_back(error);
    }else{
      notification.mContentAvailable = options["contentAvailable"].asBool();
    }
  }
  if(options.isMember("forceStart")){
    if(!options["forceStart"].isInt()){
      std::string error = "field \"forceStart\" is not an integer";
      LOG_ERROR <<error;
      errors.push_back(error);
    }else{
      notification.mForceStart = options["forceStart"].asInt();
    }
  }
  if(options.isMember("topic")){
    if (!options["topic"].isString()){
      std::string error = "field \"topic\" is not a string";
      LOG_ERROR <<error;
      errors.push_back(error);
    }else{
      notification.mTopic = options["topic"].asString();
    }
  }

  if(options.isMember("apn")){
    if(!options["apn"].isObject()){
      std::string error = "field \"apn\" is not an object";
      LOG_ERROR << error;
      errors.push_back(error);
    }else{
      const auto apnJson = options["apn"];
      APN apn;
      if(apnJson.isMember("from")){
        if (!apnJson["from"].isString()){
          std::string error = "field \"apn.from\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        }else {
         apn.from = notification.mFrom; 
        }
      }
      if(apnJson.isMember("title")){
        if (!apnJson["title"].isString()){
          std::string error = "field \"apn.title\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        }else{
          apn.title = apnJson["title"].asString();
        }
      }
      if(apnJson.isMember("text")){
        if(!apnJson["text"].isString()){
          std::string error = "field \"apn.text\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        }else{
          apn.text = apnJson["text"].asString();
        }
      }
      if(apnJson.isMember("badge")){
        if(!apnJson["badge"].isInt()){
          std::string error = "field \"apn.badge\" is not an integer";
          LOG_ERROR << error;
          errors.push_back(error);
        }else{
          apn.badge = apnJson["badge"].asInt();
        }
      }
      if(apnJson.isMember("sound")){
        if(!apnJson["sound"].isString()){
          std::string error = "field \"apn.sound\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        }else{
          apn.sound = apnJson["sound"].asString();
        }
      }
      if(apnJson.isMember("notId")){
        if(!apnJson["notId"].isInt()){
          std::string error = "field \"apn.notId\" is not an integer";
          LOG_ERROR << error;
          errors.push_back(error);
        }else{
          apn.notId = apnJson["notId"].asInt();
        }
      }
      if(options.operator[]("apn").isMember("category")){
        if(!apnJson["category"].isString()){
          std::string error = "field \"apn.category\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        }else{
          apn.category = apnJson["category"].asString();
        }
      }
      notification.mApn = std::move(apn);
    }
  }
  if(options.isMember("gcm")){
    if(!options.operator[]("gcm").isObject()){
      std::string error = "field \"gcm\" is not an object";
      LOG_ERROR << error;
      errors.push_back(error);
    }else{
      GCM gcm;
      if(options.operator[]("gcm").isMember("from")){
        if (!options.operator[]("gcm")["from"].isString()){
          std::string error = "field \"gcm.from\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        }
        else{
          gcm.from = options.operator[]("gcm")["from"].asString();
        }
      }
      if(options["gcm"].isMember("title")){
        if(!options["gcm"]["title"].isString()){
          std::string error = "field \"gcm.title\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.title = options["gcm"]["title"].asString();
        }
      }

      if(options["gcm"].isMember("text")){
        if(!options["gcm"]["text"].isString()){
          std::string error = "field \"gcm.text\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.text = options["gcm"]["text"].asString();
        }
      }
      if(options["gcm"].isMember("image")){
        if(!options["gcm"]["image"].isString()){
          std::string error = "field \"gcm.image\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.image = options["gcm"]["image"].asString();
        }
      }

      if(options["gcm"].isMember("style")){
        if(!options["gcm"]["style"].isString()){
          std::string error = "field \"gcm.style\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.style = options["gcm"]["style"].asString();
        }
      }
      if(options["gcm"].isMember("summaryText")){
        if(!options["gcm"]["summaryText"].isString()){
          std::string error = "field \"gcm.summaryText\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.summaryText = options["gcm"]["summaryText"].asString();
        }
      }
      if(options["gcm"].isMember("picture")){
        if(!options["gcm"]["picture"].isString()){
          std::string error = "field \"gcm.picture\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.picture = options["gcm"]["picture"].asString();
        }
      }
      if(options["gcm"].isMember("badge")){
        if(!options["gcm"]["badge"].isInt()){
          std::string error = "field \"gcm.badge\" is not an integer";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.badge = options["gcm"]["badge"].asInt();
        }
      }
      if(options["gcm"].isMember("sound")){
        if(!options["gcm"]["sound"].isString()){
          std::string error = "field \"gcm.sound\" is not a string";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.sound = options["gcm"]["sound"].asString();
        }
      }
      if(options["gcm"].isMember("notId")){
        if(!options["gcm"]["notId"].isInt()){
          std::string error = "field \"gcm.notId\" is not an integer";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
          gcm.notId = options["gcm"]["notId"].asInt();
        }
      }
      //not implemented yet
      if(options["gcm"].isMember("actions")){
        if(!options["gcm"]["actions"].isArray()){
          std::string error = "field \"gcm.actions\" is not an array";
          LOG_ERROR << error;
          errors.push_back(error);
        } else {
        }
      }
      notification.mGcm = std::move(gcm);
    }
}


  if(options.isMember("query")){
    if(!options["query"].isObject()){
      std::string error = "field \"query\" is not an object";
      LOG_ERROR << error;
      errors.push_back(error);
    }else{
      if(!options["query"].isMember("query") || !options["query"]["query"].isString()){
        std::string error = "field \"query.query\" is not a string";
      }else{
        Query query;
        query.query = options["query"]["query"].asString();
      }
    }
  }

  if(options.isMember("tokens")){
    if(!options["tokens"].isArray()){
      std::string error = "field \"tokens\" is not an array";
      LOG_ERROR << error;
      errors.push_back(error);
    }else{
      if(!options["tokens"].isArray()){
        std::string error = "field \"tokens\" is not an array";
        LOG_ERROR << error;
        errors.push_back(error);
      }else{
        std::vector<std::string> tokens;
        for(const auto& token : options["tokens"]){
          tokens.push_back(token.asString());
        }
        notification.mTokens = std::move(tokens);
      }
    }
  }
  if(options.isMember("payload")){
    Json::FastWriter fast;
    notification.mEjson = fast.write(options["payload"]["payload"]);
  }
  if(options.isMember("delayUntil")){
    if( !options["delayUntil"].isString()){
      std::string error = "field \"delayUntil\" is not a string";
      LOG_ERROR << error;
      errors.push_back(error);
    }else{
      notification.mDelayUntil = options["delayUntil"].asString();
    }
  }
  if(options.isMember("createdBy")){
    if(!options["createdBy"].isString()){
      std::string error = "field \"createdBy\" is not a string";
      LOG_ERROR << error;
      errors.push_back(error);
    }else{
      notification.mCreatedBy = options["createdBy"].asString();
    }
  }
  if (errors.empty()){
    return std::make_pair(std::move(notification),std::nullopt);
  }else{
    return std::make_pair(std::nullopt,std::move(errors));
  }
}
