FROM ubuntu:16.04

RUN apt-get update -y
RUN apt-get install -y apt-utils | true
RUN apt-get install -y software-properties-common python-software-properties
RUN apt-get update -y

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-add-repository -y ppa:george-edison55/cmake-3.x
RUN add-apt-repository -y ppa:gnome3-team/gnome3
RUN add-apt-repository -y ppa:gnome3-team/gnome3-staging
RUN apt-get update -y

RUN apt-get install -y gcc-5 g++-5 cmake cmake-data liblua5.2-dev libglib2.0-dev libxerces-c-dev libsdl2-dev libsdl2-image-dev libpango1.0-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-good1.0-dev libgtk-3-dev
RUN cmake build-cmake -DWITH_CEF=OFF

RUN make -j4

