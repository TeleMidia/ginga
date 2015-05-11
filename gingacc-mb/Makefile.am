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
  libgingaccmbsdl.la\
  libgingaccmbdecsdlap.la\
  libgingaccmbdecsdlimg.la\
  libgingaccmbdecsdlf.la\
  libgingaccmbdecsdlav.la\
  $(NULL)

# gingacc-mb
libgingaccmb_la_CXXFLAGS = $(AM_CXXFLAGS) $(SDL2_CFLAGS) $(FFMPEG_CFLAGS)
libgingaccmb_la_LDFLAGS = $(AM_LDFLAGS) $(SDL2_LIBS) $(FFMPEG_LIBS)

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

# gingacc-mb-sdl (SDL)
libgingaccmbsdl_la_CXXFLAGS = $(AM_CXXFLAGS) $(SDL2_CFLAGS) $(X11_CFLAGS) $(FFMPEG_CFLAGS)
libgingaccmbsdl_la_LDFLAGS = $(AM_LDFLAGS) $(SDL2_LIBS) $(X11_LIBS) $(FFMPEG_LIBS) 

libgingaccmbsdl_la_SOURCES =\
  src/interface/sdl/SDLDeviceScreen.cpp\
  src/interface/sdl/input/SDLEventBuffer.cpp\
  src/interface/sdl/input/SDLInputEvent.cpp\
  src/interface/sdl/output/SDLSurface.cpp\
  src/interface/sdl/output/SDLWindow.cpp\
  src/interface/sdl/content/SDLConvert.cpp\
  $(NULL)

# gingacc-mb-dec-sdl-ap (SDL Audio)
libgingaccmbdecsdlap_la_CXXFLAGS =\
  $(AM_CXXFLAGS) $(SDL2_CFLAGS) $(FFMPEG_CFLAGS)

libgingaccmbdecsdlap_la_LDFLAGS =\
  $(AM_LDFLAGS) $(SDL2_LIBS) $(FFMPEG_LIBS)

libgingaccmbdecsdlap_la_SOURCES =\
  src/interface/sdl/content/audio/SDLAudioProvider.cpp\
  src/interface/sdl/content/audio/SDL2ffmpeg.cpp\
  $(NULL)

# gingacc-mb-dec-sdl-img (SDL Image)
libgingaccmbdecsdlimg_la_CXXFLAGS =\
  $(AM_CXXFLAGS) $(SDL2_CFLAGS) $(SDL2IMAGE_CFLAGS)
if BUILD_SVG
libgingaccmbdecsdlimg_la_CXXFLAGS += $(LIBRSVG_CFLAGS)
endif

libgingaccmbdecsdlimg_la_LDFLAGS =\
  $(AM_LDFLAGS) $(SDL2_LIBS) $(SDL2IMAGE_LIBS)
if BUILD_SVG
libgingaccmbdecsdlimg_la_LDFLAGS += $(LIBRSVG_LIBS)
endif

libgingaccmbdecsdlimg_la_SOURCES =\
  src/interface/sdl/content/image/SDLImageProvider.cpp\
  $(NULL)

if BUILD_SVG
libgingaccmbdecsdlimg_la_SOURCES += src/interface/sdl/content/image/SDLSvgDecoder.cpp
endif

# gingacc-mb-dec-sdl-f (SDL Text)
libgingaccmbdecsdlf_la_CXXFLAGS =\
  $(AM_CXXFLAGS) $(SDL2_CFLAGS) $(SDL2TTF_CFLAGS)

libgingaccmbdecsdlf_la_LDFLAGS =\
  $(AM_LDFLAGS) $(SDL2_LIBS) $(SDL2TTF_LIBS)

libgingaccmbdecsdlf_la_SOURCES =\
  src/interface/sdl/content/text/SDLFontProvider.cpp\
  $(NULL)

# gingacc-mb-dec-sdl-av (SDL Audio/Video)
libgingaccmbdecsdlav_la_CXXFLAGS =\
  $(AM_CXXFLAGS) $(SDL2_CFLAGS) $(FFMPEG_CFLAGS)

libgingaccmbdecsdlav_la_LDFLAGS =\
  $(AM_LDFLAGS) $(SDL2_LIBS) $(FFMPEG_LIBS)

libgingaccmbdecsdlav_la_LIBADD =\
  libgingaccmbdecsdlap.la\
  $(NULL)

libgingaccmbdecsdlav_la_SOURCES =\
  src/interface/sdl/content/video/SDLVideoProvider.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
gingaccmbinclude_HEADERS =\
  include/mb/IInputManager.h\
  include/mb/ILocalScreenManager.h\
  include/mb/ScreenManagerFactory.h\
  include/mb/IMBDefs.h\
  include/mb/LocalScreenManager.h\
  $(NULL)

interfaceincludedir = $(gingaccmbincludedir)/interface
interfaceinclude_HEADERS =\
  include/mb/interface/CodeMap.h\
  include/mb/interface/ICmdEventListener.h\
  include/mb/interface/IContinuousMediaProvider.h\
  include/mb/interface/IDataBuffer.h\
  include/mb/interface/IDeviceScreen.h\
  include/mb/interface/IDiscreteMediaProvider.h\
  include/mb/interface/IEventBuffer.h\
  include/mb/interface/IFontProvider.h\
  include/mb/interface/IImageProvider.h\
  include/mb/interface/IInputEvent.h\
  include/mb/interface/IInputEventListener.h\
  include/mb/interface/IMediaProvider.h\
  include/mb/interface/IMotionEventListener.h\
  include/mb/interface/IProviderListener.h\
  include/mb/interface/ISurface.h\
  include/mb/interface/IWindow.h\
  $(NULL)
endif

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
  ../gingacc-mb/libgingaccmbdecsdlap.la\
  ../gingacc-mb/libgingaccmbdecsdlav.la\
  ../gingacc-mb/libgingaccmbdecsdlf.la\
  ../gingacc-mb/libgingaccmbdecsdlimg.la\
  ../gingacc-mb/libgingaccmbsdl.la\
  ../telemidia-util/libtelemidiautil.la\
  $(NULL)	
endif
