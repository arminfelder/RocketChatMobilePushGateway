FROM afelder/proxygen:2018.1

ADD https://git.fairkom.net/api/v4/projects/chat%2FRocketChatMobilePushGateway/repository/commits version.json

RUN mkdir /pushGateway \
    && cd /pushGateway \
    && git clone --recursive https://git.fairkom.net/chat/RocketChatMobilePushGateway.git \
    && cd RocketChatMobilePushGateway \
    && cmake . \
    && make

RUN ln -s /certs /pushGateway/RocketChatMobilePushGateway/credentials

EXPOSE 11000

ENTRYPOINT ["/pushGateway/RocketChatMobilePushGateway/rocketChatMobilePushGateway"]

