## Building the repository on Windows

Download and install [MSYS2](<http://www.msys2.org>).

To install the dependencies, run on MSYS terminal:

``` bash
  pacman -Syu --noconfirm
  pacman -Su --needed --noconfirm autoconf automake1.6 libtool pkgconf make mingw-w64-x86_64-gcc mingw64/mingw-w64-x86_64-cmake mingw64/mingw-w64-x86_64-ninja mingw64/mingw-w64-x86_64-make mingw64/mingw-w64-x86_64-glib2 mingw64/mingw-w64-x86_64-libsoup mingw64/mingw-w64-x86_64-gtk3 mingw64/mingw-w64-x86_64-gdk-pixbuf2 mingw64/mingw-w64-x86_64-cairo mingw64/mingw-w64-x86_64-pango  mingw64/mingw-w64-x86_64-gst-libav mingw64/mingw-w64-x86_64-gst-plugins-base mingw64/mingw-w64-x86_64-gst-plugins-good mingw64/mingw-w64-x86_64-gst-plugins-bad mingw-w64-x86_64-gst-plugins-ugly
  # mingw-w64-x86_64-lua points to 5.4
  wget -P /tmp/ https://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-lua-5.3.5-1-any.pkg.tar.xz
  pacman -U /tmp/mingw-w64-x86_64-lua-5.3.5-1-any.pkg.tar.xz
```

To build with autotools, run:

``` bash
  ./bootstrap
  ./configure
  make install
```

To build installer, run:

``` bash
  cd extra/windows
  ./buidl-installer.sh
```
