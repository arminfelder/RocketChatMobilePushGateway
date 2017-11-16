//
// Created by armin on 10.11.17.
//

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
