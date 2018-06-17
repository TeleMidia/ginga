FROM ubuntu:16.04

# Setup
RUN apt-get update -y -qq
RUN apt-get install -y apt-utils -qq
RUN apt-get install -y software-properties-common python-software-properties -qq
RUN apt-get update -y -qq

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-add-repository -y ppa:george-edison55/cmake-3.x
RUN add-apt-repository -y ppa:gnome3-team/gnome3
RUN apt-get update -y -qq

RUN apt-get install -y git gcc g++ autotools-dev dh-autoreconf \
        cmake cmake-data liblua5.2-dev libglib2.0-dev libpango1.0-dev \
        librsvg2-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
        gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-libav \
        libgtk-3-dev libsoup2.4-dev -qq

# Build preparation
RUN mkdir -p /src/
RUN git clone https://github.com/telemidia/ginga.git /src/ginga
RUN mkdir -p /src/ginga/_build

# Build - cmake
WORKDIR /src/ginga/_build
RUN cmake ../build-cmake -DWITH_CEF=OFF
RUN make -j4

# Build - autotools
RUN git clone https://github.com/telemidia/nclua /src/nclua
WORKDIR /src/nclua
RUN ./bootstrap && ./configure
RUN make -j8
RUN make install

WORKDIR /src/ginga/
RUN ./bootstrap && ./configure
RUN make -j4

