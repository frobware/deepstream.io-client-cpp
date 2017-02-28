# We need to fix the generated Swig wrapper as the current version of
# Swig does not handle std::unique_ptr. Our fix directly calls
# std::move() for the few cases where this smart pointer type is
# exposed in the deepstream public API.

add_custom_command(
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/python/deepstreamPYTHON_wrap.cxx
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/python/deepstreamPYTHON_wrap_fixed.cxx
  COMMAND ${CMAKE_SOURCE_DIR}/fix-deepstream-swig.pl ${CMAKE_CURRENT_BINARY_DIR}/python/deepstreamPYTHON_wrap.cxx
  COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/python/deepstreamPYTHON_wrap_fixed.cxx)

include(${SWIG_USE_FILE})
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_SWIG_FLAGS "")
set(CMAKE_SWIG_OUTDIR python)
set_source_files_properties(${DEEPSTREAM_SWIG_INTERFACE} PROPERTIES CPLUSPLUS ON)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/python/deepstreamPYTHON_wrap.cxx PROPERTIES COMPILE_FLAGS -Wno-self-assign)
swig_add_module(deepstream python ${DEEPSTREAM_SWIG_INTERFACE} ${CMAKE_CURRENT_BINARY_DIR}/python/deepstreamPYTHON_wrap_fixed.cxx)
swig_link_libraries(deepstream libdeepstream ${POCO_LIBS} ${PYTHON_LIBRARIES})

add_test(
  NAME    python-swig-test
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/test/python-swig-test.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
