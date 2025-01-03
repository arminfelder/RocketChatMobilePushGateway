FROM docker.io/debian:12

ARG SSH_PASS
ARG UID=1000
RUN useradd -m -u ${UID} -s /bin/bash builder


LABEL maintainer="armin.felder@osalliance.com"

RUN test -n "$SSH_PASS"

# Use noninteractive environment for apt
ENV DEBIAN_FRONTEND=noninteractive

# Update dependencies and install required packages
RUN apt-get update \
    && apt-get install --yes \
        libjsoncpp-dev \
        cmake \
        build-essential \
        gdb \
        git \
        gcc \
        g++ \
        zlib1g-dev \
        openssl \
        libssl-dev \
        software-properties-common \
        sudo \
        curl \
        wget \
        make \
        pkg-config \
        locales \
        libssl-dev \
        libc-ares-dev \
        uuid \
        uuid-dev \
        libossp-uuid-dev \
        libgflags-dev \
        doxygen \
        openssh-server \
        rsync \
        libpsl-dev \
        libnghttp2-dev \
        libnghttp3-dev


RUN mkdir /var/run/sshd

RUN echo 'root:${SSH_PASS}' | chpasswd

RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

RUN chmod -R 777 /tmp

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
