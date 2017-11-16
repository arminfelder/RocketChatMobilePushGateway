//
// Created by armin on 16.11.17.
//

#include "GooglePushHandler.h"
#include "../models/GooglePushModel.h"

void GooglePushHandler::onRequest(std::unique_ptr<HTTPMessage> headers) noexcept {

}

void GooglePushHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
    if(mBody){
        mBody->prependChain(std::move(body));
    } else{
        mBody = std::move(body);
    }
}

void GooglePushHandler::onUpgrade(proxygen::UpgradeProtocol prot) noexcept {

}

void GooglePushHandler::onEOM() noexcept {
    if (mBody) {
        try {
            std::string body(reinterpret_cast<const char *>(mBody->data()));
            GooglePushModel googlePushModel(body);
            std::string response;
            if (googlePushModel.sendMessage()) {
                ResponseBuilder(downstream_).status(200, "OK").body("").sendWithEOM();

            } else {
                ResponseBuilder(downstream_).status(500, "FAILURE").body("failed to send push mesage").sendWithEOM();

            }

        } catch (Exception &e) {
            std::cout << "exception " << e.what() << std::endl;
            ResponseBuilder(downstream_).status(500, "FAILURE").body("failed to send push mesage").sendWithEOM();
        }
    } else {
        ResponseBuilder(downstream_).status(400, "BAD REQUEST").body("failed to send push message").sendWithEOM();
    }
}

void GooglePushHandler::requestComplete() noexcept {

}

void GooglePushHandler::onError(ProxygenError err) noexcept {

}
