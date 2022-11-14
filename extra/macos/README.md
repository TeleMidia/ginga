## Building on macOS

Download [brew](<http://brew.sh>).

To install the dependencies run on macOS terminal:

```bash
    $ brew install bash gnu-sed
    $ brew install git gnu-sed autoconf automake libtool pkg-config gettext cmake \
        glib pango gsettings-desktop-schemas libogg cairo libpng gst-libav \
        gst-plugins-bad librsvg python faac gst-plugins-base python3 \
        gst-plugins-good gst-plugins-ugly freetype gstreamer webp gtk+3 \
        libvorbis libvpx x264 lua lame libsoup libsoup icu4c libpsl 
```

To build with autotools run on bash shell:

```bash
    export PATH="/usr/local/opt/gnu-sed/libexec/gnubin:$PATH"
    export PKG_CONFIG_PATH="$(echo /usr/local/Cellar/glib/**/lib/pkgconfig/):$(echo /usr/local/Cellar/gtk+3/**/lib/pkgconfig/):$(echo /usr//local/Cellar/icu4c/**/lib/pkgconfig/)" 
    ./bootstrap
    ./configure
    make
```

### Build installer

Execute `osx_bundle_gen.py` with sudo to generate Ginga.app bundle:

```bash
    sudo python3 osx_bundle_gen.py
```

Use MacOS`s Packages:

1. In payload menu, select Applications folder, click in +, and select the Ginga.app
2. In script menu, select 'Pos Installation' script and select 'pkg_pos_install.sh' script in this folder