FROM afelder/proxygen:v2020.07.13.00-builder

LABEL maintainer="armin.felder@osalliance.com"

RUN apt-get update && apt-get --yes install libjsoncpp-dev libcurl4-openssl-dev libboost-all-dev cmake build-essential&& apt-get clean
ADD . /pushGateway/RocketChatMobilePushGateway 

RUN cd /pushGateway/RocketChatMobilePushGateway \
   && cmake . \
   && make -j$[$(nproc) + 1] \
   && rm CMake* -rf \
   && rm cmake* -rf


FROM afelder/proxygen:v2020.07.13.00

RUN groupadd -g 1001 pushgateway && \
    useradd --no-log-init -g 1001 -u 1001 pushgateway

RUN apt-get update && apt-get install -y libjsoncpp1 libcurl4 libboost-context1.65.1 libboost-filesystem1.65.1 libboost-program-options1.65.1 libboost-regex1.65.1 libboost-system1.65.1 && apt-get clean

COPY --from=0 /pushGateway/RocketChatMobilePushGateway /usr/local/bin/


RUN mkdir -p /etc/RocketChatMobilePushGateway/credentials
RUN mkdir -p /etc/RocketChatMobilePushGateway/credentials/google
RUN mkdir -p /etc/RocketChatMobilePushGateway/credentials/apple
RUN chown -R 1001:1001 /etc/RocketChatMobilePushGateway
RUN mkdir /certs
RUN chown -R 1001:1001 /certs
RUN ln -s /certs /etc/RocketChatMobilePushGateway/credentials

EXPOSE 11000

USER 1001

CMD ["rocketChatMobilePushGateway","--logtostderr=1"]

