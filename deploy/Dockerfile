FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    ca-certificates \
    libcurl4-openssl-dev \
    libncurses-dev \
    nlohmann-json3-dev \
    libglib2.0-dev \
    rpm

WORKDIR /app

COPY . .

ENTRYPOINT ["./deploy/docker-entrypoint.sh"]

