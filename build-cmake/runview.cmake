cmake_minimum_required (VERSION 3.2)
project (runview)

set(CMAKE_INCLUDE_CURRENTDIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the QtWidgets library
find_package (Qt5 REQUIRED COMPONENTS Core Widgets)

qt5_wrap_ui (UI_GENERATED_HEADERS
  ../src-runview/widget.ui
)

add_executable (runview
  ../src-runview/main.cpp
  ../src-runview/widget.cpp
  ../src-runview/widget.h

  ${UI_GENERATED_HEADERS}
)

target_link_libraries (runview
  Qt5::Core
  Qt5::Widgets

  libginga
)

message (${LIBGINGA_INCLUDE_DIRS})

target_include_directories(runview PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}

  ${LIBGINGA_INCLUDE_DIRS}
)

