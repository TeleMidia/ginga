<div style="width:100%">
<a href="https://semaphoreci.com/telemidia/ginga">
  Ubuntu 14.04:
   <img
    src="https://semaphoreci.com/api/v1/projects/067d8fed-5ecc-4408-b10a-20e615756bf2/1327970/shields_badge.svg"
    alt="Build Status (Ubuntu 14.04)"
    title="Build Status (Ubuntu 14.04)">
</a>
&nbsp;&nbsp;&nbsp;
<a href="https://travis-ci.org/TeleMidia/ginga/builds">
  Ubuntu 16.04:
   <img
    src="https://travis-ci.org/TeleMidia/ginga.svg?branch=master"
    alt="Build Status (Ubuntu 16.04)"
    title="Build Status (Ubuntu 16.04)">
</a>
&nbsp;&nbsp;&nbsp;
<a href="https://ci.appveyor.com/project/robertogerson/ginga">
  Windows:
   <img
    src="https://ci.appveyor.com/api/projects/status/1j9m853yd87o4691?svg=true"
    alt="Build Status (Windows)" title="Build Status (Windows)">
</a>
</div>

# Ginga

The iTV middleware.

  * http://www.ginga.org.br
  * http://www.ncl.org.br
  * http://www.telemidia.puc-rio.br

## Releases

There are no stable releases yet.

## Nightly builds

**Linux Debian/Ubuntu/Mint**:

To install a nightly build of this repository you can run:

    $ sudo add-apt-repository ppa:telemidia/daily-builds
    $ sudo apt-get update
    $ sudo apt-get install ginga-itv

WARNING:  Nightly builds are automatically generated builds from the latest
development code.  By their nature these builds are likely to contain bugs.
**We value your help**.  Please, [report problems or give us feedback if you
find any problem](https://github.com/telemidia/ginga/issues).

<!--
**Windows**

TODO.

**macOS**

TODO.
-->

## Usage

To play an NCL file run:

    $ ginga examples/primeiro-joao/01sync.ncl

To check the available options run:

    $ ginga --help

## Building the repository

### Dependencies

Required:

  * Cairo >= 1.10
  * GLib >= 2.32
  * GStreamer >= 1.8
  * GTK+ >= 3.4
  * Pango >= 1.30
  * Libxml >= 2.9

Optional:

  * <a href="https://bitbucket.org/chromiumembedded/cef">CEF</a> >= 3.0
  * <a href="<https://wiki.gnome.org/action/show/Projects/LibRsvg>
"> librsvg </a> >= 2.40
  * Libsoup >= 2.42
  * <a href="https://www.lua.org">Lua</a> >= 5.2
  * <a href="https://github.com/TeleMidia/nclua">NCLua</a> >= 1.0

### Linux

On Ubuntu-based distros, to install the dependencies run:

    $ sudo apt-get install -y git gcc g++ autotools-dev dh-autoreconf \
        cmake cmake-data liblua5.2-dev libglib2.0-dev libpango1.0-dev \
        librsvg2-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
        libgstreamer-plugins-good1.0-dev libgtk-3-dev libsoup2.4-dev -qq

To build with autotools run:

    $ ./bootstrap
    $ ./configure
    $ make

To build with `cmake` run:

    $ mkdir _build && cd _build
    $ cmake ../build-cmake
    $ make

Build with `cmake` will downloads and builds the <a
href="https://github.com/TeleMidia/nclua">NCLua</a> library automatically.

To build on windows see [extra/windows/README.md](extra/windows/README.md).

To build on macOS see [extra/macos/README.md](extra/macos/README.md).

## Support

There is not a dedicated mailing list for Ginga yet.  If you have a question
or want to discuss the develoment of Ginga itself post an
[issue](https://github.com/telemidia/ginga/issues) or send a message to
info@telemidia.puc-rio.br.

## Development

  * <a href="http://www.telemidia.puc-rio.br/~gflima/misc/ginga">
    Read the API documentation</a>.

  * <a href="https://github.com/TeleMidia/ginga/issues">
    Find and file bug reports</a>.

---
Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

Permission is granted to copy, distribute and/or modify this document under
the terms of the GNU Free Documentation License, Version 1.3 or any later
version published by the Free Software Foundation; with no Invariant
Sections, with no Front-Cover Texts, and with no Back-Cover Texts. A copy of
the license is included in the "GNU Free Documentation License" file as part
of this distribution.
