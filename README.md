<a href="https://semaphoreci.com/telemidia/ginga">
  Ubuntu 14.04: <img src="https://semaphoreci.com/api/v1/projects/067d8fed-5ecc-4408-b10a-20e615756bf2/1327970/shields_badge.svg" alt="Build Status (Ubuntu 14.04)" title="Build Status (Ubuntu 14.04)">
</a> |
<a href="https://travis-ci.org/TeleMidia/ginga/builds">
  Ubuntu 16.04: <img src="https://travis-ci.org/TeleMidia/ginga.svg?branch=master" alt="Build Status (Ubuntu 16.04)" title="Build Status (Ubuntu 16.04)">
</a> |
<a href="https://ci.appveyor.com/project/robertogerson/ginga">
  Windows: <img src="https://ci.appveyor.com/api/projects/status/1j9m853yd87o4691?svg=true" alt="Build Status (Win32)" title="Build Status (Win32)">
</a>

# Ginga

The iTV middleware.

  * http://www.ginga.org.br
  * http://www.ncl.org.br
  * http://www.telemidia.puc-rio.br

## Installation

### Development version (nightly builds)

**Linux Debian/Ubuntu/Linux Mint**:

In a Debian-based system you can easily install a (nightly built) development
version of ginga using:

    $ sudo add-apt-repository ppa:telemidia/daily-builds
    $ sudo apt-get update
    $ sudo apt-get install ginga-itv

**Windows**

TODO.

**macOS**

TODO.

## Usage

The basic way of running ginga is passing a `.ncl` document to be played as a
parameter, e.g.:

    $ ginga examples/primeiro-joao/01sync.ncl

You can check the available options for the `ginga` executable with:

    $ ginga --help

## Building

### Dependencies

Required:

  * cairo >= 1.10
  * glib >= 2.32
  * gstreamer >= 1.8
  * GTK+ >= 3.4
  * pango >= 1.30
  * libxml >= 2.9

Optional:

  * cef >= 3.0, https://bitbucket.org/chromiumembedded/cef
  * librsvg >= 2.40
  * lua >= 5.2
  * nclua >= 1.0, http://github.com/telemidia/nclua

### Linux

On Ubuntu-based distros, we suggest install the dependencies through apt-get.

    $ sudo apt-get install -y git gcc g++ autotools-dev dh-autoreconf cmake cmake-data \
        liblua5.2-dev libglib2.0-dev libxerces-c-dev libpango1.0-dev librsvg2-dev \
        libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
        libgstreamer-plugins-good1.0-dev libgtk-3-dev -qq

Build with autotools.

    $ ./bootstrap
    $ ./configure
    $ make

Or build with cmake. When using cmake, nclua will be automatically
downloaded and built.

    $ mkdir _build && cd _build
    $ cmake ../build-cmake
    $ make

### Windows

Download MSYS2 and install the dependencies through MinGW.

    $ pacman -Syu --noconfirm

    $ pacman -Sy --noconfirm base-devel mpfr cmake mingw64/mingw-w64-x86_64-glib2 glib2-devel \
        mingw64/mingw-w64-x86_64-cairo mingw64/mingw-w64-x86_64-gst-plugins-base \
        mingw64/mingw-w64-x86_64-gst-plugins-good mingw64/mingw-w64-x86_64-gst-plugins-bad \
        mingw64/mingw-w64-x86_64-gtk3 mingw64/mingw-w64-x86_64-pango \
        mingw64/mingw-w64-x86_64-xerces-c mingw-w64-x86_64-lua mingw64/mingw-w64-x86_64-gst-plugins-ugly \
        mingw-w64-x86_64-gst-libav-1.12.4-1 mingw-w64-x86_64-gcc mingw-w64-x86_64-make

Build with autotools.

    $ ./bootstrap
    $ ./configure
    $ make

### macOS

TODO

---
Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

Permission is granted to copy, distribute and/or modify this document under
the terms of the GNU Free Documentation License, Version 1.3 or any later
version published by the Free Software Foundation; with no Invariant
Sections, with no Front-Cover Texts, and with no Back-Cover Texts. A copy of
the license is included in the "GNU Free Documentation License" file as part
of this distribution.
