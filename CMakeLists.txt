cmake_minimum_required(VERSION 3.2)
project(ginga)
cmake_policy(SET CMP0087 NEW)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wno-deprecated-declarations)
set(PACKAGE "ginga")
set(PACKAGE_URL "http://github.com/telemidia/ginga")
set(PACKAGE_BUGREPORT "bugs@telemidia.puc-rio.br")
set(VERSION 1.0)
set(PACKAGE_VERSION ${VERSION})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build-cmake/CMakeModules)

# set all target runtime to CMAKE_BINARY_DIR
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# ------------------------
# required deps
# ------------------------
find_package(GLIB 2.32 REQUIRED COMPONENTS gobject gio gthread gmodule)
find_package(LibSoup 2.4 REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(GDK3 REQUIRED)
find_package(Cairo REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(Pango REQUIRED)
find_package(RSVG REQUIRED)
find_package(GStreamer 1.0 REQUIRED)
find_package(GTK3 REQUIRED)
pkg_check_modules(JSON jsoncpp REQUIRED)
find_package(Lua 5.3 REQUIRED)
pkg_check_modules(GSSDP gssdp-1.2 REQUIRED)
option(WITH_NCLUA_BUILD "Download and build nclua." OFF)
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
else()
  PKG_CHECK_MODULES(NCLUA nclua REQUIRED)
endif()

# ------------------------
# optional targets
# ------------------------
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
  add_subdirectory(${CEF_ROOT}/libcef_dll libcef_dll_bin)
  # logical target used to link the libcef library.
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
  set(CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/")
  set(WITH_CEF ON)
endif()

if(WITH_OPENGL)
  list(APPEND LIBGINGA_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
  list(APPEND LIBGINGA_LIBS ${OPENGL_LIBRARIES})
endif()

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

# ------------------------
# ginga and gingagui targets
# ------------------------

# tests use TOP_SRCDIR and ABS_TOP_SRCDIR
set(TOP_SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}/")
set(ABS_TOP_SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}/")

# libginga
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
if(WITH_CEF)
  list(APPEND LIBGINGA_SOURCES ./lib/player/HTMLPlayer.cpp )
  list(APPEND GINGA_INCLUDE_DIRS ${CEF_ROOT} ${CEF_ROOT}/include)
  list(APPEND GINGA_LIBS libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
endif()

set(LIBGINGA_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/lib/
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
  ${CMAKE_BINARY_DIR} # conifured config.h
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

# libginga target
add_library(libginga ${LIBGINGA_SOURCES})

if(WITH_NCLUA_BUILD)
  add_dependencies(libginga nclua_build)
endif()

target_include_directories(libginga PRIVATE ${LIBGINGA_INCLUDE_DIRS})
target_link_libraries(libginga PRIVATE ${LIBGINGA_LIBS})
set_target_properties(libginga PROPERTIES OUTPUT_NAME "ginga")

# ginga/gingagui source
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
  ${LIBGINGA_INCLUDE_DIRS}
  ${GTK3_INCLUDE_DIRS}
)
set(GINGAGUI_GTK_LIBS
  ${LIBGINGA_LIBS}
  ${GTK3_LIBRARIES}
)

# ginga/gingagui targets
add_executable(ginga src/ginga.cpp)
target_include_directories(ginga PRIVATE ${GINGAGUI_GTK_INCLUDE_DIRS})
target_link_libraries(ginga PRIVATE libginga ${GTK3_LIBRARIES})

add_executable(gingagui ${GINGAGUI_GTK_SOURCES})
target_include_directories(gingagui PRIVATE ${GINGAGUI_GTK_INCLUDE_DIRS})
target_link_libraries(gingagui PRIVATE libginga ${GINGAGUI_GTK_LIBS})

# ------------------------
# tests
# ------------------------
enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})

macro(add_ginga_test target)
  add_executable(${target} EXCLUDE_FROM_ALL ${ARGN})
  target_include_directories(${target} PRIVATE ${GINGAGUI_GTK_INCLUDE_DIRS})
  target_link_libraries(${target} PRIVATE libginga ${GINGAGUI_GTK_LIBS})
  add_dependencies(${target} libginga)
  add_dependencies(check ${target})
  add_test(${target} ${CMAKE_BINARY_DIR}/${target})

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

# ------------------------
# doxygen documentation
# ------------------------
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

# ------------------------
# install
# ------------------------
set(ICON "${CMAKE_SOURCE_DIR}/src-gui/icons/common/ginga.ico")

# <install_prefix> should differ from Debug and Release
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_INSTALL_PREFIX "/usr/")
  install(TARGETS libginga DESTINATION lib)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  install(TARGETS libginga DESTINATION bin)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/deploy)
endif()

# install files src
install(TARGETS ginga DESTINATION bin)
install(TARGETS gingagui DESTINATION bin)
install(DIRECTORY src-gui/icons/ DESTINATION share/ginga/icons)
install(FILES src-gui/style/dark.css src-gui/style/light.css DESTINATION share/ginga/style)
install(FILES ${ICON} DESTINATION share/pixmaps)
set(GINGADATADIR ${CMAKE_INSTALL_PREFIX}/share/ginga/)
set(GINGABINDIR ${CMAKE_INSTALL_PREFIX}/bin)

# others required install at Windows 
if(CMAKE_SYSTEM_NAME STREQUAL "Windows") 
  # Windows requires the following extra steps:
  # * <install_prefix>/bin/gingagui.exe requires:
  install(DIRECTORY "C:/msys64/mingw64/share/icons/hicolor/16x16/" DESTINATION share/icons/hicolor/16x16)
  install(DIRECTORY "C:/msys64/mingw64/share/icons/Adwaita/16x16/" DESTINATION share/icons/Adwaita/16x16)
  install(DIRECTORY "C:/msys64/mingw64/share/icons/Adwaita/scalable/ui/" DESTINATION share/icons/Adwaita/16x16/actions)
  
  # * <install_prefix>/bin/libnclua-0.dll  requires:
  install(DIRECTORY "C:/msys64/mingw64/share/lua/5.3/nclua/" DESTINATION share/lua/5.3/nclua)
  install(DIRECTORY "C:/msys64/mingw64/lib/lua/5.3/nclua/" DESTINATION lib/lua/5.3/nclua)
  
  # * <install_prefix>/bin/libginga.dll requires:
  install(DIRECTORY "C:/msys64/mingw64/lib/gdk-pixbuf-2.0/" DESTINATION lib/gdk-pixbuf-2.0)
  
  # * <install_prefix>/bin/libglib-2.0-0.dll  requires:
  install(DIRECTORY "C:/msys64/mingw64/share/glib-2.0/" DESTINATION share/glib-2.0)
  # * <install_prefix>/bin/libgstreamer-1.0-0.dll 
  # https://gstreamer.freedesktop.org/documentation/installing/on-windows.html?gi-language=c#download-and-install-gstreamer-binaries
  install(DIRECTORY "C:/msys64/mingw64/lib/gstreamer-1.0/" DESTINATION lib/gstreamer-1.0)
  # * <install_prefix> is suposed to run outside msys2 bash (from cmd orpowershell), so ginga deps should be packaged.
  # The following code use GET_RUNTIME_DEPENDENCIES to get at runtime
  # https://discourse.cmake.org/t/file-get-runtime-deps-issues/2574/3
  install(CODE [[
    LIST(APPEND pre_exclude_regexes "api-ms-.*") # windows API
    LIST(APPEND pre_exclude_regexes "ext-ms-.*") # windows API
    LIST(APPEND pre_exclude_regexes "ieshims.dll") # windows API
    LIST(APPEND pre_exclude_regexes "emclient.dll") # windows API
    LIST(APPEND pre_exclude_regexes "devicelockhelpers.dll") # windows API
    LIST(APPEND post_exclude_regexes ".*WINDOWS[\\/]system32.*") # windows system dlls
    file(GLOB GST_PLUGINS "C:/msys64/mingw64/lib/gstreamer-1.0/*.dll")
    file(GET_RUNTIME_DEPENDENCIES
      EXECUTABLES $<TARGET_FILE:ginga> $<TARGET_FILE:gingagui> "C:/msys64/mingw64/bin/gst-play-1.0.exe"
      LIBRARIES ${GST_PLUGINS} $<TARGET_FILE:libginga>
      DIRECTORIES "C:/msys64/mingw64/bin/"
      RESOLVED_DEPENDENCIES_VAR _r_deps
      UNRESOLVED_DEPENDENCIES_VAR _u_deps
      PRE_EXCLUDE_REGEXES ${pre_exclude_regexes}
      POST_EXCLUDE_REGEXES ${post_exclude_regexes}
      POST_INCLUDE_REGEXES ${post_include_regexes}
      )
    # message("-- GET_RUNTIME_DEPENDENCIES LIBRARIES=$<TARGET_FILE:libginga> ${GST_PLUGINS}")
    # message("-- GET_RUNTIME_DEPENDENCIES EXECUTABLES=$<TARGET_FILE:ginga> $<TARGET_FILE:gingagui> C:/msys64/mingw64/bin/gst-play-1.0.exe")  
    # message("-- GET_RUNTIME_DEPENDENCIES RESOLVED_DEPENDENCIES_VAR=${_r_deps}")
    # message("-- GET_RUNTIME_DEPENDENCIES UNRESOLVED_DEPENDENCIES_VAR=${_u_deps}")
    foreach(_file ${_r_deps})
        file(INSTALL
              DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
              TYPE SHARED_LIBRARY
              FOLLOW_SYMLINK_CHAIN
              FILES "${_file}"
        )
    endforeach()
  ]])
  # * install gst-play-1.0.exe for testing 
  install(FILES "C:/msys64/mingw64/bin/gst-play-1.0.exe" DESTINATION bin)
endif()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/build-cmake/config.h.in ${CMAKE_BINARY_DIR}/config.h)

