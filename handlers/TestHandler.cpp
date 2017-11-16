//
// Created by armin on 10.11.17.
//

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