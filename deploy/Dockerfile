FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    ca-certificates \
    libcurl4-openssl-dev \
    libncurses-dev \
    nlohmann-json3-dev

WORKDIR /app

COPY . .

ENTRYPOINT ["./deploy/docker-entrypoint.sh"]

