FROM afelder/proxygen:v2020.09.14.00-builder as builder

LABEL maintainer="armin.felder@osalliance.com"

RUN apt-get update \
 && apt-get --no-install-recommends -y install libjsoncpp-dev libcurl4-openssl-dev libboost-all-dev cmake build-essential \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

COPY . /pushGateway/RocketChatMobilePushGateway 
WORKDIR /pushGateway/RocketChatMobilePushGateway

RUN cmake . \
   && make -j$(($(nproc) + 1)) \
   && rm CMake* -rf \
   && rm cmake* -rf


FROM afelder/proxygen:v2020.09.14.00

RUN useradd --no-log-init -g 0 -u 1001 pushgateway

RUN mv /usr/local/lib/proxygen/cmake

RUN apt-get update \
 && apt-get install --no-install-recommends -y libjsoncpp1 libcurl4 libboost-context1.65.1 libboost-filesystem1.65.1 libboost-program-options1.65.1 libboost-regex1.65.1 libboost-system1.65.1 \
 && apt-get clean \
 && rm -rf /var/lib/apt/lists/*

COPY --from=builder /pushGateway/RocketChatMobilePushGateway /usr/local/bin/

RUN mkdir -p /etc/RocketChatMobilePushGateway/credentials/google \
 && mkdir -p /etc/RocketChatMobilePushGateway/credentials/apple \
 && chown -R 1001:0 /etc/RocketChatMobilePushGateway \
 && ln -s /certs /etc/RocketChatMobilePushGateway/credentials

EXPOSE 11000

USER 1001

CMD ["rocketChatMobilePushGateway","--logtostderr=1"]

