# first stage
FROM rwal-base:latest AS builder

COPY --chown=1000:1000 . .
RUN rm -rf build && \
    mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# second stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libqt5core5a \
    libqt5dbus5 \
    libqt5widgets5 \
    libcurl4 \
    libncurses6 \
    && rm -rf /var/lib/apt/lists/*

# Create necessary directories (with correct case)
RUN mkdir -p /home/user/.config/Aloncie/Rwal \
             /home/user/.cache/Aloncie/Rwal \
             /home/user/Pictures/Rwal

COPY --from=builder /app/build/rwal /usr/local/bin/rwal

# Non-root user for runtime
RUN useradd -m user
USER user
WORKDIR /home/user

CMD ["rwal"]
