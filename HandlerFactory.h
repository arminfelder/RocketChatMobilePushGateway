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

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_HANDLERFACTORY_H
#define ROCKETCHATMOBILEPUSHGATEWAY_HANDLERFACTORY_H

#include <folly/Memory.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>

#include "handlers/NotFoundHandler.h"
#include "utils.h"
#include "handlers/ApplePushHandler.h"
#include "handlers/GooglePushHandler.h"


using namespace proxygen;

class HandlerFactory : public RequestHandlerFactory{
public:
    void onServerStart(folly::EventBase* /*evb*/) noexcept override;

    void onServerStop() noexcept override;

    RequestHandler* onRequest(RequestHandler*, HTTPMessage*) noexcept override;

private:

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_HANDLERFACTORY_H
