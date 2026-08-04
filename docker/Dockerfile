# ps3dev toolchain image - for the Fallout 1 CE PS3 port
#
# IMPORTANT: this uses the ps3aqua fork, NOT the official ps3dev toolchain.
# Reason: SDL2_PSL1GHT (ps3aqua) depends on this trio; a mixed match does not build:
#   - ps3aqua/ps3toolchain  -> GCC 7.5.0 + newlib 4.4.0 (PS3-patched; includes sys/select.h etc.)
#   - ps3aqua/PSL1GHT       -> sysutil/video_out.h, audio_out.h (absent from ps3dev master)
#   - ps3aqua/SDL2_PSL1GHT  -> written against the headers/libs of the above
# (ps3dev's prebuilt nightly tarball was tried: its newlib has no sys/select.h,
#  PSL1GHT master has no video_out.h -> SDL2 does not build.)
#
# The toolchain is built from source (~1-2 hours, one time; stays in the Docker layer cache).
# The GDB steps (003, 007) are skipped.
#
# Build (from the project root):
#   docker build -t ps3dev -f docker/Dockerfile .
# Usage:
#   docker run --rm -v "$PWD":/src -w /src ps3dev make

FROM ubuntu:24.04

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
ENV DEBIAN_FRONTEND=noninteractive

# archive.ubuntu.com is sometimes unreachable or extremely slow, so the TR
# mirror is used. Alternative: azure.archive.ubuntu.com (CDN backed).
RUN sed -i 's|http://archive.ubuntu.com/ubuntu|http://tr.archive.ubuntu.com/ubuntu|g; s|http://security.ubuntu.com/ubuntu|http://tr.archive.ubuntu.com/ubuntu|g' \
        /etc/apt/sources.list.d/ubuntu.sources

RUN apt-get update && apt-get install -y --no-install-recommends \
        autoconf \
        automake \
        bison \
        bzip2 \
        ca-certificates \
        clang \
        cmake \
        curl \
        flex \
        g++ \
        gcc \
        git \
        libelf-dev \
        libgmp-dev \
        libmpc-dev \
        libmpfr-dev \
        libncurses-dev \
        libssl-dev \
        libtool \
        libtool-bin \
        make \
        patch \
        pkg-config \
        python-is-python3 \
        python3 \
        python3-dev \
        texinfo \
        wget \
        xz-utils \
        zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

ENV PS3DEV=/usr/local/ps3dev
ENV PSL1GHT=$PS3DEV
ENV PATH=$PS3DEV/bin:$PS3DEV/ppu/bin:$PS3DEV/spu/bin:$PATH

# --- Toolchain + PSL1GHT (from source) ---
# Steps: 001 binutils-PPU, 002 gcc+newlib-PPU, 004 symlinks,
#        005 binutils-SPU, 006 gcc+newlib-SPU, 008 PSL1GHT (pinned ps3aqua commit)
# Split layer by layer so an interrupted build can resume from the cache.
RUN git clone --depth 1 https://github.com/ps3aqua/ps3toolchain.git /opt/ps3toolchain
# ftp.gnu.org intermittently returns HTTP 500, so kernel.org's GNU mirror is
# used. The newlib source (sourceware) is not affected by this.
RUN sed -i 's|https://ftp.gnu.org/gnu/|https://mirrors.kernel.org/gnu/|g' /opt/ps3toolchain/scripts/*.sh
RUN cd /opt/ps3toolchain && ./toolchain.sh 1
RUN cd /opt/ps3toolchain && ./toolchain.sh 2
RUN cd /opt/ps3toolchain && ./toolchain.sh 4
RUN cd /opt/ps3toolchain && ./toolchain.sh 5
RUN cd /opt/ps3toolchain && ./toolchain.sh 6
# python3-setuptools: PSL1GHT's pkgcrypt tool does "import distutils", which was
# dropped from the stdlib in Python 3.12; setuptools provides the shim.
# Deliberately NOT added to the main apt layer - doing so would blow the cache
# of the GCC layers.
RUN apt-get update && apt-get install -y --no-install-recommends python3-setuptools \
    && rm -rf /var/lib/apt/lists/*
RUN cd /opt/ps3toolchain && ./toolchain.sh 8 \
    && rm -rf /opt/ps3toolchain/build \
    && ppu-gcc --version

# --- SDL2_PSL1GHT (local clone; the shipped configure is stale, so autogen is required) ---
COPY SDL2_PSL1GHT /opt/SDL2_PSL1GHT
RUN cd /opt/SDL2_PSL1GHT \
    && ./autogen.sh \
    && mkdir -p build-ps3 && cd build-ps3 \
    && ../configure --host=powerpc64-ps3-elf --prefix="$PS3DEV/portlibs/ppu" \
    && make -j"$(nproc)" \
    && make install

WORKDIR /src
