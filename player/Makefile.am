include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=\
  $(NULL)

if BUILD_DATA_PROCESSING
AM_CPPFLAGS+=\
  $(INCLUDE_GINGACC_DATAPROCESSING)\
  $(NULL)
endif

AM_CXXFLAGS= $(NCLUA_CFLAGS) $(SDL2_CFLAGS)
AM_LDFLAGS= $(LTSTATIC) $(NCLUA_LIBS) $(SDL2_LIBS)
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

if BUILD_BERKELIUM
AM_CXXFLAGS+= $(BERKELIUM_CFLAGS)
AM_LDFLAGS+= $(BERKELIUM_LIBS)
player_la_SOURCES+=\
  BerkeliumHandler.cpp\
  BerkeliumPlayer.cpp\
  $(NULL)
endif
