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

#include "TestHandler.h"

TestHandler::TestHandler() {

}

void TestHandler::onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept {

}

void TestHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
    if(mBody){
        mBody->prependChain(std::move(body));
    } else{
        mBody = std::move(body);
    }
}

void TestHandler::onEOM() noexcept {
    ResponseBuilder(downstream_).status(200, "OK")
            .header("Set-Cookie", "test=123;Max-Age=60").body("Hello World").sendWithEOM();
}

void TestHandler::onUpgrade(proxygen::UpgradeProtocol proto) noexcept {

}

void TestHandler::requestComplete() noexcept {
    delete this;
}

void TestHandler::onError(proxygen::ProxygenError err) noexcept {

}