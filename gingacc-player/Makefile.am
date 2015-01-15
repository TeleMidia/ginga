include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_IC)\
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
  libgingaccplaavp.la\
  libgingaccplacha.la\
  libgingaccplaimg.la\
  libgingaccplalua.la\
  libgingaccplapav.la\
  libgingaccplaplt.la\
  libgingaccplasrt.la\
  libgingaccplatxt.la\
  libgingaccplatim.la\
  libgingaccplawdg.la\
  $(NULL)

# gingacc-player
libgingaccplayer_la_SOURCES = src/Player.cpp
if BUILD_MULTIPROCESS
libgingaccplayer_la_SOURCES += src/PlayerProcess.cpp
endif

# gingacc-pla-avp (Audio and video)
libgingaccplaavp_la_SOURCES =\
  src/video/AVPlayer.cpp\
  $(NULL)

# gingacc-pla-bkl
if BUILD_BERKELIUM
gingaccplayerlib_LTLIBRARIES += libgingaccplabkl.la
libgingaccplabkl_la_CXXFLAGS = $(AM_CXXFLAGS) $(BERKELIUM_CFLAGS)
libgingaccplabkl_la_LDFLAGS = $(AM_LDFLAGS) $(BERKELIUM_LIBS)
libgingaccplabkl_la_SOURCES =\
  src/application/declarative/xhtml/berkelium/BerkeliumHandler.cpp\
  src/application/declarative/xhtml/berkelium/BerkeliumPlayer.cpp\
  $(NULL)
endif

# gingacc-pla-lua
libgingaccplalua_la_CXXFLAGS = $(AM_CXXFLAGS) -Wall\
  $(NCLUA_CFLAGS) $(SDL2_CFLAGS)
libgingaccplalua_la_LDFLAGS = $(NCLUA_LIBS) $(SDL2_LIBS)
libgingaccplalua_la_SOURCES =\
  src/application/imperative/lua/LuaPlayer.cpp\
  $(NULL)

# gingacc-pla-img (Image)
libgingaccplaimg_la_SOURCES =\
  src/image/ImagePlayer.cpp\
  $(NULL)

# gingacc-pla-tim (Time - Network Time Protocol)
libgingaccplatim_la_SOURCES =\
  src/time/ntp/NTPDatagram.cpp\
  src/time/ntp/NTPPlayer.cpp\
  src/time/ntp/UDPClient.cpp\
  $(NULL)

# gingacc-pla-txt (Text)
libgingaccplatxt_la_SOURCES =\
  src/text/TextPlayer.cpp\
  $(NULL)

# gingacc-pla-plt (Plain text)
libgingaccplaplt_la_SOURCES =\
  src/text/plain/PlainTxtPlayer.cpp\
  $(NULL)

# gingacc-pla-srt (SRT)
libgingaccplasrt_la_SOURCES =\
  src/text/srt/SrtPlayer.cpp\
  $(NULL)

# gingacc-pla-cha
libgingaccplacha_la_LIBADD =\
  libgingaccplaavp.la\
  $(NULL)

libgingaccplacha_la_SOURCES =\
  src/tv/ChannelPlayer.cpp\
  $(NULL)

# gingacc-pla-pav (?)
libgingaccplapav_la_LIBADD =\
  libgingaccplaavp.la\
  $(NULL)

libgingaccplapav_la_SOURCES =\
  src/tv/ProgramAV.cpp\
  $(NULL)

# gingacc-pla-wdg (Widget)
libgingaccplawdg_la_SOURCES =\
  src/widget/ShowButton.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
gingaccplayerinclude_HEADERS =\
  include/player/IApplicationPlayer.h\
  include/player/IPlayer.h\
  include/player/Player.h\
  include/player/INCLPlayer.h\
  include/player/IPlayerListener.h\
  include/player/IProgramAV.h\
  include/player/IShowButton.h\
  $(NULL)
endif

############################## Configuration ###############################
gingaccplayerconfdir = $(gingaconfdir)

buttondir = $(gingaccplayerconfdir)/img/button
button_DATA =\
  files/images/pauseButton.png\
  files/images/playButton.png\
  files/images/stopButton.png\
  $(NULL)

fontdir = $(gingaccplayerconfdir)/font
font_DATA =\
  files/fonts/decker.ttf\
  files/fonts/vera.ttf\
  $(NULL)
