cmake_minimum_required(VERSION 3.2)
project(ginga)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (PACKAGE "ginga")
set (VERSION 1.0)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build-cmake/CMakeModules)

if (UNIX)
  set (CMAKE_INSTALL_PREFIX /usr/)
  set (GINGADATADIR /usr/share/ginga/)
  set (GINGABINDIR /usr/bin)
  add_definitions (-DGINGADATADIR="${GINGADATADIR}")
  add_definitions (-DGINGABINDIR="${GINGABINDIR}")
endif()

# set all target runtime to bin
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GINGA_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GINGA_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GINGA_BINARY_DIR})

# find dependencies
find_package (GLIB 2.32 REQUIRED COMPONENTS gobject gio gthread gmodule)
find_package (LibSoup 2.4 REQUIRED)
find_package (LibXml2 REQUIRED)
find_package (GDK3 REQUIRED)
find_package (Cairo REQUIRED)
find_package (Fontconfig REQUIRED)
find_package (Pango REQUIRED)
find_package (RSVG REQUIRED)
find_package (GStreamer 1.0 REQUIRED)
find_package (Lua 5.2 REQUIRED)
find_package (GTK3 REQUIRED)
pkg_check_modules (GSSDP gssdp-1.2 REQUIRED)
pkg_check_modules (JSON jsoncpp REQUIRED)

# optional dependencies
option (WITH_NCLUA_BUILD "Download and build nclua." OFF)
option (WITH_CEF "Build with chromium embedded support." OFF)
option (WITH_OPENGL "Build Ginga with opengl support." OFF)
option (WITH_GINGAQT "Build nclcomposer's ginga plugin." OFF)

if (WITH_OPENGL)
  find_package (SDL2)
  find_package (OpenGL)
  set (WITH_OPENGL ON)
else ()
  set (WITH_OPENGL OFF) # openGL not found, turn it off
endif()

if (WITH_OPENGL)
  add_definitions (-DWITH_OPENGL=1)
endif ()

if (WITH_NCLUA_BUILD) 
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
  ExternalProject_Get_Property (nclua_build install_dir)
  set (NCLUA_INCLUDE_DIRS "${install_dir}/include")
  set (NCLUA_LIBRARIES "${install_dir}/lib/libnclua.a")
  link_directories ("${install_dir}/lib")
  add_definitions (-DWITH_NCLUA=1)
else()
  PKG_CHECK_MODULES(NCLUA nclua REQUIRED)
  add_definitions (-DWITH_NCLUA=1)
endif ()

