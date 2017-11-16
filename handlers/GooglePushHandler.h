//
// Created by armin on 16.11.17.
//

#ifndef ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHHANDLER_H
#define ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHHANDLER_H

#include <folly/Memory.h>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <proxygen/httpserver/ResponseBuilder.h>
#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>
#include <jsoncpp/json/json.h>


using namespace proxygen;

class GooglePushHandler:  public RequestHandler{
public:
    void onRequest(std::unique_ptr<HTTPMessage> headers) noexcept override;

    void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;

    void onUpgrade(proxygen::UpgradeProtocol prot) noexcept override;

    void onEOM() noexcept override;

    void requestComplete() noexcept override;

    void onError(ProxygenError err) noexcept override;

private:
    std::unique_ptr<folly::IOBuf> mBody;

};


#endif //ROCKETCHATMOBILEPUSHGATEWAY_GOOGLEPUSHHANDLER_H
