include $(top_srcdir)/build-aux/Makefile.am.common

# button
buttondir= $(gingaconfdir)/img/button
button_DATA=\
  images/pauseButton.png\
  images/playButton.png\
  images/stopButton.png\
  $(NULL)

# contextmanager
contextmanagerdir= $(gingaconfdir)/contextmanager
contextmanager_DATA=\
  contextmanager/users.ini\
  contextmanager/contexts.ini\
  $(NULL)

# font
fontdir= $(gingaconfdir)/font
font_DATA=\
  fonts/decker.ttf\
  fonts/vera.ttf\
  $(NULL)

# formatter
formatterdir= $(gingaconfdir)/cfg/formatter
formatter_DATA=\
  formatter/ctrldefs.ini\
  formatter/mimedefs.ini\
  $(NULL)

formatterimgdir= $(gingaconfdir)/img/roller/
formatterimg_DATA=\
  formatterimg/loading.png\
  formatterimg/b1.png\
  formatterimg/b2.png\
  formatterimg/b3.png\
  formatterimg/b4.png\
  formatterimg/b6.png\
  formatterimg/b5.png\
  formatterimg/b7.png\
  formatterimg/b8.png\
  $(NULL)

formattermddir= $(gingaconfdir)/md/
formattermd_DATA=\
  formattermd/active-device-reset.png\
  formattermd/active-device.png\
  $(NULL)

# ini
if OS_WINDOWS
INTERNAL_DELIM= "\\\\"
FOREIGN_DELIM= "/"
INSTALL_PREFIX= "$(bindir)"
else
INTERNAL_DELIM= "/"
FOREIGN_DELIM= "\\\\"
INSTALL_PREFIX= "$(gingaconfdir)"
endif
FILES_PREFIX= "$(gingaconfdir)"

DISTCLEANFILES= ini/ginga.ini
ini/ginga.ini: ini/ginga.ini.in
	@$(SED)\
	  -e 's:@INTERNAL_DELIM@:$(INTERNAL_DELIM):'\
	  -e 's:@FOREIGN_DELIM@:$(FOREIGN_DELIM):'\
	  -e 's:@INSTALL_PREFIX@:$(INSTALL_PREFIX):'\
	  -e 's:@FILES_PREFIX@:$(FILES_PREFIX):' $< >$@

gingaconf_DATA=\
  ini/ginga.ini\
  ini/mimetypes.ini\
  $(NULL)

# mb
mbdir= $(gingaconfdir)/mb/config
mb_DATA=\
  mb/input.cfg\
  $(NULL)

# tuner
tunerdir= $(gingaconfdir)/tuner
tuner_DATA=\
  tuner/tuner.ini\
  tuner/isdbt.ini\
  tuner/tuning.png\
  $(NULL)
