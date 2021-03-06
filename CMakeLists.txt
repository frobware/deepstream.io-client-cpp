cmake_minimum_required(VERSION 2.8.12)

if(NOT BUILD_TESTING)
  set(BUILD_TESTING OFF CACHE STRING "Build tests. Options are OFF ON." FORCE)
endif()

if(NOT BUILD_COVERAGE)
  set(BUILD_COVERAGE OFF CACHE STRING "Build with --coverage; options are OFF ON." FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

project(deepstream.io-client-cpp)

enable_language(C)
enable_language(CXX)
include(CTest)
include(ExternalProject)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

# Put the libaries and binaries that get built into directories at the
# top of the build tree rather than in hard-to-find leaf directories.

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(CCACHE "Use ccache if available" ON)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE AND CCACHE_PROGRAM)
  message(STATUS "using ccache")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

# Use ld.gold if it is available and isn't explicitly disabled.
option(USE_LD_GOLD "Use GNU gold linker" ON)
if(USE_LD_GOLD AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -fuse-ld=gold -Wl,--version OUTPUT_VARIABLE stdout ERROR_QUIET)
  if("${stdout}" MATCHES "GNU gold")
    message(STATUS "matched GNU gold")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
  else()
    message(WARNING "GNU gold linker isn't available, using the default system linker.")
  endif()
endif()

# We absolutely depend on C++11 support from the toolchain; exit if
# the toolchain doesn't support it.

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

CHECK_C_COMPILER_FLAG("-std=c11" COMPILER_SUPPORTS_C11)
if(COMPILER_SUPPORTS_C11)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
else()
  message(FATAL_ERROR "Compiler ${CMAKE_C_COMPILER} has no C11 support.")
endif()

if(BUILD_TESTING)
  find_package(Boost 1.46 REQUIRED COMPONENTS unit_test_framework)
  enable_testing()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra -Wall -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -pedantic")

if(NOT DEFINED(CMAKE_BUILD_TYPE) OR "Debug" STREQUAL "${CMAKE_BUILD_TYPE}")
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_ASSERTIONS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG_PEDANTIC")
  endif()
endif()

if(BUILD_COVERAGE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif()

externalproject_add(poco
  PREFIX thirdparty
  URL https://github.com/pocoproject/poco/archive/poco-1.7.7-release.tar.gz
  URL_HASH MD5=247b97b545715dc38c8619e412fbcd96
  CMAKE_GENERATOR ${CMAKE_GENERATOR}
  CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
  CMAKE_ARGS
  "-DENABLE_CRYPTO=OFF"
  "-DENABLE_DATA=OFF"
  "-DENABLE_JSON=OFF"
  "-DENABLE_MONGODB=OFF"
  "-DENABLE_NETSSL=OFF"
  "-DENABLE_PAGECOMPILER=OFF"
  "-DENABLE_PAGECOMPILER_FILE2PAGE=OFF"
  "-DENABLE_UTIL=OFF"
  "-DENABLE_XML=OFF"
  "-DENABLE_ZIP=OFF"
  -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/thirdparty
  LOG_CONFIGURE ON
  UPDATE_COMMAND ""
)

find_package(FLEX 2.5 REQUIRED)

link_directories(${CMAKE_BINARY_DIR}/thirdparty/lib)
include_directories(${CMAKE_BINARY_DIR}/thirdparty/include)

include_directories("include")

add_subdirectory(doc)
add_subdirectory(include)
add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

message(STATUS "CMAKE_SYSTEM=${CMAKE_SYSTEM}")
message(STATUS "CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_GENERATOR=${CMAKE_GENERATOR}")
message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")

message(STATUS "BUILD_TESTING=${BUILD_TESTING}")
message(STATUS "BUILD_COVERAGE=${BUILD_COVERAGE}")
