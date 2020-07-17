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

#include "HandlerFactory.h"

void HandlerFactory::onServerStart(folly::EventBase* /*evb*/) noexcept{

}


void HandlerFactory::onServerStop() noexcept{

}

RequestHandler* HandlerFactory::onRequest(RequestHandler* requestHandler, HTTPMessage* httpMessage) noexcept{
   UNUSED(requestHandler);
    if(httpMessage) {
        std::vector<std::string> segments = utils::getSegments(httpMessage->getURL());
        if(segments.size()>2){
            if(segments[0] == "push"&&segments[2] == "send"){
                if(segments[1] == "gcm"){
                    return new GooglePushHandler();
                }else if(segments[1] == "apn"){
                    return new ApplePushHandler();
                }
            }
        }
    }
    return new NotFoundHandler();
}
