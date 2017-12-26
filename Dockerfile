FROM debian:latest

RUN apt-get update \ 
    && apt-get install apt-transport-https \
    ca-certificates \
    curl \
    gnupg2 \
    software-properties-common -y

RUN apt-get install \
    sudo \
    build-essential cmake libcurl4-gnutls-dev git libjsoncpp-dev \
    g++ \
    automake \
    autoconf \
    autoconf-archive \
    libtool \
    libboost-all-dev \
    libevent-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    make \
    zlib1g-dev \
    binutils-dev \
    libjemalloc-dev \
    libssl-dev \
    pkg-config -y \
    wget

RUN mkdir /proxygenSrc \
    && cd /proxygenSrc \
    && git clone https://github.com/facebook/proxygen.git


RUN cd /proxygenSrc/proxygen \
    && git clone https://github.com/facebook/folly.git

RUN cd /proxygenSrc/proxygen/folly/folly/test \
    && rm -rf gtest \
    && wget https://github.com/google/googletest/archive/release-1.8.0.tar.gz \
    && tar zxf release-1.8.0.tar.gz \
    && rm -f release-1.8.0.tar.gz \
    && mv googletest-release-1.8.0 gtest

ENV CXXFLAGS "$CXXFLAGS -march=native"
ENV CFLAGS "$CFLAGS -march=native"
ENV MAKEFLAGS="-j$[$(nproc) + 1]"

RUN cd /proxygenSrc/proxygen/folly/folly \
    && autoreconf -ivf \
    && ./configure \
    && make \
    && make install \
    && cd ..

RUN cd /proxygenSrc/proxygen/proxygen \
    && ./deps.sh \
    && ./reinstall.sh

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

