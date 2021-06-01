#!/bin/bash
function ldd_deps() {
  echo $(ldd $@ | grep -v : | grep -v Windows |  cut -f2 | cut -d' ' -f1 | sort -u)
}

function create_folder() {
  if ! test -d $1; then mkdir -p $1; fi
}

echo "Coping /mingw64/bin deps ..."
GINGA_DEPS=""

# ginga deps
GINGA_DEPS+="/mingw64/bin/ginga.exe "
GINGA_DEPS+="/mingw64/bin/gingagui.exe "
GINGA_DEPS+="/mingw64/bin/libginga.dll "

# gnome deps
GINGA_DEPS+="/mingw64/bin/libcairo-2.dll "
GINGA_DEPS+="/mingw64/bin/libgdk-3-0.dll "
GINGA_DEPS+="/mingw64/bin/libgio-2.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libglib-2.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgobject-2.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libatk-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libbz2-1.dll "
GINGA_DEPS+="/mingw64/bin/libcairo-2.dll "
GINGA_DEPS+="/mingw64/bin/libcairo-gobject-2.dll "
# GINGA_DEPS+="/mingw64/bin/libcroco-0.6-3.dll "
GINGA_DEPS+="/mingw64/bin/libcurl-4.dll "
# GINGA_DEPS+="/mingw64/bin/libeay32.dll "
GINGA_DEPS+="/mingw64/bin/libepoxy-0.dll "
GINGA_DEPS+="/mingw64/bin/libexpat-1.dll "
GINGA_DEPS+="/mingw64/bin/libffi-7.dll "
GINGA_DEPS+="/mingw64/bin/libfontconfig-1.dll "
GINGA_DEPS+="/mingw64/bin/libfreetype-6.dll "
GINGA_DEPS+="/mingw64/bin/libgcc_s_seh-1.dll "
GINGA_DEPS+="/mingw64/bin/libgdk_pixbuf-2.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgdk-3-0.dll "
GINGA_DEPS+="/mingw64/bin/libgio-2.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libglib-2.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgmodule-2.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgobject-2.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgraphite2.dll "
GINGA_DEPS+="/mingw64/bin/libgstapp-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstbase-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstreamer-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstvideo-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgtk-3-0.dll "
GINGA_DEPS+="/mingw64/bin/libharfbuzz-0.dll "
GINGA_DEPS+="/mingw64/bin/libiconv-2.dll "
# GINGA_DEPS+="/mingw64/bin/libicudt67.dll "
# GINGA_DEPS+="/mingw64/bin/libicuuc67.dll "
GINGA_DEPS+="/mingw64/bin/libidn2-0.dll "
GINGA_DEPS+="/mingw64/bin/libintl-8.dll "
GINGA_DEPS+="/mingw64/bin/liblzma-5.dll "
GINGA_DEPS+="/mingw64/bin/libnclua-0.dll "
GINGA_DEPS+="/mingw64/bin/libnghttp2-14.dll "
GINGA_DEPS+="/mingw64/bin/liborc-0.4-0.dll "
GINGA_DEPS+="/mingw64/bin/libpango-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libpangocairo-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libpangoft2-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libpangowin32-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libpcre-1.dll "
GINGA_DEPS+="/mingw64/bin/libpixman-1-0.dll "
GINGA_DEPS+="/mingw64/bin/libpng16-16.dll "
GINGA_DEPS+="/mingw64/bin/librsvg-2-2.dll "
GINGA_DEPS+="/mingw64/bin/libssp-0.dll "
GINGA_DEPS+="/mingw64/bin/libpsl-5.dll "
GINGA_DEPS+="/mingw64/bin/libthai-0.dll "
GINGA_DEPS+="/mingw64/bin/libdatrie-1.dll "
GINGA_DEPS+="/mingw64/bin/libbrotlidec.dll "
GINGA_DEPS+="/mingw64/bin/libbrotlicommon.dll "
GINGA_DEPS+="/mingw64/bin/libstdc++-6.dll "
GINGA_DEPS+="/mingw64/bin/libunistring-2.dll "
GINGA_DEPS+="/mingw64/bin/libwinpthread-1.dll "
GINGA_DEPS+="/mingw64/bin/libxml2-2.dll "
GINGA_DEPS+="/mingw64/bin/lua53.dll "
GINGA_DEPS+="/mingw64/bin/zlib1.dll "

# av deps
GINGA_DEPS+="/mingw64/bin/avcodec-58.dll "
GINGA_DEPS+="/mingw64/bin/avfilter-7.dll "
GINGA_DEPS+="/mingw64/bin/avformat-58.dll "
GINGA_DEPS+="/mingw64/bin/avutil-56.dll "
GINGA_DEPS+="/mingw64/bin/libfaac-0.dll "
GINGA_DEPS+="/mingw64/bin/libfaad-2.dll "
GINGA_DEPS+="/mingw64/bin/libFLAC-8.dll "
GINGA_DEPS+="/mingw64/bin/libmpg123-0.dll "
GINGA_DEPS+="/mingw64/bin/libmpg123-0.dll "
GINGA_DEPS+="/mingw64/bin/libmpeg2-0.dll "
GINGA_DEPS+="/mingw64/bin/libnice-10.dll "
GINGA_DEPS+="/mingw64/bin/libogg-0.dll "
GINGA_DEPS+="/mingw64/bin/libtheora-0.dll "
GINGA_DEPS+="/mingw64/bin/libvpx-1.dll "
GINGA_DEPS+="/mingw64/bin/libcurl-4.dll "
GINGA_DEPS+="/mingw64/bin/libjpeg-8.dll "
GINGA_DEPS+="/mingw64/bin/libvorbisidec-1.dll "

