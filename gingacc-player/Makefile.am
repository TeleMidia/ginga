include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_PLAYER)\
  $(NULL)

if BUILD_DATA_PROCESSING
AM_CPPFLAGS +=\
  $(INCLUDE_GINGACC_TUNER)\
  $(INCLUDE_GINGACC_TSPARSER)\
  $(INCLUDE_GINGACC_DATAPROCESSING)\
  $(NULL)
endif

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS)

gingaccplayerlib_LTLIBRARIES =\
  libgingaccplayer.la\
  $(NULL)

libgingaccplayer_la_CPPFLAGS = $(AM_CPPFLAGS)
libgingaccplayer_la_CXXFLAGS = $(AM_CXXFLAGS)
libgingaccplayer_la_LDFLAGS = $(AM_LDFLAGS)
libgingaccplayer_la_SOURCES = src/Player.cpp

libgingaccplayer_la_SOURCES +=\
  src/video/AVPlayer.cpp\
  $(NULL)

if BUILD_BERKELIUM
libgingaccplayer_la_CXXFLAGS += $(BERKELIUM_CFLAGS)
libgingaccplayer_la_LDFLAGS += $(BERKELIUM_LIBS)
libgingaccplayer_la_SOURCES +=\
  src/application/declarative/xhtml/berkelium/BerkeliumHandler.cpp\
  src/application/declarative/xhtml/berkelium/BerkeliumPlayer.cpp\
  $(NULL)
endif

libgingaccplayer_la_CXXFLAGS += $(NCLUA_CFLAGS) $(SDL2_CFLAGS)
libgingaccplayer_la_LDFLAGS += $(NCLUA_LIBS) $(SDL2_LIBS)
libgingaccplayer_la_SOURCES +=\
  src/application/imperative/lua/LuaPlayer.cpp\
  $(NULL)

libgingaccplayer_la_SOURCES +=\
  src/image/ImagePlayer.cpp\
  $(NULL)

libgingaccplayer_la_SOURCES +=\
  src/time/ntp/NTPDatagram.cpp\
  src/time/ntp/NTPPlayer.cpp\
  src/time/ntp/UDPClient.cpp\
  $(NULL)

libgingaccplayer_la_SOURCES +=\
  src/text/TextPlayer.cpp\
  $(NULL)

libgingaccplayer_la_SOURCES +=\
  src/text/plain/PlainTxtPlayer.cpp\
  $(NULL)

libgingaccplayer_la_SOURCES +=\
  src/text/srt/SrtPlayer.cpp\
  $(NULL)

libgingaccplayer_la_SOURCES +=\
  src/tv/ChannelPlayer.cpp\
  $(NULL)

libgingaccplayer_la_SOURCES +=\
  src/tv/ProgramAV.cpp\
  $(NULL)

libgingaccplayer_la_SOURCES +=\
  src/widget/ShowButton.cpp\
  $(NULL)
