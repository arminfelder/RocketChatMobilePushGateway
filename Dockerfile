FROM debian:latest

RUN apt-get update &&
apt-get install build-essential cmake libcurlpp-dev git ibjsoncpp-dev
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
 pkg-config -y &&
mkdir /proxygenSrc && cd /proxygenSrc &&
git clone https://github.com/facebook/proxygen.git &&
cd proxygen &&
https://github.com/facebook/folly.git &&
cd folly &&
autoreconf -ivf &&
./configure &&
make march=native &&
make check &&
make install &&
cd .. &&
cd proxygen &&
./deps.sh &&
./reinstall.sh &&
mkdir /pushGateway &&
cd /pushGateway &&
git clone https://git.fairkom.net/chat/RocketChatMobilePushGateway.git &&
cd RocketChatMobilePushGateway &&
cmake . &&
make march=native&&
./rocketChatMobilePushGateway

EXPOSE 11000
