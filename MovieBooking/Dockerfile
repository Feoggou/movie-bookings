FROM ubuntu:24.04

ADD docker_init.sh /
RUN chmod +x /docker_init.sh

RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    g++ \
    zip \
    cmake \
    git \
    ninja-build \
    pkg-config \
    doxygen \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /project

CMD ["/docker_init.sh"]
