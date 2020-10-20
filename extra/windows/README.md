## Building the repository on Windows

Download and install [MSYS2](<http://www.msys2.org>).

To install the dependencies, run on MSYS terminal:

``` bash
  pacman -Syu --noconfirm
  pacman -Sy --noconfirm mingw64/mingw-w64-x86_64-x264-git base-devel mpfr cmake mingw64/mingw-w64-x86_64-glib2 glib2-devel mingw64/mingw-w64-x86_64-cairo mingw64/mingw-w64-x86_64-gst-plugins-base mingw64/mingw-w64-x86_64-gst-plugins-good mingw64/mingw-w64-x86_64-gst-plugins-bad mingw64/mingw-w64-x86_64-gtk3 mingw64/mingw-w64-x86_64-pango mingw-w64-x86_64-lua mingw64/mingw-w64-x86_64-gst-libav mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw64/mingw-w64-x86_64-libsoup mingw-w64-x86_64-gst-plugins-ugly
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