# gst deps
GINGA_DEPS+="/mingw64/bin/gst-play-1.0.exe "
GINGA_DEPS+="/mingw64/bin/libgstadaptivedemux-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstapp-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstbase-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstaudio-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstvideo-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstreamer-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstbadaudio-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstbasecamerabinsrc-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstcodecparsers-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstcontroller-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstfft-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstgl-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstmpegts-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstnet-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstpbutils-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstriff-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstrtp-1.0-0.dll " # essential
GINGA_DEPS+="/mingw64/bin/libgstrtsp-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgstsdp-1.0-0.dll "
GINGA_DEPS+="/mingw64/bin/libgsttag-1.0-0.dll " # essential

create_folder ./ginga/
for i in $GINGA_DEPS; do
  cp -u $i ./ginga/
done

LDD_DEPS="$(ldd_deps "$GINGA_DEPS")"
LDD_DEPS_EXCPTS="wslbscrwh64.dll wslbscr64.dll "
LDD_DEPS_FINAL=$(comm -23 <(tr ' ' $'\n' <<< $LDD_DEPS | sort) <(tr ' ' $'\n' <<< $LDD_DEPS_EXCPTS | sort))

for i in $LDD_DEPS_FINAL; do 
  cp /mingw64/bin/$i ./ginga/
done

# ginga-gui/GTK icons
echo "Coping ginga-gui icons ..."

create_folder ./ginga/share/ginga/
cp -r -u /mingw64/share/ginga/* ./ginga/share/ginga/

create_folder ./ginga/share/icons/Adwaita/16x16/
cp -r -u /mingw64/share/icons/Adwaita/16x16/* ./ginga/share/icons/Adwaita/16x16/

create_folder ./ginga/share/icons/Adwaita/16x16/actions
cp -r -u /mingw64/share/icons/Adwaita/scalable/ui/* ./ginga/share/icons/Adwaita/16x16/actions

create_folder ./ginga/share/icons/hicolor/16x16/
cp -r -u /mingw64/share/icons/hicolor/16x16/* ./ginga/share/icons/hicolor/16x16/

create_folder ./ginga/share/glib-2.0/
cp -r -u /mingw64/share/glib-2.0/* ./ginga/share/glib-2.0/

create_folder ./ginga/lib/gdk-pixbuf-2.0
cp -r -u /mingw64/lib/gdk-pixbuf-2.0/* ./ginga/lib/gdk-pixbuf-2.0

if ! test -f rcedit-x64.exe; then mkdir -p $1; 
  wget https://github.com/electron/rcedit/releases/download/v1.1.1/rcedit-x64.exe -O ./rcedit-x64.exe
fi

./rcedit-x64.exe ginga/ginga.exe --set-icon ./icon.ico 
./rcedit-x64.exe ginga/gingagui.exe --set-icon ./icon.ico

# lib/gstreamer-1.0
echo "Coping mingw64/lib/gstreamer-1.0 deps ..."
create_folder ./ginga/lib/gstreamer-1.0

GST_PLUGINS=$(ls /mingw64/lib/gstreamer-1.0/)
GST_PLUGINS_EXCPTS="include/ libgsta52dec.dll  libgstcacasink.dll  libgstcamerabin.dll  libgstcdio.dll  libgstchromaprint.dll  libgstcolormanagement.dll  libgstd3d11.dll  libgstdash.dll  libgstdtsdec.dll  libgstdvdread.dll  libgstfdkaac.dll  libgstfluidsynthmidi.dll  libgstgme.dll  libgstmicrodns.dll  libgstnvcodec.dll  libgstopenal.dll  libgstopencv.dll  libgstopenexr.dll  libgstopengl.dll  libgstopenh264.dll  libgstresindvd.dll  libgstshout2.dll  libgstsmoothstreaming.dll  libgstsndfile.dll  libgstsoundtouch.dll  libgstsrtp.dll  libgsttaglib.dll  libgsttwolame.dll  libgstvoamrwbenc.dll  libgstvulkan.dll  libgstwebrtc.dll  libgstzbar.dll "
GST_PLUGINS_FINAL=$(comm -23 <(tr ' ' $'\n' <<< $GST_PLUGINS | sort) <(tr ' ' $'\n' <<< $GST_PLUGINS_EXCPTS | sort))

for i in $GST_PLUGINS_FINAL; do
  cp -r -u /mingw64/lib/gstreamer-1.0/$i ./ginga/lib/gstreamer-1.0/
done

# nclua deps
echo "Coping /mingw64/lib/lua deps ..."
create_folder ./lib/lua/
cp -r -u /mingw64/lib/lua/* ./lib/lua/
echo "Coping /mingw64/share/lua deps ..."
create_folder ./share/lua/
cp -r -u /mingw64/share/lua/* share/lua/

# run inno setup
iscc ./ginga.iss