/********************************************************************************************************************
 *                                                                                                                  *
 * Copyright (C) 2017 Armin Felder                                                                                  *
 * This file is part of RocketChatMobilePushGateway <https://git.fairkom.net/chat/RocketChatMobilePushGateway>.     *
 *                                                                                                                  *
 * RocketChatMobilePushGateway is free software: you can redistribute it and/or modify                              *
 * it under the terms of the GNU General Public License as published by                                             *
 * the Free Software Foundation, either version 3 of the License, or                                                *
 * (at your option) any later version.                                                                              *
 *                                                                                                                  *
 * RocketChatMobilePushGateway is distributed in the hope that it will be useful,                                   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                                                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                                    *
 * GNU General Public License for more details.                                                                     *
 *                                                                                                                  *
 * You should have received a copy of the GNU General Public License                                                *
 * along with RocketChatMobilePushGateway. If not, see <http://www.gnu.org/licenses/>.                              *
 *                                                                                                                  *
 ********************************************************************************************************************/
#include "../models/GooglePushModel.h"
#include "../models/ForwardGatewayModel.h"
#include "../Settings.h"
#include "GooglePushHandler.h"

#include "glog/logging.h"

void GooglePushHandler::pushMessage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback)
{
    const auto resp = HttpResponse::newHttpResponse();
    if (const auto body = req->getJsonObject(); !body)
     {
          LOG(ERROR) << "invalid json";
          resp->setStatusCode(k400BadRequest);
          resp->setBody(
              "invalid payload, must be JSON");
     }else{

         GooglePushModel googlePushModel(body);
         if (googlePushModel.sendMessage()) {
           resp->setStatusCode(k200OK);
         }else if(Settings::forwardGatewayEnabled()){
           ForwardGatewayModel forwardModel{};
           if (auto bodyPtr = std::string(req->getBody()); forwardModel.forwardMessage(
               std::move(bodyPtr), req->getPath().data()))
           {
             resp->setStatusCode(k200OK);
           }else{
             resp->setStatusCode(k500InternalServerError);
             resp->setBody("failed to send push message through forwardgateway");
           }
         }
     }

    callback(resp);
}
