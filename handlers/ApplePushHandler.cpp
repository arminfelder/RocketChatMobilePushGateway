//
// Created by armin on 16.11.17.
//

#include "ApplePushHandler.h"

void ApplePushHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {

}

void ApplePushHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {

}

void ApplePushHandler::onUpgrade(proxygen::UpgradeProtocol prot) noexcept {

}

void ApplePushHandler::onEOM() noexcept {

}

void ApplePushHandler::requestComplete() noexcept {

}

void ApplePushHandler::onError(ProxygenError err) noexcept {

}
