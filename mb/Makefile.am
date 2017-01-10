include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(NULL)

AM_CXXFLAGS=\
  $(FFMPEG_CFLAGS)\
  $(JPEG_CFLAGS)\
  $(SDL2IMAGE_CFLAGS)\
  $(SDL2TTF_CFLAGS)\
  $(SDL2_CFLAGS)\
  $(X11_CFLAGS)\
  $(NULL)

AM_LDFLAGS=\
  $(LTSTATIC)\
  $(FFMPEG_LIBS)\
  $(JPEG_LIBS)\
  $(SDL2IMAGE_LIBS)\
  $(SDL2TTF_LIBS)\
  $(SDL2_LIBS)\
  $(X11_LIBS)\
  $(NULL)

noinst_LTLIBRARIES= mb.la
mb_la_SOURCES=\
  CodeMap.cpp\
  InputEventFactory.cpp\
  InputManager.cpp\
  LocalScreenManager.cpp\
  Matrix.cpp\
  SDL2ffmpeg.cpp\
  SDLAudioProvider.cpp\
  SDLConvert.cpp\
  SDLDeviceScreen.cpp\
  SDLEventBuffer.cpp\
  SDLFontProvider.cpp\
  SDLImageProvider.cpp\
  SDLInputEvent.cpp\
  SDLSurface.cpp\
  SDLVideoProvider.cpp\
  SDLWindow.cpp\
  ScreenManagerFactory.cpp\
  $(NULL)

if BUILD_SVG
AM_CXXFLAGS+= $(LIBRSVG_CFLAGS)
AM_LDFLAGS+= $(LIBRSVG_LIBS)
mb_la_SOURCES+= SDLSvgDecoder.cpp
endif

if BUILD_LIBBPG
AM_CXXFLAGS+= $(LIBBPG_CFLAGS)
AM_LDFLAGS+= $(LIBBPG_LIBS)
mb_la_SOURCES+= SDLBpgDecoder.cpp
endif
