include $(top_srcdir)/build-aux/Makefile.am.common

AM_CXXFLAGS= $(FFMPEG_CFLAGS) $(NCLUA_CFLAGS) $(SDL2_CFLAGS)
AM_LDFLAGS= $(LTSTATIC) $(FFMPEG_LIBS) $(NCLUA_LIBS) $(SDL2_LIBS)
noinst_LTLIBRARIES= player.la
player_la_SOURCES=\
  AVPlayer.cpp\
  ChannelPlayer.cpp\
  ImagePlayer.cpp\
  LuaPlayer.cpp\
  NTPDatagram.cpp\
  NTPPlayer.cpp\
  PlainTxtPlayer.cpp\
  Player.cpp\
  ProgramAV.cpp\
  ShowButton.cpp\
  SrtPlayer.cpp\
  TextPlayer.cpp\
  UDPClient.cpp\
  $(NULL)

if BUILD_SSML
AM_CXXFLAGS+= $(ESPEAK_CFLAGS)
AM_LDFLAGS+= $(ESPEAK_LIBS)
player_la_SOURCES+= SsmlPlayer.cpp
endif

if BUILD_BERKELIUM
AM_CXXFLAGS+= $(BERKELIUM_CFLAGS)
AM_LDFLAGS+= $(BERKELIUM_LIBS)
player_la_SOURCES+=\
  BerkeliumHandler.cpp\
  BerkeliumPlayer.cpp\
  $(NULL)
endif
