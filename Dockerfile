# Build stage
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake ca-certificates \
	qtbase5-dev libqt5dbus5 libqt5widgets5 \
    libcurl4-openssl-dev libncurses-dev nlohmann-json3-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .
RUN mkdir build && cd build && cmake .. && make -j$(nproc)

# Runtime stage
FROM ubuntu:22.04

ARG UID=1000
ARG GID=1000
ARG USERNAME=rwal

RUN apt-get update && apt-get install -y --no-install-recommends \
    libqt5core5a libqt5dbus5 libqt5widgets5 \
	libcurl4 ca-certificates libncurses6 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN groupadd -g ${GID} ${USERNAME} || true && \
    useradd -m -u ${UID} -g ${USERNAME} -s /bin/bash ${USERNAME} || true

USER ${USERNAME}
WORKDIR /home/${USERNAME}

COPY --from=builder /build/build/rwal /usr/local/bin/rwal

ENTRYPOINT ["/usr/local/bin/rwal"]
