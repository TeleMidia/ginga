## Building the repository on macOS

Download [brew](<brew.sh>).

To install the dependencies run on macOS terminal:

    $ brew install  git autoconf automake libtool pkg-config gettext cmake \
        glib pango gsettings-desktop-schemas libogg cairo libpng gst-libav \
        gst-plugins-bad librsvg python faac gst-plugins-base libsoup python3 \
        gst-plugins-good ffmpeg gst-plugins-ugly freetype gstreamer webp gtk+3 \
        libvorbis  libvpx x264 lua lame libsoup

To build with autotools run:

    $ ./bootstrap
    $ ./configure
    $ make

## Deploy on macOS

Execute `osx_bundle_gen.py` with sudo to generate Ginga.app bundle:

    $ sudo python3 osx_bundle_gen.py

Use MacOS`s Packages:

1. In payload menu, select Applications folder, click in +, and select the Ginga.app
2. In script menu, select 'Pos Installation' script and select 'pkg_pos_install.sh' script in this folder