# Build stage
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake qtbase5-dev libqt5dbus5 libqt5widgets5 \
    libcurl4-openssl-dev libncurses-dev nlohmann-json3-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .
RUN mkdir build && cd build && cmake .. && make -j$(nproc)

# Runtime stage
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y --no-install-recommends \
    libqt5core5a libqt5dbus5 libqt5widgets5 libcurl4 libncurses6 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -u 1000 rwal && \
    mkdir -p /home/rwal/.config/Aloncie /home/rwal/.cache/Aloncie /home/rwal/Pictures/Rwal && \
    chown -R rwal:rwal /home/rwal

COPY --from=builder /build/build/rwal /usr/local/bin/rwal

USER rwal
WORKDIR /home/rwal

ENTRYPOINT ["/usr/local/bin/rwal"]
