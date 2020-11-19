FROM ubuntu:18.04

RUN apt-get update \
    && apt-get install -y \
        git \
        make \
        g++ \
        libssl-dev \
    && rm -r /var/lib/apt/lists/*

RUN git clone https://github.com/snwagh/falcon-public.git Falcon

RUN cd Falcon \
    && make all -j$(nproc)

WORKDIR Falcon
