cmake_minimum_required(VERSION 3.2)
project(ginga-docs LANGUAGES NONE)
# include common defs
include ("../CMakeLists.defs.txt")
# disable cpp checking
set(CMAKE_C_COMPILER_FORCED FALSE)
set(CMAKE_CXX_COMPILER_FORCED FALSE)

# doxygen targe
set(DOXYGEN_PROJECT_NAME ${PACKAGE})
find_package(Doxygen)
set (DOC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(Doxyfile.in Doxyfile @ONLY)
add_custom_target(doc ALL ${DOXYGEN_EXECUTABLE} ${DOC_DIR}/Doxyfile
  WORKING_DIRECTORY ${DOC_DIR}
  COMMENT "Generating API documentation with Doxygen" VERBATIM
)