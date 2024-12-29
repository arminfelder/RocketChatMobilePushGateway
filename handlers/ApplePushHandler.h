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

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHHANDLER_H
#define ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHHANDLER_H

#include <drogon/HttpController.h>

using namespace drogon;

class ApplePushHandler : public HttpController<ApplePushHandler>{
public:

    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ApplePushHandler::pushMessage, "/push/apn/send", Post);

    METHOD_LIST_END

    void pushMessage(const HttpRequestPtr &,
                      std::function<void(const HttpResponsePtr &)> &&callback) const;

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHHANDLER_H
