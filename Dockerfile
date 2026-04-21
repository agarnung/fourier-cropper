# syntax=docker/dockerfile:1
# BuildKit: caché de apt y de compilación (rebuilds mucho más rápidas).
#   DOCKER_BUILDKIT=1 docker compose build
FROM ubuntu:24.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive \
    PATH="/usr/lib/ccache:${PATH}"
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
    apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        ccache \
        qtbase5-dev \
        qttools5-dev-tools \
        pkg-config \
        libopencv-dev \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY fourier-cropper.pro ./
COPY assets/ ./assets/
COPY app/ ./app/
COPY widgets/ ./widgets/
COPY libs/ ./libs/
COPY main.cpp ./
RUN sed -i 's|/usr/local/include/opencv4|/usr/include/opencv4|g' fourier-cropper.pro
RUN --mount=type=cache,target=/root/.ccache \
    mkdir -p build \
    && cd build \
    && qmake CONFIG+=release ../fourier-cropper.pro \
    && make -j"$(nproc)" \
    && install -m755 FourierCropper /usr/local/bin/FourierCropper

FROM ubuntu:24.04
# Runtime OpenCV: solo lo que enlaza el binario (core/imgproc/imgcodecs).
ENV DEBIAN_FRONTEND=noninteractive
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt/lists,sharing=locked \
    apt-get update \
    && apt-get install -y --no-install-recommends \
        libqt5widgets5 \
        libqt5gui5 \
        libqt5core5a \
        libqt5svg5 \
        libglib2.0-0 \
        libfontconfig1 \
        libfreetype6 \
        libxcb-xinerama0 \
        libxcb-cursor0 \
        libxkbcommon-x11-0 \
        libdbus-1-3 \
        libxcb1 \
        libx11-6 \
        libx11-xcb1 \
        libsm6 \
        libice6 \
        libgl1 \
        libqt5dbus5 \
        libopencv-core406t64 \
        libopencv-imgproc406t64 \
        libopencv-imgcodecs406t64 \
    && rm -rf /var/lib/apt/lists/*
COPY --from=builder /usr/local/bin/FourierCropper /usr/local/bin/FourierCropper
RUN ! ldd /usr/local/bin/FourierCropper | grep -q 'not found'
WORKDIR /opt/fourier-cropper
ENV QT_QPA_PLATFORM=xcb
ENTRYPOINT ["/usr/local/bin/FourierCropper"]
