FROM afelder/proxygen:2018.1

ADD . /pushGateway/RocketChatMobilePushGateway 

RUN cd /pushGateway/RocketChatMobilePushGateway \
   &&cmake . \
   && make \
   && rm CMake* -rf \
   && rm cmake* -rf

RUN ln -s /certs /pushGateway/RocketChatMobilePushGateway/credentials

EXPOSE 11000

ENTRYPOINT ["/pushGateway/RocketChatMobilePushGateway/rocketChatMobilePushGateway"]

