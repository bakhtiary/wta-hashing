cmake_minimum_required (VERSION 2.6)
project (dotProductHashing)
find_package (Threads)

find_package( BLAS REQUIRED )
message( STATUS BLAS found: ${BLAS_LIBRARIES} )

FIND_PACKAGE( Boost 1.40 COMPONENTS program_options REQUIRED )
IF(Boost_FOUND)
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
ELSEIF(NOT Boost_FOUND)
  MESSAGE(FATAL_ERROR "Unable to find correct Boost version. Did you set BOOST_ROOT?")
ENDIF()

FIND_PACKAGE(Boost 1.40 COMPONENTS python )

SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )

INCLUDE_DIRECTORIES( "include/" )
INCLUDE_DIRECTORIES( "src/" )
INCLUDE_DIRECTORIES( "gtest-1.7.0/fused-src" )
INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIRS}" "/usr/include/python2.7")

file(GLOB include_SRC
    "include/*.h"
)

file(GLOB core_SRC
    "src/core/*.h"
    "src/core/*.cpp"
)

file(GLOB util_src
    "src/util/*.h"
    "src/util/*.cpp"
)

file(GLOB test_SRC
    "src/test/*.h"
    "src/test/*.cpp"
)

add_executable(example1 ${core_SRC} ${util_src} "src/examples/example1.cpp")
add_executable(largeDB ${core_SRC} ${util_src} "src/examples/largeDB.cpp")
TARGET_LINK_LIBRARIES(example1 ${Boost_LIBRARIES} )

#file(GLOB gtest_SRC
#    "gtest-1.7.0/src/*.cc"
#)
#add_executable(gtest ${core_SRC} ${test_SRC} ${gtest_SRC} )

add_executable(gtest ${core_SRC} ${util_src} ${test_SRC} ${gtest_SRC} "gtest-1.7.0/fused-src/gtest/gtest-all.cc" "gtest-1.7.0/fused-src/gtest/gtest_main.cc")
TARGET_LINK_LIBRARIES(gtest ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

ADD_LIBRARY(dot_product_hasher STATIC ${core_SRC} ${util_src} src/python/DotProductHasher.cxx)
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(dot_product_hasher PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
TARGET_LINK_LIBRARIES(dot_product_hasher ${Boost_LIBRARIES})

ADD_LIBRARY(py_dot_product_hasher SHARED ${core_SRC} ${util_src} src/python/DotProductHasher.cxx)
TARGET_LINK_LIBRARIES(py_dot_product_hasher ${Boost_LIBRARIES})


#set_property(TARGET gtest PROPERTY CXX_STANDARD 11)
#set_property(TARGET gtest PROPERTY CXX_STANDARD_REQUIRED ON)

#set_property(TARGET example1 PROPERTY CXX_STANDARD 11)
#set_property(TARGET example1 PROPERTY CXX_STANDARD_REQUIRED ON)

#set_property(TARGET dot_product_hasher PROPERTY CXX_STANDARD 11)
#set_property(TARGET dot_product_hasher PROPERTY CXX_STANDARD_REQUIRED ON)

#configure_file(cmake/dotProductHashingConfig.cmake
#  "${CMAKE_CURRENT_BINARY_DIR}/dotProductHashingConfig.cmake"
#  COPYONLY
#)

#set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" )
#configure_file(dotProductHashingConfig.cmake.in
#  "${PROJECT_BINARY_DIR}/dotProductHashingConfig.cmake")

set ( the_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include" )
#set ( dotProductHashing_LIBRARIES "$<TARGET_FILE:dot_product_hasher>" )
include ( CMakePackageConfigHelpers )
# We write in the 'export' folder in order not to collide with
# the previous config file:
configure_package_config_file ( 
       "${CMAKE_SOURCE_DIR}/cmake/dotProductHashingConfig.cmake.in" 
       "${CMAKE_BINARY_DIR}/dotProductHashingConfig.cmake"
       INSTALL_DESTINATION "${CMAKE_BINARY_DIR}"
       PATH_VARS the_INCLUDE_DIRS )


export ( TARGETS dot_product_hasher FILE "${CMAKE_BINARY_DIR}/dot_product_hasherTargets.cmake" )

export(PACKAGE dotProductHashing)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

#set(DotProductHashing_VERSION 0.1)

#include(GenerateExportHeader)

#generate_export_header(dot_product_hasher)
#set_property(TARGET dot_product_hasher PROPERTY VERSION ${DotProductHashing_VERSION})
#set_property(TARGET dot_product_hasher PROPERTY SOVERSION 0)
#set_property(TARGET dot_product_hasher PROPERTY
#  INTERFACE_dot_product_hasher_MAJOR_VERSION 0)
#set_property(TARGET dot_product_hasher APPEND PROPERTY
#  COMPATIBLE_INTERFACE_STRING dot_product_hasher_MAJOR_VERSION
#)

#install(TARGETS dot_product_hasher EXPORT dot_product_hasherTargets
#  LIBRARY DESTINATION lib
#  ARCHIVE DESTINATION lib
#  RUNTIME DESTINATION bin
#  INCLUDES DESTINATION include
#)
#install(
#  FILES
#    include/DotProductHasher.hpp
#    "${CMAKE_CURRENT_BINARY_DIR}/dot_product_hasher_export.h"
#  DESTINATION
#    include
#  COMPONENT
#    Devel
#)

#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(
#  "${CMAKE_CURRENT_BINARY_DIR}/dot_product_hasher/dot_product_hasherConfigVersion.cmake"
#  VERSION ${DotProductHashing_VERSION}
#  COMPATIBILITY AnyNewerVersion
#)

#export(EXPORT dot_product_hasherTargets
#  FILE "${CMAKE_CURRENT_BINARY_DIR}/dot_product_hasher/dot_product_hasherTargets.cmake"
#  NAMESPACE dot_product_hasher::
#)

#configure_file(cmake/dot_product_hasherConfig.cmake
#  "${CMAKE_CURRENT_BINARY_DIR}/dot_product_hasher/dot_product_hasherConfig.cmake"
#  COPYONLY
#)

#set(ConfigPackageLocation lib/cmake/dot_product_hasher)
#install(EXPORT dot_product_hasherTargets
#  FILE
#    dot_product_hasherTargets.cmake
#  NAMESPACE
#    dot_product_hasher::
#  DESTINATION
#    ${ConfigPackageLocation}
#)
#install(
#  FILES
#    cmake/dot_product_hasherConfig.cmake
#    "${CMAKE_CURRENT_BINARY_DIR}/dot_product_hasher/dot_product_hasherConfigVersion.cmake"
#  DESTINATION
#    ${ConfigPackageLocation}
#  COMPONENT
#    Devel
#)

