FROM ubuntu:16.04

RUN \
  apt update && apt upgrade -y && \
  apt install -y \
  build-essential \
  git \
  autoconf \
  cmake \
  python \
  unzip \
  cpio \
  wget \
  vim \
  locales \
  lzop \
  bc \
  nano \
  libncurses5-dev

RUN locale-gen --no-purge en_US.UTF-8

COPY fs /openfang/fs
COPY legacy_src /openfang/legacy_src
COPY config /openfang/config
COPY patches /openfang/patches
COPY buildscripts /openfang/buildscripts

WORKDIR "/openfang/_build"
RUN wget https://buildroot.org/downloads/buildroot-2016.02.tar.gz;
RUN tar xvf buildroot-2016.02.tar.gz;
RUN rm buildroot-2016.02.tar.gz;



# Change to the buildroot directory
WORKDIR "/openfang/_build/buildroot-2016.02"

RUN patch -p1 < /openfang/patches/add_fp_no_fused_madd.patch

# Replace the default buildroot config files with our custom ones
RUN cp /openfang/config/buildroot.config .config
RUN cp /openfang/config/busybox.config package/busybox
RUN cp /openfang/config/uClibc-ng.config package/uclibc

# We want to use specific sources so copy these into the download directory
RUN mkdir -p dl
RUN cp /openfang/legacy_src/kernel-3.10.14.tar.xz dl/
RUN cp /openfang/legacy_src/uboot-v2013.07.tar.xz dl/

RUN make

WORKDIR /openfang

RUN apt install -y nfs-kernel-server
RUN apt install -y tftpd-hpa

# Copy over custom packages such as v4l2rtspserver
RUN cp -r custompackages/package/* /openfang/_build/buildroot-2016.02/package/

ENTRYPOINT ["tail", "-f", "/dev/null"]
