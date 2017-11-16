//
// Created by armin on 09.11.17.
//

#include "HandlerFactory.h"

void HandlerFactory::onServerStart(folly::EventBase* /*evb*/) noexcept{

}


void HandlerFactory::onServerStop() noexcept{

}

RequestHandler* HandlerFactory::onRequest(RequestHandler* requestHandler, HTTPMessage* httpMessage) noexcept{
    if(httpMessage) {
        std::cout << "path " << httpMessage->getPath()<<std::endl;
        std::cout<< "url "<<httpMessage->getURL()<<std::endl;
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
    return new TestHandler();
}
