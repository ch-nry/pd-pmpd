cmake_minimum_required (VERSION 3.14)
project(pd-pmpd C)

# Platform-specific and compiler-specific settings
if(MSVC)
  # MSVC-specific flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /O2 /Ot /fp:fast")
else()
  # GCC/Clang-specific flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -ffast-math -ffinite-math-only -finline-functions -funroll-loops")
endif()

if(APPLE)
  set(platform "darwin")
elseif(LINUX)
  set(platform "linux")
elseif(WIN32)
  set(platform "windows")
endif()
if(platform STREQUAL "windows")
  set(extension "dll")
else()
  set(extension "so")
endif()
if(CMAKE_GENERATOR_PLATFORM MATCHES Win32)
  set(arch i386)
elseif(APPLE AND CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(arch "fat")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*arm64.*")
  set(arch "arm64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*arm.*")
  set(arch "arm")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*aarch64.*")
  set(arch "aarch64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*64.*")
  set(arch "amd64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*86.*")
  set(arch "i386")
endif()

if(PD_FLOATSIZE64)
 set(floatsize "64")
else()
 set(floatsize "32")
endif()

set (PUREDATA_PLATFORM_SUFFIX "${platform}" CACHE STRING "the target platform (darwin, linux, windows) - leave empty to autodetect")
set (PUREDATA_ARCH_SUFFIX "${arch}" CACHE STRING "the target architecture (amd64, arm, arm64, i386, fat) - leave empty to autodetect")
set (PUREDATA_LIB_SUFFIX "${extension}" CACHE STRING "the dynamic library extension (so, dll) - leave empty to autodetect")


# apply with argument '-DPD_FLOATSIZE64=ON' or '-DPUREDATA_FLOATSIZE=64'
set(PUREDATA_FLOATSIZE ${floatsize} CACHE STRING "the floatsize for processing (32, 64) - the default is 32")
if(PUREDATA_FLOATSIZE STREQUAL 64)
  add_definitions(-DPD_FLOATSIZE=64)
elseif(PUREDATA_FLOATSIZE STREQUAL 32)
else()
  message(FATAL "FLOATSIZE must be 32 or 64")
endif()

set (PUREDATA_SUFFIX "" CACHE STRING "the external extension (.linux_amd64_64.so, .d_fat, .dll) - leave empty to autodetect (.${platform}-${arch}-${floatsize}.${extension})")

if(PUREDATA_SUFFIX STREQUAL "")
  if(PUREDATA_PLATFORM_SUFFIX STREQUAL "")
    message(FATAL "Could not determine target platform")
  endif()
  if(PUREDATA_ARCH_SUFFIX STREQUAL "")
    message(FATAL "Could not determine target architecture")
  endif()

  set(PUREDATA_SUFFIX ".${PUREDATA_PLATFORM_SUFFIX}-${PUREDATA_ARCH_SUFFIX}-${PUREDATA_FLOATSIZE}.${PUREDATA_LIB_SUFFIX}")
  # necessary exception?
  if(PUREDATA_FLOATSIZE STREQUAL 32 AND APPLE AND CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(PUREDATA_SUFFIX ".d_fat")
  endif()
endif()



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
      OUTPUT_NAME "${OBJECT_NAME}"
      C_VISIBILITY_PRESET hidden
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1
      DEFINE_SYMBOL "PMPD_EXPORTS"
  )

  set_target_properties(${PMPD_TARGET} PROPERTIES SUFFIX "${PUREDATA_SUFFIX}")

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
        FILES_MATCHING PATTERN "*.pd" PATTERN "*.html")

install(FILES
  LICENSE.txt
  README.md
  DESTINATION pmpd
)
