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

add_executable(gtest ${core_SRC} ${util_src} ${test_SRC} ${gtest_SRC} "gtest-1.7.0/fused-src/gtest/gtest-all.cc" "gtest-1.7.0/fused-src/gtest/gtest_main.cc")
TARGET_LINK_LIBRARIES(gtest ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

ADD_LIBRARY(dot_product_hasher STATIC ${core_SRC} ${util_src} src/python/DotProductHasher.cxx)
IF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
SET_TARGET_PROPERTIES(dot_product_hasher PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
TARGET_LINK_LIBRARIES(dot_product_hasher ${Boost_LIBRARIES})

ADD_LIBRARY(py_dot_product_hasher SHARED ${core_SRC} ${util_src} src/python/DotProductHasher.cxx)
TARGET_LINK_LIBRARIES(py_dot_product_hasher ${Boost_LIBRARIES})

set ( the_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include" )
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
