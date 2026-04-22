# Stage 1 - Build the application
FROM debian:bookworm AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ca-certificates \
    qtbase5-dev \
    libqt6dbus6 \
    libqt6widgets6 \
    libcurl4-openssl-dev \
    libncurses-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy the source code and build the application
WORKDIR /build
COPY . .

# Build the application
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel $(nproc)

# Stage 2 - Create the runtime image
FROM debian:bookworm-slim

# Set build arguments for user and group IDs
ARG UID=1000
ARG GID=1000
ARG USERNAME=rwal

# Install runtime dependencies
RUN apt-get update && apt-get upgrade -y && apt-get install -y --no-install-recommends \
    libqt6core6a \
    libqt6dbus6 \
    libqt6widgets6r\
    libcurl4 \
    ca-certificates \
    libncurses6 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Create a non-root user to run the application
RUN groupadd -g ${GID} ${USERNAME} 2>/dev/null || true && \
    useradd -m -u ${UID} -g ${USERNAME} -s /bin/bash ${USERNAME} 2>/dev/null || true

# Create necessary directories and set permissions
RUN mkdir -p /home/${USERNAME}/.local/share/Aloncie/Rwal/downloads && \
    chown -R ${USERNAME}:${USERNAME} /home/${USERNAME}

# Switch to the non-root user
USER ${USERNAME}
WORKDIR /home/${USERNAME}

# Copy the built application from the builder stage
COPY --from=builder /build/build/rwal /usr/local/bin/rwal

# Set the entry point to run the application
ENTRYPOINT ["/usr/local/bin/rwal"]
