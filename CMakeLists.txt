cmake_minimum_required (VERSION 3.14)
project(pd-pmpd C)

SET(CMAKE_C_FLAGS "-O3 -ffinite-math-only -ffast-math -Wall -Werror")

set(PUREDATA_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/pure-data/src" CACHE PATH "Path to folder containing m_pd.h" )
if(WIN32)
  set(PUREDATA_LIB "${CMAKE_SOURCE_DIR}/pure-data/bin/pd.lib" CACHE PATH "Path to pd.lib for Windows")
endif()

include_directories(${PUREDATA_INCLUDE_DIRS} ${CMAKE_BINARY_DIR})

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/package" CACHE PATH "Package destination folder" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang$")
  set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

# Get git info
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
include(git_info)
get_commit_sha(GIT_SHA)
get_commit_date(GIT_DATE)
get_tag(GIT_TAG)
configure_file("${CMAKE_SOURCE_DIR}/pmpd_version.c.in" "${CMAKE_BINARY_DIR}/pmpd_version.c" @ONLY)

set(SOURCES
  iAmbient2D.c
  iAmbient3D.c
  iCircle2D.c
  iCircle3D.c
  iCylinder3D.c
  iLine2D.c
  iPlane3D.c
  iSeg2D.c
  iSphere3D.c
  link.c
  link2D.c
  link3D.c
  mass.c
  mass2D.c
  mass3D.c
  pmpd.c
  pmpd2d.c
  pmpd3d.c
  pmpd~.c
  pmpd2d~.c
  pmpd3d~.c
  tCircle2D.c
  tCircle3D.c
  tCube3D.c
  tCylinder3D.c
  tLine2D.c
  tLink2D.c
  tLink3D.c
  tPlane3D.c
  tSeg2D.c
  tSphere3D.c
  tSquare2D.c
)

foreach(SOURCE IN LISTS SOURCES)
  message(STATUS "Processing ${SOURCE}")
  get_filename_component(OBJECT_NAME ${SOURCE} NAME_WLE)
  string(REPLACE "~" "_tilde" PMPD_TARGET ${OBJECT_NAME})

  add_library(${PMPD_TARGET} SHARED ${SOURCE} pmpd_version.h "${CMAKE_BINARY_DIR}/pmpd_version.c")

  if(WIN32)
    target_link_libraries(${PMPD_TARGET} PRIVATE "${PUREDATA_LIB}")
  endif()

  # set library binary file name and extension
  set_target_properties(${PMPD_TARGET}
    PROPERTIES
      PREFIX ""
      OUTPUT_NAME "${OBJECT_NAME}")

  set_target_properties(${PMPD_TARGET} PROPERTIES
      C_VISIBILITY_PRESET hidden
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1
      )

  set_target_properties(${PMPD_TARGET} PROPERTIES DEFINE_SYMBOL "PMPD_EXPORTS")

  if(APPLE)
    set(PLATFORM_SUFFIX "d") # d for Darwin
  elseif(UNIX)
    set(PLATFORM_SUFFIX "l") # l for Linux
  else()
    set(PLATFORM_SUFFIX "m") # m for Microsoft
  endif()
 
  if(CMAKE_GENERATOR_PLATFORM MATCHES Win32)
    set(ARCH_SUFFIX i386)
  elseif(APPLE AND CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(ARCH_SUFFIX "fat")
  elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*arm.*")
    set(ARCH_SUFFIX "arm")
  elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*aarch64.*")
    set(ARCH_SUFFIX "aarch64")
  elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*64.*")
    set(ARCH_SUFFIX "amd64")
  elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*86.*")
    set(ARCH_SUFFIX "i386")
  else()
    message(FATAL "Could not determine target architecture")
  endif()

  set_target_properties(${PMPD_TARGET} PROPERTIES SUFFIX ".${PLATFORM_SUFFIX}_${ARCH_SUFFIX}" PREFIX "")

  install(TARGETS ${PMPD_TARGET}
      DESTINATION pmpd
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                  GROUP_READ GROUP_EXECUTE
                  WORLD_READ WORLD_EXECUTE
  )

  install(FILES "${OBJECT_NAME}-help.pd" DESTINATION pmpd)
endforeach()

include(GenerateExportHeader)
generate_export_header(pmpd)

install(DIRECTORY examples/ DESTINATION pmpd/examples
        FILES_MATCHING PATTERN "*.pd")

install(FILES 
  manual/pmpd.pdf
  LICENSE.txt
  README.md
  DESTINATION pmpd
)
