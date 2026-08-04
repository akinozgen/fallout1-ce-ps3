# ps3dev toolchain image — Fallout 1 CE PS3 portu icin
#
# ONEMLI: Resmi ps3dev toolchain'i DEGIL, ps3aqua fork'u kullaniliyor.
# Sebep: SDL2_PSL1GHT (ps3aqua) su ucluye bagimli, karisik eslestirme derlenmiyor:
#   - ps3aqua/ps3toolchain  -> GCC 7.5.0 + newlib 4.4.0 (PS3 yamali; sys/select.h vb. icerir)
#   - ps3aqua/PSL1GHT       -> sysutil/video_out.h, audio_out.h (ps3dev master'da yok)
#   - ps3aqua/SDL2_PSL1GHT  -> yukaridakilerin header/lib'lerine gore yazilmis
# (ps3dev'in prebuilt nightly tarball'i denendi: newlib'inde sys/select.h yok,
#  PSL1GHT master'inda video_out.h yok -> SDL2 derlenmiyor.)
#
# Toolchain kaynaktan derlenir (~1-2 saat, tek seferlik; Docker layer cache'te kalir).
# GDB adimlari (003, 007) atlanir.
#
# Build (proje kokunden):
#   docker build -t ps3dev -f docker/Dockerfile .
# Kullanim:
#   docker run --rm -v "$PWD":/src -w /src ps3dev make

FROM ubuntu:24.04

SHELL ["/bin/bash", "-o", "pipefail", "-c"]
ENV DEBIAN_FRONTEND=noninteractive

# archive.ubuntu.com zaman zaman erisilemez ya da asiri yavas olabiliyor;
# TR aynasi kullaniliyor. Alternatif: azure.archive.ubuntu.com (CDN destekli).
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

# --- Toolchain + PSL1GHT (kaynaktan) ---
# Adimlar: 001 binutils-PPU, 002 gcc+newlib-PPU, 004 symlinks,
#          005 binutils-SPU, 006 gcc+newlib-SPU, 008 PSL1GHT (pinli ps3aqua commit)
# Katman katman ayrildi ki build kesilirse cache'ten devam etsin.
RUN git clone --depth 1 https://github.com/ps3aqua/ps3toolchain.git /opt/ps3toolchain
# ftp.gnu.org araliklarla HTTP 500 dondurebiliyor; kernel.org'un GNU aynasi
# kullaniliyor. newlib kaynagi (sourceware) bundan etkilenmez.
RUN sed -i 's|https://ftp.gnu.org/gnu/|https://mirrors.kernel.org/gnu/|g' /opt/ps3toolchain/scripts/*.sh
RUN cd /opt/ps3toolchain && ./toolchain.sh 1
RUN cd /opt/ps3toolchain && ./toolchain.sh 2
RUN cd /opt/ps3toolchain && ./toolchain.sh 4
RUN cd /opt/ps3toolchain && ./toolchain.sh 5
RUN cd /opt/ps3toolchain && ./toolchain.sh 6
# python3-setuptools: PSL1GHT'in pkgcrypt araci "import distutils" yapiyor;
# Python 3.12'de stdlib'den kalkti, setuptools shim sagliyor. Bilerek ana apt
# katmanina EKLENMEDI — oraya eklemek GCC katmanlarinin cache'ini patlatir.
RUN apt-get update && apt-get install -y --no-install-recommends python3-setuptools \
    && rm -rf /var/lib/apt/lists/*
RUN cd /opt/ps3toolchain && ./toolchain.sh 8 \
    && rm -rf /opt/ps3toolchain/build \
    && ppu-gcc --version

# --- SDL2_PSL1GHT (lokal klon; hazir configure bayat oldugundan autogen sart) ---
COPY SDL2_PSL1GHT /opt/SDL2_PSL1GHT
RUN cd /opt/SDL2_PSL1GHT \
    && ./autogen.sh \
    && mkdir -p build-ps3 && cd build-ps3 \
    && ../configure --host=powerpc64-ps3-elf --prefix="$PS3DEV/portlibs/ppu" \
    && make -j"$(nproc)" \
    && make install

WORKDIR /src
