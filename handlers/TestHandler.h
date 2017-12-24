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

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_TESTHANDLER_H
#define ROCKETCHATMOBILEPUSHGATEWAY_TESTHANDLER_H

#include <folly/Memory.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>


using namespace proxygen;

class TestHandler : public RequestHandler{
public:
    TestHandler();

    void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override ;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept  override ;

    void onEOM() noexcept override ;

    void onUpgrade(proxygen::UpgradeProtocol proto) noexcept override ;

    void requestComplete() noexcept override ;

    void onError(proxygen::ProxygenError err) noexcept override ;

private:
    std::unique_ptr<folly::IOBuf> mBody;

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_TESTHANDLER_H
