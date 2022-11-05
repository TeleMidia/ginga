cmake_minimum_required(VERSION 3.2)
project(ginga)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PACKAGE "ginga")
set(VERSION 1.0)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build-cmake/CMakeModules)

# set all target runtime to bin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GINGA_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GINGA_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GINGA_BINARY_DIR})

# find dependencies
find_package(GLIB 2.32 REQUIRED COMPONENTS gobject gio gthread gmodule)
find_package(LibSoup 2.4 REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(GDK3 REQUIRED)
find_package(Cairo REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(Pango REQUIRED)
find_package(RSVG REQUIRED)
find_package(GStreamer 1.0 REQUIRED)
find_package(Lua 5.2 REQUIRED)
find_package(GTK3 REQUIRED)
pkg_check_modules(GSSDP gssdp-1.2 REQUIRED)
pkg_check_modules(JSON jsoncpp REQUIRED)

# optional dependencies
option(WITH_NCLUA_BUILD "Download and build nclua." OFF)
option(WITH_CEF "Build with chromium embedded support." OFF)
option(WITH_OPENGL "Build Ginga with opengl support." OFF)
option(WITH_GINGAQT "Build nclcomposer's ginga plugin." OFF)

if(WITH_OPENGL)
  find_package(SDL2)
  find_package(OpenGL)
  set(WITH_OPENGL ON)
else()
  set(WITH_OPENGL OFF) # openGL not found, turn it off
endif()

if(WITH_OPENGL)
  add_definitions(-DWITH_OPENGL=1)
endif()

if(WITH_NCLUA_BUILD)
  # nclua download/configure/build/install
  include(ExternalProject)
  ExternalProject_Add(nclua_build
    GIT_REPOSITORY "https://github.com/telemidia/nclua.git"
    GIT_TAG master
    UPDATE_COMMAND ""
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/nclua
    CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap &&
    <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
    --without-nclua-gst
    BUILD_COMMAND make
    INSTALL_COMMAND make install
  )
  ExternalProject_Get_Property(nclua_build install_dir)
  set(NCLUA_INCLUDE_DIRS "${install_dir}/include")
  set(NCLUA_LIBRARIES "${install_dir}/lib/libnclua.a")
  link_directories("${install_dir}/lib")
  add_definitions(-DWITH_NCLUA=1)
else()
  PKG_CHECK_MODULES(NCLUA nclua REQUIRED)
  add_definitions(-DWITH_NCLUA=1)
endif()

if(WITH_CEF)
  # CEF download/configure/build/install
  set(CEF_VERSION cef_binary_3.3029.1619.geeeb5d7_linux64_minimal)
  set(CEF_SHA1 f809ce65b0b532fcbba6cec5f0e60f731fd3cbbd)
  set(CEF_FILE ${CEF_VERSION}.tar.bz2)
  set(CEF_URL http://opensource.spotify.com/cefbuilds/${CEF_FILE})
  set(CEF_LOCAL_PATH ${CMAKE_BINARY_DIR}/3rdparty)
  set(CEF_LOCAL_FILE ${CMAKE_BINARY_DIR}/3rdparty/${CEF_FILE}.tar.gz)

  if(NOT EXISTS "${CEF_LOCAL_FILE}")
    message(STATUS "-- Downloading chromium embedded binary package...")
    file(DOWNLOAD ${CEF_URL} ${CEF_LOCAL_FILE}
      EXPECTED_HASH SHA1=${CEF_SHA1}
      SHOW_PROGRESS)

    message(STATUS "-- Extracting chromium embedded files...")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xzf ${CEF_LOCAL_FILE}
      WORKING_DIRECTORY ${CEF_LOCAL_PATH}
    )
  endif()

  set(CEF_ROOT "${CEF_LOCAL_PATH}/${CEF_VERSION}/")
  find_package(CEF REQUIRED)
  add_definitions(-DWITH_CEF=1)
  add_subdirectory(${CEF_ROOT}/libcef_dll libcef_dll_bin)

  # logical target used to link the libcef library.
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
  set(CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/")

else()
  add_definitions(-DWITH_CEF=0)
endif()

# libginga sources
set(LIBGINGA_SOURCES
  ./lib/ginga.h
  ./lib/aux-ginga.cpp
  ./lib/aux-gl.cpp
  ./lib/Composition.cpp
  ./lib/Context.cpp
  ./lib/Document.cpp
  ./lib/Event.cpp
  ./lib/Formatter.cpp
  ./lib/Ginga.cpp
  ./lib/Media.cpp
  ./lib/MediaSettings.cpp
  ./lib/Object.cpp
  ./lib/Parser.cpp
  ./lib/ParserLua.cpp
  ./lib/Predicate.cpp
  ./lib/Switch.cpp
  ./lib/Player.cpp
  ./lib/PlayerAnimator.cpp
  ./lib/PlayerImage.cpp
  ./lib/PlayerLua.cpp
  ./lib/PlayerSigGen.cpp
  ./lib/PlayerText.cpp
  ./lib/PlayerVideo.cpp
  ./lib/PlayerRemote.cpp
  ./lib/PlayerSvg.cpp
  ./lib/WebServices.cpp
)

configure_file(${CMAKE_SOURCE_DIR}/build-cmake/config.h.in ${CMAKE_BINARY_DIR}/config.h)

set(LIBGINGA_INCLUDE_DIRS
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
  ${GLIB_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR}
  ${GDKPIXBUF_INCLUDE_DIRS}
  ${GDK3_INCLUDE_DIRS}
  ${CAIRO_INCLUDE_DIRS}
  ${FONTCONFIG_INCLUDE_DIR}
  ${PANGO_INCLUDE_DIRS}
  ${RSVG_INCLUDE_DIRS}
  ${GSTREAMER_INCLUDE_DIRS}
  ${GSTREAMER_BASE_INCLUDE_DIRS}
  ${LIBSOUP_INCLUDE_DIRS}
  ${GSSDP_INCLUDE_DIRS}
  ${JSON_INCLUDE_DIRS}
  ${NCLUA_INCLUDE_DIRS}
  ${LUA_INCLUDE_DIR}
)

set(LIBGINGA_LIBS
  pthread
  ${GLIB_LIBRARIES}
  ${GLIB_GIO_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  ${GDK3_LIBRARIES}
  ${GDKPIXBUF_LIBRARIES}
  ${CAIRO_LIBRARIES}
  ${FONTCONFIG_LIBRARIES}
  ${PANGO_LIBRARIES}
  ${RSVG_LIBRARIES}
  ${GSTREAMER_LIBRARIES}
  ${GSTREAMER_APP_LIBRARIES}
  ${GSTREAMER_VIDEO_LIBRARIES}
  ${LIBSOUP_LIBRARIES}
  ${GSSDP_LIBRARIES}
  ${JSON_LIBRARIES}
  ${NCLUA_LIBRARIES}
  ${LUA_LIBRARIES}
)

if(WITH_OPENGL)
  list(APPEND LIBGINGA_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
  list(APPEND LIBGINGA_LIBS ${OPENGL_LIBRARIES})
endif()

if(WITH_CEF)
  list(APPEND LIBGINGA_SOURCES
    ./lib/player/HTMLPlayer.cpp
  )
  list(APPEND GINGA_INCLUDE_DIRS
    ${CEF_ROOT}
    ${CEF_ROOT}/include
  )
  list(APPEND GINGA_LIBS
    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )
endif()

# gingagui sources
set(GINGAGUI_GTK_SOURCES
  ./src-gui/ginga-gtk.cpp
  ./src-gui/model/SaveLoadSettings.cpp
  ./src-gui/model/HttpMessage.cpp
  ./src-gui/view/TrackerWindow.cpp
  ./src-gui/view/AboutWindow.cpp
  ./src-gui/view/BigPictureWindow.cpp
  ./src-gui/view/Draw.cpp
  ./src-gui/view/FullscreenWindow.cpp
  ./src-gui/view/HelpWindow.cpp
  ./src-gui/view/MainWindow.cpp
  ./src-gui/view/TvControlWindow.cpp
)
set(GINGAGUI_GTK_INCLUDE_DIRS
  ./src-gui/
  ./src-gui/model/
  ./src-gui/view/
)

# libginga target
add_library(libginga ${LIBGINGA_SOURCES})

if(WITH_NCLUA_BUILD)
  add_dependencies(libginga nclua_build)
endif()

target_include_directories(libginga PUBLIC ${LIBGINGA_INCLUDE_DIRS})
target_link_libraries(libginga PUBLIC ${LIBGINGA_LIBS})
set_target_properties(libginga PROPERTIES OUTPUT_NAME "ginga")

# ginga target
add_executable(ginga src/ginga.cpp)
target_include_directories(ginga PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries(ginga PRIVATE libginga ${GTK3_LIBRARIES})

# gingagui target
list(APPEND GINGAGUI_GTK_INCLUDE_DIRS
  ${LIBGINGA_INCLUDE_DIRS}
  ${GTK3_INCLUDE_DIRS}
)
set(GINGAGUI_GTK_LIBS
  ${LIBGINGA_LIBS}
  ${GTK3_LIBRARIES}
)
add_executable(gingagui ${GINGAGUI_GTK_SOURCES})
target_include_directories(gingagui PRIVATE ${GINGAGUI_GTK_INCLUDE_DIRS})
target_link_libraries(gingagui PRIVATE libginga ${GINGAGUI_GTK_LIBS})

# optional ginga-gl target
if(WITH_OPENGL)
  add_executable(ginga-gl src/ginga-gl.cpp)
  target_include_directories(ginga-gl PRIVATE
    ${SDL2_INCLUDE_DIR}
  )
  target_link_libraries(ginga-gl PRIVATE
    libginga
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
  )
endif()

# optional ginga-qt target
if(WITH_GINGAQT)
  set(CMAKE_INCLUDE_CURRENTDIR ON)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_AUTOUIC ON)

  # find the QtWidgets library
  find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
  add_executable(ginga-qt
    ./src/ginga-qt.cpp
  )
  target_link_libraries(ginga-qt
    Qt5::Core
    Qt5::Widgets
    libginga
  )
  target_compile_definitions(ginga-qt PRIVATE -DQT_NO_KEYWORDS)
  target_include_directories(ginga-qt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${LIBGINGA_INCLUDE_DIRS}
  )
endif()

if(WITH_CEF)
  # Copy binary and resource files to the target output directory.
  COPY_FILES(ginga "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES(ginga "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

  # Set SUID permissions on the chrome-sandbox target.
  SET_LINUX_SUID_PERMISSIONS(ginga "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()

# tests
enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

macro(add_ginga_test target)
  add_executable(${target} EXCLUDE_FROM_ALL ${ARGN})
  target_include_directories(${target} PUBLIC ${GINGAGUI_GTK_INCLUDE_DIRS})
  target_link_libraries(${target} PUBLIC libginga ${GINGAGUI_GTK_LIBS})
  add_dependencies(${target} libginga)
  add_dependencies(check ${target})
  add_test(${target} ${GINGA_BINARY_DIR}/${target})

  if(${target} MATCHES xfail-*)
    set_tests_properties(${target} PROPERTIES WILL_FAIL TRUE)
  endif()
endmacro()

file(GLOB GINGA_TESTS_SRC "./tests/*.cpp")

foreach(SRC ${GINGA_TESTS_SRC})
  get_filename_component(TEST_NAME ${SRC} NAME_WE)
  add_ginga_test(${TEST_NAME} ${SRC})

  # fix PATH when using not-msys testing tool (vscode cmake-test-explorer)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_property(TEST ${TEST_NAME} PROPERTY ENVIRONMENT "PATH=%PATH%\;C:\\msys64\\mingw64\\bin")
  endif()
endforeach()

# tests use TOP_SRCDIR and ABS_TOP_SRCDIR
add_definitions(-DTOP_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}/")
add_definitions(-DABS_TOP_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}/")

# doxygen documentation
find_package(Doxygen)

if(DOXYGEN_FOUND)
  set(top_builddir ${CMAKE_CURRENT_BINARY_DIR})
  set(top_srcdir ${CMAKE_CURRENT_SOURCE_DIR}/..)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/./doc/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

# install lib/bin/data
set(ICON "${CMAKE_SOURCE_DIR}/src-gui/icons/common/ginga.ico")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_INSTALL_PREFIX "/usr/")
  install(TARGETS libginga DESTINATION lib)
  add_definitions(-DGINGADATADIR="/usr/share/ginga/")
  add_definitions(-DGINGABINDIR="/usr/bin/")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_INSTALL_PREFIX "C:/msys64/mingw64")
  install(TARGETS libginga DESTINATION bin)

  # windows do not uses GINGADATADIR/GINGABINDIR
endif()

install(TARGETS ginga DESTINATION bin)
install(TARGETS gingagui DESTINATION bin)
install(DIRECTORY src-gui/icons/ DESTINATION share/ginga/icons)
install(FILES src-gui/style/dark.css src-gui/style/light.css DESTINATION share/ginga/style)
install(FILES ${ICON} DESTINATION share/pixmaps)

# win deploy require a parent folder or gst fail loading. TODO: fix it
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_BUILD_TYPE MATCHES Release)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/Ginga")
  set(DST_DIR "dst")
  install(TARGETS libginga DESTINATION ${DST_DIR})
  install(TARGETS ginga DESTINATION ${DST_DIR})
  install(TARGETS gingagui DESTINATION ${DST_DIR})
  install(DIRECTORY src-gui/icons/ DESTINATION ${DST_DIR}/share/ginga/icons)
  install(FILES src-gui/style/dark.css src-gui/style/light.css DESTINATION ${DST_DIR}/share/ginga/style)
  install(FILES ${ICON} DESTINATION ${DST_DIR}/share/pixmaps)
endif()

# message after configure
message("
---
summary of main build options:

version:              ${VERSION}
CMAKE_SYSTEM_NAME:    ${CMAKE_SYSTEM_NAME}
CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}
CMAKE_C_COMPILER:     ${CMAKE_CXX_COMPILER}
STATIC_LINKING:       ${STATIC_LINKING}
BUILD_SHARED_LIBS:    ${BUILD_SHARED_LIBS}
CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}

Optional build:
cef player:         ${WITH_CEF}
ginga-qt:           ${WITH_GINGAQT}
ginga-gl:           ${WITH_OPENGL}
")

# cpack
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Ginga middleware")
set(CPACK_PACKAGE_VENDOR "TeleMídia Lab/PUC-Rio")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_CONTACT alan@telemidia.puc-rio.br)
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(HELP "http://github.com/TeleMidia/ginga")

# cpack deb
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_NAME "ginga-itv") # ginga already exist
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alan Guedes")
  set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/graphics")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Ginga® is the middleware of the Japanese-Brazilian Digital TV System (ISDB-TB) and ITU-T Recommendation for IPTV services. Ginga is made up by a set of standardized technologies and Brazilian innovations that make it the most advanced middleware specification.")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "liblua5.2-0, libglib2.0-0, libpango-1.0-0, librsvg2-2, libsoup2.4-1, libgstreamer1.0-0, libgstreamer-plugins-base1.0-0, libfontconfig1, libgtk-3-0, gstreamer1.0-plugins-good, gstreamer1.0-plugins-bad, gstreamer1.0-plugins-ugly, gstreamer1.0-libav, libgssdp-1.2-0, libjsoncpp1, nclua")
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
  set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
  set(CPACK_DEBIAN_REPLACES "xz")
