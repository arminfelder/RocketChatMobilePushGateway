//
// Created by armin on 09.11.17.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_HANDLERFACTORY_H
#define ROCKETCHATMOBILEPUSHGATEWAY_HANDLERFACTORY_H

#include <folly/Memory.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>

#include "handlers/TestHandler.h"
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