if (WITH_CEF) 
  # CEF download/configure/build/install
  set(CEF_VERSION cef_binary_3.3029.1619.geeeb5d7_linux64_minimal)
  set(CEF_SHA1 f809ce65b0b532fcbba6cec5f0e60f731fd3cbbd)
  set(CEF_FILE ${CEF_VERSION}.tar.bz2)
  set(CEF_URL http://opensource.spotify.com/cefbuilds/${CEF_FILE})
  set(CEF_LOCAL_PATH ${CMAKE_BINARY_DIR}/3rdparty)
  set(CEF_LOCAL_FILE ${CMAKE_BINARY_DIR}/3rdparty/${CEF_FILE}.tar.gz)
  if (NOT EXISTS "${CEF_LOCAL_FILE}")
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
  SET (CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/")

else()
  add_definitions (-DWITH_CEF=0)
endif()

# libginga sources
set (LIBGINGA_SOURCES
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

configure_file (${CMAKE_SOURCE_DIR}/build-cmake/config.h.in ${CMAKE_BINARY_DIR}/config.h)

set (LIBGINGA_INCLUDE_DIRS
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

set (LIBGINGA_LIBS
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

if (WITH_OPENGL)
  list (APPEND LIBGINGA_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
  list (APPEND LIBGINGA_LIBS ${OPENGL_LIBRARIES})
endif ()

if (WITH_CEF)
  list (APPEND LIBGINGA_SOURCES
    ./lib/player/HTMLPlayer.cpp
  )
  list (APPEND GINGA_INCLUDE_DIRS
    ${CEF_ROOT}
    ${CEF_ROOT}/include
  )
  list (APPEND GINGA_LIBS
    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )
endif ()

# gingagui sources
set (GINGAGUI_GTK_SOURCES
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
set (GINGAGUI_GTK_INCLUDE_DIRS
  ./src-gui/
  ./src-gui/model/
  ./src-gui/view/
)

# libginga target
add_library (libginga ${LIBGINGA_SOURCES})
if (WITH_NCLUA_BUILD)
  add_dependencies (libginga nclua_build)
endif()
target_include_directories (libginga PUBLIC ${LIBGINGA_INCLUDE_DIRS})
target_link_libraries (libginga PUBLIC ${LIBGINGA_LIBS})
set_target_properties (libginga PROPERTIES OUTPUT_NAME "ginga")

# ginga target
add_executable (ginga src/ginga.cpp)
target_include_directories (ginga PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries (ginga PRIVATE libginga ${GTK3_LIBRARIES})

# gingagui target
list (APPEND GINGAGUI_GTK_INCLUDE_DIRS
  ${LIBGINGA_INCLUDE_DIRS}
  ${GTK3_INCLUDE_DIRS}
)
set (GINGAGUI_GTK_LIBS
  ${LIBGINGA_LIBS}
  ${GTK3_LIBRARIES}
)
add_executable (gingagui ${GINGAGUI_GTK_SOURCES})
target_include_directories (gingagui PRIVATE ${GINGAGUI_GTK_INCLUDE_DIRS})
target_link_libraries (gingagui PRIVATE libginga ${GINGAGUI_GTK_LIBS})

# optional ginga-gl target
if (WITH_OPENGL)
  add_executable (ginga-gl src/ginga-gl.cpp)
  target_include_directories (ginga-gl PRIVATE
    ${SDL2_INCLUDE_DIR}
  )
  target_link_libraries (ginga-gl PRIVATE
    libginga
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
  )
endif ()

# optional ginga-qt target
if (WITH_GINGAQT)
  set (CMAKE_INCLUDE_CURRENTDIR ON)
  set (CMAKE_AUTOMOC ON)
  set (CMAKE_AUTORCC ON)
  set (CMAKE_AUTOUIC ON)
  # find the QtWidgets library
  find_package (Qt5 REQUIRED COMPONENTS Core Widgets)
  add_executable (ginga-qt
    ./src/ginga-qt.cpp
  )
  target_link_libraries (ginga-qt
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
endif ()

# Set rpath so that libraries can be placed next to the executable.
# set_target_properties (ginga PROPERTIES INSTALL_RPATH "$ORIGIN")
# set_target_properties (ginga PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

if (WITH_CEF)
  # Copy binary and resource files to the target output directory.
  COPY_FILES(ginga "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES(ginga "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
  # Set SUID permissions on the chrome-sandbox target.
  SET_LINUX_SUID_PERMISSIONS(ginga "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()

# tests
enable_testing ()
add_custom_target (check COMMAND ${CMAKE_CTEST_COMMAND})
macro (add_ginga_test target)
  add_executable (${target} EXCLUDE_FROM_ALL ${ARGN})
  target_include_directories (${target} PUBLIC ${GINGAGUI_GTK_INCLUDE_DIRS})
  target_link_libraries (${target} PUBLIC libginga ${GINGAGUI_GTK_LIBS})
  add_dependencies (${target} libginga)
  add_dependencies (check ${target})
  add_test (${target} ${GINGA_BINARY_DIR}/${target})
  if (${target} MATCHES xfail-*)
    set_tests_properties(${target} PROPERTIES WILL_FAIL TRUE)
  endif()
endmacro ()
file(GLOB GINGA_TESTS_SRC "./tests/*.cpp")
foreach (SRC ${GINGA_TESTS_SRC})
  get_filename_component (TEST_NAME ${SRC} NAME_WE)
  add_ginga_test (${TEST_NAME} ${SRC})
endforeach ()

# tests use TOP_SRCDIR and ABS_TOP_SRCDIR
add_definitions (-DTOP_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}/")
add_definitions (-DABS_TOP_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}/")

# Doxygen documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set (top_builddir ${CMAKE_CURRENT_BINARY_DIR})
  set (top_srcdir ${CMAKE_CURRENT_SOURCE_DIR}/..)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/./doc/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

# message after configure
message ( "
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

# install
# install (DIRECTORY ${CMAKE_BINARY_OUTPUT_DIR} DESTINATION bin)
if (WIN32)
  install (TARGETS libginga DESTINATION bin)
else ()
  install (TARGETS libginga DESTINATION lib)
endif()
install (TARGETS ginga DESTINATION bin)
install (TARGETS gingagui DESTINATION bin)
install (DIRECTORY src-gui/icons/ DESTINATION share/ginga/icons)
install (FILES src-gui/style/dark.css src-gui/style/light.css DESTINATION share/ginga/style)
install (FILES src-gui/application-x-ncl-NCL.xml DESTINATION share/ginga/mime)
install (FILES src-gui/icons/common/ginga.ico DESTINATION share/pixmaps)

# cpack
set (CPACK_PACKAGE_NAME "ginga-itv") 
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Ginga middleware") 
set (CPACK_PACKAGE_VENDOR "TeleMídia Lab/PUC-Rio")
set (CPACK_PACKAGE_VERSION ${VERSION})
set (CPACK_PACKAGE_EXECUTABLE "gingagui;ginga")
set (CPACK_PACKAGE_CONTACT alan@telemidia.puc-rio.br)
set (CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")

# deb
set (CPACK_GENERATOR "DEB")
set (CPACK_DEBIAN_PACKAGE_MAINTAINER "Alan Guedes")
set (CPACK_DEBIAN_PACKAGE_SECTION "contrib/graphics")
set (CPACK_DEBIAN_PACKAGE_DESCRIPTION "Ginga® is the middleware of the Japanese-Brazilian Digital TV System (ISDB-TB) and ITU-T Recommendation for IPTV services. Ginga is made up by a set of standardized technologies and Brazilian innovations that make it the most advanced middleware specification.")
# set (CPACK_DEBIAN_PACKAGE_DEPENDS "liblua5.2-dev, libglib2.0-dev, libpango1.0-dev, librsvg2-dev, libsoup2.4-dev, libgstreamer1.0-dev, libgstreamer-plugins-base1.0-dev, libfontconfig1-dev, libgtk-3-dev, gstreamer1.0-plugins-good, gstreamer1.0-plugins-bad, gstreamer1.0-plugins-ugly, gstreamer1.0-libav, nclua")
set (CPACK_DEBIAN_PACKAGE_DEPENDS "liblua5.2-0, libglib2.0-0, libpango-1.0-0, librsvg2-2, libsoup2.4-1, libgstreamer1.0-0, libgstreamer-plugins-base1.0-0, libfontconfig1, libgtk-3-0, gstreamer1.0-plugins-good, gstreamer1.0-plugins-bad, gstreamer1.0-plugins-ugly, gstreamer1.0-libav, libgssdp-1.2-0, libjsoncpp1, nclua")
set (CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set (CPACK_DEBIAN_COMPRESSION_TYPE "xz")
set (CPACK_DEBIAN_REPLACES "xz")

include (CPack)
