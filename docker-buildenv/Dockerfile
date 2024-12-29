FROM docker.io/debian:12

LABEL maintainer="armin.felder@osalliance.com"

# Use noninteractive environment for apt
ENV DEBIAN_FRONTEND=noninteractive

# Update dependencies and install required packages
RUN apt-get update \
    && apt-get install --yes \
        libjsoncpp-dev \
        libcurl4-openssl-dev \
        libboost-all-dev \
        cmake \
        build-essential \
        gdb \
        git gcc g++ cmake \
    zlib1g-dev \
    openssl \
     libssl-dev \
    software-properties-common \
        sudo \
    curl \
    wget \
    cmake \
    make \
    pkg-config \
    locales \
    git \
        openssl \
     libssl-dev \
     libc-ares-dev

RUN apt-get update \
      && apt-get install --yes \
        uuid \
        uuid-dev \
        libossp-uuid-dev \
        libgflags-dev \
        doxygen \
        openssh-server \
        rsync


RUN mkdir /var/run/sshd

RUN echo 'root:GaxahhohphuYea9cah4Od2equ0cui3eich4' | chpasswd

RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
