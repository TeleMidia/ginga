include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
	-std=c++11 \
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS) $(JPEG_LIBS)

gingaccmblib_LTLIBRARIES =\
  libgingaccmb.la\
  $(NULL)

# gingacc-mb
libgingaccmb_la_CXXFLAGS = $(AM_CXXFLAGS) $(SDL2_CFLAGS) $(FFMPEG_CFLAGS) $(X11_CFLAGS) $(SDL2IMAGE_CFLAGS) $(SDL2TTF_CFLAGS)
libgingaccmb_la_LDFLAGS = $(AM_LDFLAGS) $(SDL2_LIBS) $(FFMPEG_LIBS) $(SDL2IMAGE_LIBS) $(SDL2TTF_LIBS)

libgingaccmb_la_SOURCES =\
  src/ScreenManagerFactory.cpp\
  src/interface/CodeMap.cpp\
  src/interface/InputEventFactory.cpp\
  src/interface/Matrix.cpp\
  src/InputManager.cpp\
  src/LocalScreenManager.cpp

if BUILD_REMOTE_MB
libgingaccmb_la_SOURCES +=\
	src/DistributedInputManagerMB.cpp\
	src/DistributedInputManagerListener.cpp\
	src/remote/StubClient.cpp\
	src/DistributedScreenManager.cpp


libgingaccmb_la_LDFLAGS += -lboost_system
endif

libgingaccmb_la_SOURCES +=\
	$(NULL)	

libgingaccmb_la_SOURCES +=\
  src/interface/sdl/SDLDeviceScreen.cpp\
  src/interface/sdl/input/SDLEventBuffer.cpp\
  src/interface/sdl/input/SDLInputEvent.cpp\
  src/interface/sdl/output/SDLSurface.cpp\
  src/interface/sdl/output/SDLWindow.cpp\
  src/interface/sdl/content/SDLConvert.cpp\
  $(NULL)

libgingaccmb_la_SOURCES +=\
  src/interface/sdl/content/audio/SDLAudioProvider.cpp\
  src/interface/sdl/content/audio/SDL2ffmpeg.cpp\
  $(NULL)

if BUILD_SVG
libgingaccmb_la_CXXFLAGS += $(LIBRSVG_CFLAGS)
endif

if BUILD_LIBBPG
libgingaccmb_la_CXXFLAGS += $(LIBBPG_CFLAGS)
endif

if BUILD_SVG
libgingaccmb_la_LDFLAGS += $(LIBRSVG_LIBS)
endif
if BUILD_LIBBPG
libgingaccmb_la_LDFLAGS += $(LIBBPG_LIBS)
endif

libgingaccmb_la_SOURCES +=\
  src/interface/sdl/content/image/SDLImageProvider.cpp\
  $(NULL)
if BUILD_SVG
libgingaccmb_la_SOURCES += src/interface/sdl/content/image/SDLSvgDecoder.cpp
endif

if BUILD_LIBBPG
libgingaccmb_la_SOURCES += src/interface/sdl/content/image/SDLBpgDecoder.cpp
endif

libgingaccmb_la_SOURCES +=\
  src/interface/sdl/content/text/SDLFontProvider.cpp\
  $(NULL)

libgingaccmb_la_SOURCES +=\
  src/interface/sdl/content/video/SDLVideoProvider.cpp\
  $(NULL)

############################## Configuration ###############################
gingaccmbconfdir = $(gingaconfdir)/mb/config
gingaccmbconf_DATA =\
  files/config/input.cfg\
  $(NULL)

############################## Ginga-mb PROGRAMS ###########################
if BUILD_REMOTE_MB
bin_PROGRAMS = gingacc_mb
gingacc_mb_SOURCES =\
   src/remote/RequestManager.cpp\
   src/remote/StubServer.cpp\
   src/main.cpp\
   $(NULL)

gingacc_mb_CPPFLAGS =\
  -std=c++11 \
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_CONTEXTMANAGER)\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(INCLUDE_GINGACC_PLAYER)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_GINGA_NCL)\
  $(INCLUDE_GINGA_LSSM)\
  $(NULL)

gingacc_mb_LDADD = \
  ../gingacc-system/libgingaccsystem.la\
  ../gingacc-mb/libgingaccmb.la\
  ../telemidia-util/libtelemidiautil.la\
  $(NULL)	
endif