endif()

# cpack nsis
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_BUILD_TYPE MATCHES Release)
  set(DISPLAY_NAME "Ginga")
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_PACKAGE_NAME "ginga")
  set(CPACK_NSIS_MUI_ICON ${ICON})
  set(CPACK_NSIS_MUI_UNIICON ${ICON})
  set(CPACK_NSIS_DISPLAY_NAME ${DISPLAY_NAME})
  set(CPACK_NSIS_BRANDING_TEXT ${DISPLAY_NAME})
  set(CPACK_NSIS_PACKAGE_NAME ${DISPLAY_NAME})
  set(CPACK_NSIS_HELP_LINK ${HELP})
  set(CPACK_NSIS_URL_INFO_ABOUT ${HELP})
  set(CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT})

  # win deploy require all dlls. TODO: automatic get them
  set(BIN_DLLS
    libsoup-2.4-1.dll libjsoncpp-24.dll libgssdp-1.2-0.dll libcairo-2.dll libgdk-3-0.dll libgio-2.0-0.dll libglib-2.0-0.dll libgobject-2.0-0.dll libatk-1.0-0.dll libbz2-1.dll libcairo-2.dll libcairo-gobject-2.dll libcurl-4.dll libepoxy-0.dll libexpat-1.dll libffi-7.dll libfontconfig-1.dll libfreetype-6.dll libgcc_s_seh-1.dll libgdk_pixbuf-2.0-0.dll libgdk-3-0.dll libgio-2.0-0.dll libglib-2.0-0.dll libgmodule-2.0-0.dll libgobject-2.0-0.dll libgraphite2.dll libgstapp-1.0-0.dll libgstbase-1.0-0.dll libgstreamer-1.0-0.dll libgstvideo-1.0-0.dll libgtk-3-0.dll libharfbuzz-0.dll libiconv-2.dll libidn2-0.dll libintl-8.dll liblzma-5.dll libnclua-0.dll libnghttp2-14.dll liborc-0.4-0.dll libpango-1.0-0.dll libpangocairo-1.0-0.dll libpangoft2-1.0-0.dll libpangowin32-1.0-0.dll libpcre-1.dll libpixman-1-0.dll libpng16-16.dll librsvg-2-2.dll libssp-0.dll libpsl-5.dll libthai-0.dll libdatrie-1.dll libbrotlidec.dll libbrotlicommon.dll libstdc++-6.dll libunistring-2.dll libwinpthread-1.dll libxml2-2.dll lua53.dll zlib1.dll avcodec-58.dll avfilter-7.dll avformat-58.dll avutil-56.dll libfaac-0.dll libfaad-2.dll libFLAC-8.dll libmpg123-0.dll libmpg123-0.dll libmpeg2-0.dll libnice-10.dll libogg-0.dll libtheora-0.dll libvpx-1.dll libcurl-4.dll libjpeg-8.dll gst-play-1.0.exe gst-inspect-1.0.exe libgstadaptivedemux-1.0-0.dll libgstapp-1.0-0.dll libgstbase-1.0-0.dll libgstaudio-1.0-0.dll libgstvideo-1.0-0.dll libgstreamer-1.0-0.dll libgstbadaudio-1.0-0.dll libgstbasecamerabinsrc-1.0-0.dll libgstcodecparsers-1.0-0.dll libgstcontroller-1.0-0.dll libgstfft-1.0-0.dll libgstgl-1.0-0.dll libgstmpegts-1.0-0.dll libgstnet-1.0-0.dll libgstpbutils-1.0-0.dll libgstriff-1.0-0.dll libgstrtsp-1.0-0.dll libgstsdp-1.0-0.dll libaom.dll libass-9.dll libbluray-2.dll libcelt0-2.dll libcrypto-1_1-x64.dll libdav1d.dll libfribidi-0.dll libgmp-10.dll libgnutls-30.dll libgomp-1.dll libgsm.dll libgstrtp-1.0-0.dll libgsttag-1.0-0.dll libgsturidownloader-1.0-0.dll libhogweed-6.dll libmfx-1.dll libmodplug-1.dll libmp3lame-0.dll libnettle-8.dll libopencore-amrnb-0.dll libopencore-amrwb-0.dll libopenjp2-7.dll libopus-0.dll libp11-kit-0.dll librtmp-1.dll libsoxr.dll libspeex-1.dll libsqlite3-0.dll libsrt.dll libssh2-1.dll libssl-1_1-x64.dll libtasn1-6.dll libtheoradec-1.dll libtheoraenc-1.dll libvorbis-0.dll libvorbisenc-2.dll libvulkan-1.dll libwebp-7.dll libwebpmux-3.dll libx264-161.dll libx265.dll libzstd.dll postproc-55.dll swresample-3.dll swscale-5.dll xvidcore.dll
  )

  foreach(DLL ${BIN_DLLS})
    install(FILES "C:/msys64/mingw64/bin/${DLL}" DESTINATION ${DST_DIR})
  endforeach()

  install(DIRECTORY "C:/msys64/mingw64/lib/gdk-pixbuf-2.0/" DESTINATION ${DST_DIR}/lib/gdk-pixbuf-2.0)
  install(DIRECTORY "C:/msys64/mingw64/lib/gstreamer-1.0/" DESTINATION ${DST_DIR}/lib/gstreamer-1.0)
  install(DIRECTORY "C:/msys64/mingw64/share/glib-2.0/" DESTINATION ${DST_DIR}/share/glib-2.0)
  install(DIRECTORY "C:/msys64/mingw64/share/icons/hicolor/16x16/" DESTINATION ${DST_DIR}/share/icons/hicolor/16x16)
  install(DIRECTORY "C:/msys64/mingw64/share/icons/Adwaita/16x16/" DESTINATION ${DST_DIR}/share/icons/Adwaita/16x16)
  install(DIRECTORY "C:/msys64/mingw64/share/icons/Adwaita/scalable/ui/" DESTINATION ${DST_DIR}/share/icons/Adwaita/16x16/actions)

  # nclua do not require dst parent dir
  install(DIRECTORY "C:/msys64/mingw64/share/lua/5.3/nclua/" DESTINATION share/lua/5.3/nclua)
  install(DIRECTORY "C:/msys64/mingw64/lib/lua/5.3/nclua/" DESTINATION lib/lua/5.3/nclua)

  # setup installer
  set(CPACK_PACKAGE_INSTALL_DIRECTORY ${DISPLAY_NAME}) # C:\Programs..\Ginga
  set(CPACK_NSIS_CREATE_ICONS_EXTRA "
    CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Ginga.lnk' '$INSTDIR\\\\${DST_DIR}\\\\gingagui.exe' ' ' '$INSTDIR\\\\${DST_DIR}\\\\share\\\\pixmaps\\\\ginga.ico'")
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    WriteRegStr HKCR '.ncl' '' 'ncl'
    WriteRegStr HKCR 'ncl' '' 'NCL Document'
    WriteRegStr HKCR 'ncl\\\\DefaultIcon' '' '$INSTDIR\\\\${DST_DIR}\\\\share\\\\pixmaps\\\\ginga.ico'
    WriteRegStr HKCR 'ncl\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\${DST_DIR}\\\\gingagui.exe %1'
    WriteRegStr HKCR 'ncl\\\\shell\\\\print\\\\command' '' '$INSTDIR\\\\${DST_DIR}\\\\gingagui.exe /p %1'
    WriteRegStr HKLM 'Software\\\\RegisteredApplications' 'Ginga' '$INSTDIR\\\\${DST_DIR}\\\\gingagui.exe'
    WriteRegStr HKCU 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\FileExts\\\\.ncl\\\\OpenWithList' 'a' '$INSTDIR\\\\${DST_DIR}\\\\gingagui.exe'
  ")

  # setup uninstaller
  set(CPACK_NSIS_UNINSTALL_NAME "Uninstall Ginga")
  set(CPACK_NSIS_DELETE_ICONS_EXTRA "
    Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Ginga.lnk'
    RMDir '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\'
    ")
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    DeleteRegKey HKCR '.ncl'
    DeleteRegKey HKCR 'ncl'
    DeleteRegKey HKCR 'ncl\\\\DefaultIcon'
    DeleteRegKey HKCR 'ncl\\\\shell\\\\open\\\\command'
    DeleteRegKey HKCU 'Software\\\\RegisteredApplications\\\\Ginga'
    DeleteRegKey HKCU 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\FileExts\\\\.ncl'  
  ")
endif()

include(CPack)
