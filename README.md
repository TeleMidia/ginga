# Ginga

The iTV middleware. More information in http://www.ginga.org.br

To play an NCL file, run:

```bash
ginga examples/primeiro-joao/01sync.ncl
```

## Nightly builds on Ubuntu-based systems

```bash
sudo add-apt-repository ppa:telemidia/daily-builds
sudo apt-get update
sudo apt-get install ginga-itv
```

## Build

Required dependencies: Cairo >= 1.10, GLib >= 2.32, GStreamer >= 1.8, GTK+ >= 3.4, Pango >= 1.30, Libxml >= 2.9, FontConfig, libsoup >= 2.42, Lua</a> >= 5.2, <a href="https://github.com/TeleMidia/nclua">NCLua</a> >= 1.0 (may use ppa:telemidia/daily-builds)
Optional dependencies: CEF >= 3.0, librsvg >= 2.40

### Build on Ubuntu-based system

Install depedencies:

```bash
sudo apt-get install -y pkg-config dpkg-dev dh-autoreconf libltdl-dev liblua5.2-dev libglib2.0-dev libpango1.0-dev librsvg2-dev libsoup2.4-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libfontconfig1-dev libgtk-3-dev gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav libgssdp-1.2-dev libjsoncpp-dev nclua -qq
```

Build with `cmake` (main way):

```bash
mkdir _build && cd _build
cmake ../build-cmake
make
```

Build with autotools (decrypted in favor cmake):

```bash
./bootstrap
./configure --prefix=/usr/
make
```

### Build on windows

See [extra/windows/README.md](extra/windows/README.md).

### Build on macos

See [extra/macos/README.md](extra/macos/README.md).

## Support

There is not a dedicated mailing list for Ginga. If you have a question or want to discuss the develoment of Ginga itself post an [issue](https://github.com/telemidia/ginga/issues) or send a message to info@telemidia.puc-rio.br.

* <a href="http://www.telemidia.puc-rio.br/~gflima/misc/ginga">Read the API documentation</a>.
* <a href="https://github.com/TeleMidia/ginga/issues">Find and file bug reports</a>.

---
Copyright (C) 2006-2019 PUC-Rio/Laboratorio TeleMidia

Permission is granted to copy, distribute and/or modify this document under
the terms of the GNU Free Documentation License, Version 1.3 or any later
version published by the Free Software Foundation; with no Invariant
Sections, with no Front-Cover Texts, and with no Back-Cover Texts. A copy of
the license is included in the "GNU Free Documentation License" file as part
of this distribution.
