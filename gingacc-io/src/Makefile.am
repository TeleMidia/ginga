
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes)

# the library search path.
localLibdir = /usr/local/lib/ginga

libgingaccio_so_LDFLAGS = -Wl,-soname,libgingaccio.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga

localLib_PROGRAMS = libgingaccio.so

libgingaccio_so_SOURCES = interface/content/AudioProvider.cpp \
	interface/content/Contents.cpp \
	interface/content/FontProvider.cpp interface/content/ImageProvider.cpp \
	interface/content/VideoProvider.cpp interface/graphic/Graphics.cpp \
	interface/graphic/Surface.cpp interface/graphic/Window.cpp \
	interface/input/CodeMap.cpp interface/input/InputEvent.cpp \
	interface/input/InputEventBuffer.cpp interface/input/Inputs.cpp \
	interface/output/DeviceAudio.cpp interface/output/DeviceCommunication.cpp \
	interface/output/DeviceScreen.cpp interface/output/OutputDevice.cpp \
	GingaLocatorFactory.cpp InputManager.cpp IOHandler.cpp OutputManager.cpp

libgingaccio_so_LDADD = -ldirectfb -ltelemidiautil $(FUSION_LIBS)
