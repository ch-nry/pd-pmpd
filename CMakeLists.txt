cmake_minimum_required (VERSION 3.14)
project(pd-pmpd C)

set(PUREDATA_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/pure-data/src" CACHE PATH "Path to folder containing m_pd.h" )
if(WIN32)
  set(PUREDATA_LIB "${CMAKE_SOURCE_DIR}/pure-data/bin/pd.lib" CACHE PATH "Path to pd.lib for Windows")
endif()

include_directories(${PUREDATA_INCLUDE_DIRS})

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/package" CACHE PATH "Package destination folder" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang$")
  set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

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
  add_library(${PMPD_TARGET} SHARED ${SOURCE})

  if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE "${PUREDATA_LIB}")
  endif()

  # set library binary file name and extension
  set_target_properties(${PMPD_TARGET}
    PROPERTIES
      PREFIX ""
      OUTPUT_NAME "${OBJECT_NAME}")

  if(APPLE)
      set_target_properties(${PMPD_TARGET} PROPERTIES SUFFIX ".pd_darwin")
      set_target_properties(${PMPD_TARGET}
        PROPERTIES
        INSTALL_RPATH "@loader_path"
      )
  elseif(UNIX)
      if(${OSSIA_ARCHITECTURE} MATCHES "arm")
          set_target_properties(${PMPD_TARGET} PROPERTIES SUFFIX ".l_arm")
      elseif(${OSSIA_ARCHITECTURE} MATCHES "x86")
          set_target_properties(${PMPD_TARGET} PROPERTIES SUFFIX ".l_i386")
      else()
          set_target_properties(${PMPD_TARGET} PROPERTIES SUFFIX ".pd_linux")
      endif()
  endif()

  install(TARGETS ${PMPD_TARGET}
      DESTINATION pmpd
      PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                  GROUP_READ GROUP_EXECUTE
                  WORLD_READ WORLD_EXECUTE
  )
endforeach()

install(DIRECTORY examples/ DESTINATION pmpd
        FILES_MATCHING PATTERN "*.pd")

install(FILES 
  manual/pmpd.pdf
  manual/pmpd.sxw
  DESTINATION pmpd
)
