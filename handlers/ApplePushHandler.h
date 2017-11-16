//
// Created by armin on 16.11.17.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHHANDLER_H
#define ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHHANDLER_H

#include <folly/Memory.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>


using namespace proxygen;

class ApplePushHandler: public RequestHandler {
public:
    void onRequest(std::unique_ptr<HTTPMessage> headers) noexcept override  ;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override  ;

    void onUpgrade(proxygen::UpgradeProtocol prot) noexcept override ;

    void onEOM() noexcept override;

    void requestComplete() noexcept override;

    void onError(ProxygenError err) noexcept override;

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_APPLEPUSHHANDLER_H
