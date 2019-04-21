FROM afelder/proxygen:latest

LABEL maintainer="armin.felder@osalliance.com"

RUN apt-get update && apt-get --yes install libjsoncpp-dev libcurl4-openssl-dev cmake && apt-get clean
ADD . /pushGateway/RocketChatMobilePushGateway 

RUN cd /pushGateway/RocketChatMobilePushGateway \
   && cmake . \
   && make -j$[$(nproc) + 1] \
   && rm CMake* -rf \
   && rm cmake* -rf

RUN ln -s /certs /pushGateway/RocketChatMobilePushGateway/credentials

EXPOSE 11000

ENTRYPOINT ["/pushGateway/RocketChatMobilePushGateway/rocketChatMobilePushGateway"]

