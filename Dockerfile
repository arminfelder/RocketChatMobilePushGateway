# Stage 1: Build Stage
FROM debian:12 as build_stage
LABEL maintainer="armin.felder@osalliance.com"

# Use noninteractive environment for apt
ENV DEBIAN_FRONTEND=noninteractive

# Define build dependencies
ARG BUILD_DEPS="\
    libjsoncpp-dev \
    cmake \
    build-essential \
    git \
    zlib1g-dev \
    openssl \
    libssl-dev \
    uuid-dev \
    libc-ares-dev \
    libossp-uuid-dev \
    libgflags-dev \
    software-properties-common \
    curl \
    wget \
    make \
    pkg-config \
    locales \
    zlib1g-dev \
"

# Install dependencies
RUN apt-get update \
    && apt-get install --yes $BUILD_DEPS \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
COPY . /pushGateway/RocketChatMobilePushGateway
WORKDIR /pushGateway/RocketChatMobilePushGateway

# Build the project
RUN mkdir build && cd build && cmake .. && make -j$(nproc) \
    && rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake

# Stage 2: Runtime Stage
FROM debian:12-slim

# Create non-root user for execution
RUN useradd --no-log-init -g 0 -u 1001 pushgateway

# Define runtime dependencies
ARG RUNTIME_DEPS="libjsoncpp25 libgflags2.2 libc-ares2 libssl3"

# Install runtime dependencies
RUN apt-get update \
    && apt-get install --no-install-recommends -y $RUNTIME_DEPS \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Copy built artifacts
COPY --from=build_stage /pushGateway/RocketChatMobilePushGateway/build/rocketChatMobilePushGateway /usr/local/bin/

# Prepare directory structure
RUN mkdir -p /usr/local/etc/RocketChatMobilePushGateway/credentials/google \
    && mkdir -p /usr/local/etc/RocketChatMobilePushGateway/credentials/apple \
    && chown -R 1001:0 /usr/local/etc/RocketChatMobilePushGateway \
    && ln -s /certs /usr/local/etc/RocketChatMobilePushGateway/credentials

EXPOSE 11000

# Switch to non-root user
USER 1001

# Default command
CMD ["rocketChatMobilePushGateway"]
