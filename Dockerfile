FROM ubuntu:18.04

RUN apt-get update \
    && apt-get install -y \
        git \
        make \
        g++ \
        libssl-dev \
        nano \
        tmux \
    && rm -r /var/lib/apt/lists/*

RUN git clone https://github.com/trhieung/falcon-public Falcon

WORKDIR Falcon

RUN chmod +x /Falcon/entrypoint.sh
ENTRYPOINT ["/Falcon/entrypoint.sh"]