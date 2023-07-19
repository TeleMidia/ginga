## Build on macOS

*We highlight that this build is not stable.*

### Dependencies

Download [brew](<http://brew.sh>).

To install the dependencies run on macOS terminal:

```bash
$ brew install bash gnu-sed
$ brew install git gnu-sed autoconf automake libtool pkg-config gettext cmake \
    glib pango gsettings-desktop-schemas libogg cairo libpng gst-libav \
    gst-plugins-bad librsvg python faac gst-plugins-base python3 \
    gst-plugins-good gst-plugins-ugly freetype gstreamer webp gtk+3 \
    libvorbis libvpx x264 lua lame libsoup libsoup icu4c libpsl meson
```

### Build

To build nclua, gssdp and ginga, run on msys bash:

```bash
git clone https://github.com/TeleMidia/nclua.git
cd nclua
./bootstrap && ./configure --without-nclua-gst --without-nclua-gtk
make
sudo make install
```

```bash
git clone -b gssdp-1.4.0 https://gitlab.gnome.org/GNOME/gssdp.git
cd gssdp
mkdir build && cd build
meson .. --prefix=/mingw64 -Dintrospection=false -Dsniffer=false -Dexamples=false -Dvapi=false
ninja all && ninja install
```

```bash
cd ginga
mkdir build && cd build
cmake .. -G Ninja -DSTATIC_LINKING=OFF -DBUILD_SHARED_LIBS=ON
ninja 
```

### Build installer

Execute `osx_bundle_gen.py` with sudo to generate Ginga.app bundle:

```bash
sudo python3 extra/macos/osx_bundle_gen.py
```

Use MacOS`s Packages:

1. In payload menu, select Applications folder, click in +, and select the Ginga.app
2. In script menu, select 'Pos Installation' script and select 'pkg_pos_install.sh' script in this folder