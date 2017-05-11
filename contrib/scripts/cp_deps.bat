::This is a batch script that copies the dlls
::of contrib folder that Ginga needs.

@echo off
pushd..
set baseDir=%cd%\
popd
set vsProjectDirName=vs2010
set cpCommand=xcopy /Y /Q

set contribDir=%~dp0
set gingaDir=%baseDir%%vsProjectDirName%\Ginga

if not exist %contribDir% (
   echo Missing %contribDir% directory!
   goto end
)

if not exist %gingaDir% mkdir %gingaDir%

::berkelium-win32
set berkeliumDir=%contribDir%\berkelium-win32\bin
echo Copying berkelium's dependencies.
%cpCommand%  %berkeliumDir%\berkelium.dll %gingaDir%
%cpCommand%  %berkeliumDir%\berkelium.exe %gingaDir%
%cpCommand%  %berkeliumDir%\freeglut.dll %gingaDir%
%cpCommand%  %berkeliumDir%\icudt46.dll %gingaDir%
%cpCommand%  %berkeliumDir%\libEGL.dll %gingaDir%
%cpCommand%  %berkeliumDir%\libGLESv2.dll %gingaDir%

::dlfcn
set dlfcnDir=%contribDir%\dlfcn-win32-static-r11\lib\x86
echo Copying dlfcn's dependencies.
%cpCommand%  %dlfcnDir%\dlfcn.dll %gingaDir%

::expat
set expatDir=%contribDir%\expat_win32bin_2_0_1\lib
echo Copying expat's dependencies.
%cpCommand%  %expatDir%\libexpat.dll %gingaDir%
%cpCommand%  %expatDir%\libexpatw.dll %gingaDir%

::ffmpeg
set ffmpegDir=%contribDir%\ffmpeg
echo Copying ffmpeg's dependencies.
%cpCommand%  %ffmpegDir%\avcodec-55.dll %gingaDir%
%cpCommand%  %ffmpegDir%\avdevice-55.dll %gingaDir%
%cpCommand%  %ffmpegDir%\avfilter-4.dll %gingaDir%
%cpCommand%  %ffmpegDir%\avformat-55.dll %gingaDir%
%cpCommand%  %ffmpegDir%\avutil-52.dll %gingaDir%
%cpCommand%  %ffmpegDir%\postproc-52.dll %gingaDir%
%cpCommand%  %ffmpegDir%\swresample-0.dll %gingaDir%
%cpCommand%  %ffmpegDir%\swscale-2.dll %gingaDir%

::FreeImage
set freeImageDir=%contribDir%\FreeImage\Dist
echo Copying FreeImage's dependencies.
%cpCommand%  %freeImageDir%\FreeImage.dll %gingaDir%

::libcurl
set libcurlDir=%contribDir%\libcurl
echo Copying libcurl's dependencies.
%cpCommand%  %libcurlDir%\libcurl.dll %gingaDir%
%cpCommand%  %libcurlDir%\curl.exe %gingaDir%

::libzip
set libzipDir=%contribDir%\libzip\bin
echo Copying libzip's dependencies.
%cpCommand%  %libzipDir%\zip.dll %gingaDir%
%cpCommand%  %libzipDir%\libzip-2.dll %gingaDir%

::nclua
set ncluaDir=%contribDir%\nclua
echo Copying nclua's dependencies.
%cpCommand%  %ncluaDir%\nclua\bin\libnclua-0.dll %gingaDir%
%cpCommand%  %ncluaDir%\lua-5.2\bin\lua52.dll %gingaDir%

if not exist %gingaDir%\nclua mkdir %gingaDir%\nclua
%cpCommand%  %ncluaDir%\nclua\lib\lua\5.2\nclua\canvas.dll %gingaDir%\nclua

if not exist %gingaDir%\nclua\event mkdir %gingaDir%\nclua\event
%cpCommand%  %ncluaDir%\nclua\lib\lua\5.2\nclua\event\tcp_socket.dll %gingaDir%\nclua\event
%cpCommand%  %ncluaDir%\nclua\lib\lua\5.2\nclua\event\stopwatch.dll %gingaDir%\nclua\event

::gtk
set gtkDir=%ncluaDir%\gtk+-3.6.4\bin
echo Copying gtk's dependencies.
%cpCommand%  %gtkDir%\libcairo-2.dll %gingaDir%
%cpCommand%  %gtkDir%\libcairo-gobject-2.dll %gingaDir%
%cpCommand%  %gtkDir%\libffi-6.dll %gingaDir%
%cpCommand%  %gtkDir%\libfontconfig-1.dll %gingaDir%
%cpCommand%  %gtkDir%\libgdk_pixbuf-2.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libgdk-3-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libgio-2.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libglib-2.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libgmodule-2.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libgobject-2.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libiconv-2.dll %gingaDir%
%cpCommand%  %gtkDir%\libintl-8.dll %gingaDir%
%cpCommand%  %gtkDir%\libjpeg-9.dll %gingaDir%
%cpCommand%  %gtkDir%\liblzma-5.dll %gingaDir%
%cpCommand%  %gtkDir%\libpango-1.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libpangocairo-1.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libpangoft2-1.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libpangowin32-1.0-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libpixman-1-0.dll %gingaDir%
%cpCommand%  %gtkDir%\libpng15-15.dll %gingaDir%
%cpCommand%  %gtkDir%\libtiff-5.dll %gingaDir%
%cpCommand%  %gtkDir%\libxml2-2.dll %gingaDir%

::pthread
set pthreadDir=%contribDir%\pthread\lib
echo Copying pthread's dependencies.
%cpCommand%  %pthreadDir%\pthreadGC2.dll %gingaDir%
%cpCommand%  %pthreadDir%\pthreadGCE2.dll %gingaDir%
%cpCommand%  %pthreadDir%\pthreadVC2.dll %gingaDir%
%cpCommand%  %pthreadDir%\pthreadVCE2.dll %gingaDir%
%cpCommand%  %pthreadDir%\pthreadVSE2.dll %gingaDir%

::sdl
set sdlDir=%contribDir%\sdl2.0
echo Copying sdl's dependencies.
%cpCommand%  %sdlDir%\libfreetype-6.dll %gingaDir%
%cpCommand%  %sdlDir%\libpng16-16.dll %gingaDir%
%cpCommand%  %sdlDir%\libwebp-4.dll %gingaDir%
%cpCommand%  %sdlDir%\SDL2.dll %gingaDir%
%cpCommand%  %sdlDir%\SDL2_image.dll %gingaDir%
%cpCommand%  %sdlDir%\SDL2_ttf.dll %gingaDir%
::%cpCommand%  %sdlDir%\zlib1.dll %gingaDir%

::xerces
set xercesDir=%contribDir%\xerces-c-3.0.1-x86-windows-vc-9.0\bin
echo Copying xerces's dependencies.
%cpCommand%  %xercesDir%\xerces-c_3_0.dll %gingaDir%

::zlib
set xercesDir=%contribDir%\zlib-1.2.8-bin\lib
echo Copying zlib's dependencies.
%cpCommand%  %xercesDir%\zlib1.dll %gingaDir%

:end
pause
