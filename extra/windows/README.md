## Building the repository on Windows

Download [MSYS2](www.msys2.org).

To install the dependencies run on MSYS terminal:

    $ pacman -Syu --noconfirm

    $ pacman -Sy --noconfirm base-devel mpfr cmake mingw64/mingw-w64-x86_64-glib2 glib2-devel \
        mingw64/mingw-w64-x86_64-cairo mingw64/mingw-w64-x86_64-gst-plugins-base \
        mingw64/mingw-w64-x86_64-gst-plugins-good mingw64/mingw-w64-x86_64-gst-plugins-bad \
        mingw64/mingw-w64-x86_64-gtk3 mingw64/mingw-w64-x86_64-pango mingw-w64-x86_64-lua \
        mingw64/mingw-w64-x86_64-gst-libav mingw-w64-x86_64-gcc mingw-w64-x86_64-make \
        mingw64/mingw-w64-x86_64-libsoup mingw64/mingw-w64-x86_64-x264-git

Build with autotools.

    $ ./bootstrap
    $ ./configure
    $ make install

## Create installer on Windows

Download rcedit (<github.com/electron/rcedit/releases>) to extra/windows folder.
Download Inno Setup (<www.jrsoftware.org/isinfo.php>).

First, change ginga and gingagui icons using (run terminal with admin privileges):

    $ extra/windows/rcedit-x64.exe /mingw64/bin/ginga.exe --set-icon extra/windows/icon.ico
    $ extra/windows/rcedit-x64.exe /mingw64/bin/gingagui.exe --set-icon extra/windows/icon.ico

Run `ginga_win_deploy_script.iss` with Inno Setup.
- If your Windows drive is not C:\\ the script must be modified
  accordingly.

Press the green play arrow to start building. At the end of the build process the program setup is automatically started.
- The output file is located on the same directory as the script is, but a directory named Output in created.

