## Build on Windows

### Dependecies

First download and install [MSYS2](http://www.msys2.org).

To install the dependencies, run on msys bash:

```bash
  pacman -Syu --noconfirm
  pacman -Su --needed --noconfirm autoconf automake libtool pkgconf make mingw-w64-x86_64-gcc mingw64/mingw-w64-x86_64-cmake mingw64/mingw-w64-x86_64-ninja mingw64/mingw-w64-x86_64-glib2 mingw64/mingw-w64-x86_64-libsoup mingw64/mingw-w64-x86_64-gtk3 mingw64/mingw-w64-x86_64-gdk-pixbuf2 mingw64/mingw-w64-x86_64-cairo mingw64/mingw-w64-x86_64-pango mingw64/mingw-w64-x86_64-gst-libav mingw64/mingw-w64-x86_64-gst-plugins-base mingw64/mingw-w64-x86_64-gst-plugins-good mingw64/mingw-w64-x86_64-gst-plugins-bad mingw-w64-x86_64-gst-plugins-ugly mingw-w64-x86_64-meson
  # pacman -R mingw-w64-x86_64-lua install to 5.4 but we require 5.3
  wget -P /tmp/ https://repo.msys2.org/mingw/x86_64/mingw-w64-x86_64-lua-5.3.5-1-any.pkg.tar.xz
  pacman --noconfirm -U  /tmp/mingw-w64-x86_64-lua-5.3.5-1-any.pkg.tar.xz
```

### Build

To build nclua, gssdp and ginga, run on msys bash:

```bash
  git clone https://github.com/TeleMidia/nclua.git
  cd nclua
  ./bootstrap
  ./configure
  make
  make install
```

> When running `./configure`, if you get the error that C compilers weren't found, try restarting MSYS2 as 64 bits (can be found in the same folder as 32 bits MSYS2). Reference: https://stackoverflow.com/questions/64078800/gcc-not-added-to-system-path-on-pacman-install

```bash
  git clone -b gssdp-1.4.0 https://gitlab.gnome.org/GNOME/gssdp.git
  cd gssdp
  mkdir build && cd build
  meson .. --prefix=/mingw64 -Dintrospection=false -Dsniffer=false -Dexamples=false -Dvapi=false
  ninja
  ninja install
```

```bash
  cd ginga
  mkdir build && cd build
  cmake .. -G Ninja -DSTATIC_LINKING=OFF -DBUILD_SHARED_LIBS=ON
  ninja 
  ninja install
```

### Debug on VSCode

```bash
  pacman -Su --needed --noconfirm mingw64/mingw-w64-x86_64-gdb
```

Install the [ms-vscode.cpptools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) and [ms-vscode.cmake-tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions, also see the [debug-lanch guide](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/debug-launch.md).

Known issues:

* Make sure msys dirs are in path to enable vscode find it and its run/debug use dependencies libs.

  ```powershell
  [Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\msys64\usr\bin" + ";C:\msys64\\mingw64\bin", "user")
  ```

* Debug fail if the "terminal.integrated.automationProfile.windows" is diffrent than cmd.

To run unit tests on VSCode, install [hbenl.vscode-test-explorer](https://marketplace.visualstudio.com/items?itemName=hbenl.vscode-test-explorer) and [fredericbonnet.cmake-test-adapter](https://marketplace.visualstudio.com/items?itemName=fredericbonnet.cmake-test-adapter) extensions, also see the its [guide](https://github.com/fredericbonnet/cmake-test-explorer).

### Build installer

```bash
  cd ginga _build
  cmake .. -G -DSTATIC_LINKING=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
  ninja 
  cpack
```

To deploy installer on github, run:

```bash
  hub release edit v1.0 -m "" -a ginga-1.0-win64.exe
```