# ------------------------
# cpack
# ------------------------
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The Ginga middleware")
set(CPACK_PACKAGE_VENDOR "TeleMídia Lab/PUC-Rio")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_CONTACT alan@telemidia.puc-rio.br)
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set(HELP "http://github.com/TeleMidia/ginga")

# cpack deb uses default install calls
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_NAME "ginga-itv") # ginga already exist
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alan Guedes")
  set(CPACK_DEBIAN_PACKAGE_SECTION "contrib/graphics")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Ginga is the middleware of the Japanese-Brazilian Digital TV System (ISDB-TB) and ITU-T Recommendation for IPTV services. Ginga is made up by a set of standardized technologies and Brazilian innovations that make it the most advanced middleware specification.")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "liblua5.2-0, libglib2.0-0, libpango-1.0-0, librsvg2-2, libsoup2.4-1, libgstreamer1.0-0, libgstreamer-plugins-base1.0-0, libfontconfig1, libgtk-3-0, gstreamer1.0-plugins-good, gstreamer1.0-plugins-bad, gstreamer1.0-plugins-ugly, gstreamer1.0-libav, libgssdp-1.2-0, libjsoncpp1, nclua")
  set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
  set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
  set(CPACK_DEBIAN_REPLACES "xz")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
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
  # setup installer
  set(CPACK_PACKAGE_INSTALL_DIRECTORY ${DISPLAY_NAME}) # C:\Programs..\Ginga
  set(CPACK_NSIS_CREATE_ICONS_EXTRA "
    CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Ginga.lnk' '$INSTDIR\\\\bin\\\\gingagui.exe' ' ' '$INSTDIR\\\\bin\\\\share\\\\pixmaps\\\\ginga.ico'")
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    WriteRegStr HKCR '.ncl' '' 'ncl'
    WriteRegStr HKCR 'ncl' '' 'NCL Document'
    WriteRegStr HKCR 'ncl\\\\DefaultIcon' '' '$INSTDIR\\\\bin\\\\share\\\\pixmaps\\\\ginga.ico'
    WriteRegStr HKCR 'ncl\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\bin\\\\gingagui.exe %1'
    WriteRegStr HKCR 'ncl\\\\shell\\\\print\\\\command' '' '$INSTDIR\\\\bin\\\\gingagui.exe /p %1'
    WriteRegStr HKLM 'Software\\\\RegisteredApplications' 'Ginga' '$INSTDIR\\\\bin\\\\gingagui.exe'
    WriteRegStr HKCU 'Software\\\\Microsoft\\\\Windows\\\\CurrentVersion\\\\Explorer\\\\FileExts\\\\.ncl\\\\OpenWithList' 'a' '$INSTDIR\\\\bin\\\\gingagui.exe'
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

# ------------------------
# message after configure
# ------------------------
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
