# Copyright (c) 2014, Siemens AG. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

project (EMBB)
cmake_minimum_required (VERSION 2.8.9)

# Version number
set (EMBB_BASE_VERSION_MAJOR 0)
set (EMBB_BASE_VERSION_MINOR 2)
set (EMBB_BASE_VERSION_PATCH 0)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING
  "Choose the type of build, options are: Debug Release
  RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

## Command line options
#
# The set option will be converted to uppercase letters by cmake!! --> ON/OFF
# Note that the help string (second argument) cannot be printed by cmake.
#
option(BUILD_TESTS "Specify whether tests should be built" ON)
option(BUILD_EXAMPLES "Specify whether examples should be built" OFF)
option(USE_EXCEPTIONS "Specify whether exceptions should be activated in C++" ON)
option(INSTALL_DOCS "Specify whether Doxygen docs should be installed" ON)
option(WARNINGS_ARE_ERRORS "Specify whether warnings should be treated as errors" OFF)

## LOCAL INSTALLATION OF SUBPROJECT BINARIES
#
include(CMakeCommon/CopyInstallFiles.cmake) # Needed in all subprojects
set(local_install_dir ${CMAKE_CURRENT_BINARY_DIR}/binaries) 

if (WARNINGS_ARE_ERRORS STREQUAL ON)
  message("-- Warnings are treated as errors")
  set(EMBB_USE_EXCEPTIONS 1)
else()
  message("-- Warnings are not treated as errors (default)")
endif()
message("   (set with command line option -DWARNINGS_ARE_ERRORS=ON/OFF)")

include(CMakeCommon/SetCompilerFlags.cmake)
SetGNUCompilerFlags(compiler_libs compiler_flags)
SetVisualStudioCompilerFlags(compiler_libs compiler_flags)

## Exception handling in C++
#
if (USE_EXCEPTIONS STREQUAL ON)
  message("-- Exceptions enabled (default) ")
  set(EMBB_USE_EXCEPTIONS 1)
else()
  message("-- Exceptions disabled")
  set(EMBB_NO_EXCEPTIONS) # A preprocessor define
  if (CMAKE_COMPILER_IS_GNUCXX)
    LIST(APPEND ${CMAKE_CXX_FLAGS} "-fno-exceptions -pthread")
  elseif (MSVC)
    LIST(FIND ${CMAKE_CXX_FLAGS} "/EHsc" EXCEPTION_FLAG)
    if (EXCEPTION_FLAG)
      string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_HAS_EXCEPTIONS=0")
  endif()
endif()
message("   (set with command line option -DUSE_EXCEPTIONS=ON/OFF)")

## Copy test execution script to local binaries folder
#   
if (DEFINED CYGWIN)
  set(test_script_in run_tests_cygwin.sh)
  set(test_script_out run_tests.sh)
elseif (DEFINED UNIX)
  set(test_script_in run_tests_linux.sh)
  set(test_script_out run_tests.sh)
else()
  set(test_script_in run_tests_windows.bat)
  set(test_script_out run_tests.bat)
endif()
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy ../scripts/${test_script_in} binaries/${test_script_out}
  )

## Test and Partest build
#
include(CMakeCommon/CheckEnableTests.cmake)
if (BUILD_TESTS STREQUAL ON)
  message("-- Building tests enabled (default)")
else()
  message("-- Building tests disabled")
endif()
message("   (set with command line option -DBUILD_TESTS=ON/OFF)")
CheckPartestInstall(${BUILD_TESTS} partest_includepath partest_libpath)

## SUBPROJECTS
#
add_subdirectory(base_c)
add_subdirectory(base_cpp)
add_subdirectory(mtapi_c)
add_subdirectory(mtapi_cpp)
add_subdirectory(containers_cpp)
add_subdirectory(algorithms_cpp)
add_subdirectory(dataflow_cpp)
if (BUILD_EXAMPLES STREQUAL ON)
  message("-- Building examples enabled")
  add_subdirectory(doc/examples)
else()
  message("-- Building examples disabled (default)")
endif()
message("   (set with command line option -DBUILD_EXAMPLES=ON/OFF)")

## INSTALLATION
#
include(CMakeCommon/SetInstallPaths.cmake)
SetInstallPaths()

## DOXYGEN 
#
include(CMakeCommon/CreateDoxygenDocumentationTarget.cmake)
CreateDoxygenDocumentationTarget()

if (INSTALL_DOCS STREQUAL ON)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc/
          DESTINATION ${INSTALL_PREFIX_DOCS} FILES_MATCHING PATTERN "*.*" PATTERN "CMakeLists.txt" EXCLUDE)
endif()
