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
#define UNUSED(x) (void)x;
#include "GooglePushHandler.h"
#include "../models/GooglePushModel.h"
#include "../Settings.h"
#include "../models/ForwardGatewayModel.h"


void GooglePushHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {
    mHeaders = std::move(headers);
}

void GooglePushHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
    if(mBody){
        (*mBody).appendChain(std::move(body));
    } else{
        mBody = std::move(body);
    }
}

void GooglePushHandler::onUpgrade(proxygen::UpgradeProtocol prot) noexcept {
    UNUSED(prot)
}

void GooglePushHandler::onEOM() noexcept {
    if (mBody) {
        try {
            std::string body(reinterpret_cast<const char *>((*mBody).data()));
            GooglePushModel googlePushModel(body);
            std::string response;
            if (googlePushModel.sendMessage()) {
                ResponseBuilder(downstream_).status(200, "OK").body("").sendWithEOM();

            } else if(Settings::forwardGatewayEnabled()){
                ForwardGatewayModel forwardModel;
                if(forwardModel.forwardMessage(std::move(mHeaders), body)){
                    ResponseBuilder(downstream_).status(200, "OK").body("").sendWithEOM();
                }else{
                    ResponseBuilder(downstream_).status(500, "FAILURE").body("failed to send push message through forwardgateway").sendWithEOM();
                }
            }else {
                ResponseBuilder(downstream_).status(500, "FAILURE").body("failed to send push message").sendWithEOM();

            }

        } catch (Exception &e) {
            LOG(ERROR) << "exception " << e.what() << std::endl;
            ResponseBuilder(downstream_).status(500, "FAILURE").body("failed to send push message").sendWithEOM();
        }
    } else {
        ResponseBuilder(downstream_).status(400, "BAD REQUEST").body("failed to send push message").sendWithEOM();
    }
}

void GooglePushHandler::requestComplete() noexcept {

}

void GooglePushHandler::onError(ProxygenError err) noexcept {
    UNUSED(err)
}
