################################################################################
#
#                         FindSymbol - v1.0
#                     (c) 2015 Fabian Thüring         
#
# This file is distributed under the MIT Open Source License. See 
# LICENSE.TXT for details.
#
################################################################################

cmake_minimum_required(VERSION 2.8)
project(findsymbol)

if(CMAKE_MAJOR_VERSION GREATER 2)
    cmake_policy(SET CMP0054 OLD)
endif()

################################################################################
# Compiler Flags
################################################################################
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-std=c++11 -march=native -Wno-sign-compare")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")
    
    if(FS_USE_CCACHE)
        set(CMAKE_CXX_COMPILER_ARG1 ${CMAKE_CXX_COMPILER})
        set(CMAKE_CXX_COMPILER "ccache")
    endif(FS_USE_CCACHE)
    
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-std=c++11 -march=native -Wno-sign-compare")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")
    
    if(FS_USE_CCACHE)
        set(CMAKE_CXX_COMPILER_ARG1 ${CMAKE_CXX_COMPILER})
        set(CMAKE_CXX_COMPILER "ccache")
        set(CCACHE_CXX_FLAGS "-Qunused-arguments -fcolor-diagnostics")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CCACHE_CXX_FLAGS}")
    endif(FS_USE_CCACHE)   
    
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "-std=c++11 -march=native -Wno-sign-compare")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -DNDEBUG")
    
    if(FS_USE_CCACHE)
        set(CMAKE_CXX_COMPILER_ARG1 ${CMAKE_CXX_COMPILER})
        set(CMAKE_CXX_COMPILER "ccache")
    endif(FS_USE_CCACHE)     
endif()

if(FS_DEBUG_LOG)
    add_definitions(-DFINDSYMBOL_LOG_ON)
endif()

################################################################################
# CMake options
################################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# Inlcude all Find*.cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Profile with gprof
if(FS_PROFILE_GPROF)
    if(NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
       message(FATAL_ERROR "Profiling with gprof only works with GNU compilers")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
    find_program(GPROF "gprof")
    if(GPROF)
        add_custom_target(gprof 
            COMMAND ${GPROF} findsymbol gmon.out > analysis.txt
            DEPENDS findsymbol
            COMMENT "Converting the profile data 'gmon.out' to 'analysis.txt'")
    else()
        message(FATAL_ERROR "GNU gprof command not FOUND")
    endif()
endif(FS_PROFILE_GPROF)

################################################################################
# Find LLVM
################################################################################
find_package(LLVM 3.6 REQUIRED all-targets) 
if(LLVM_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXXFLAGS}")
    include_directories(${LLVM_INCLUDE_DIRS})
    link_directories(${LLVM_LIBRARY_DIRS})
endif(LLVM_FOUND)

if(FS_PROFILE_GPROF)
    string(REPLACE "-fomit-frame-pointer" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif(FS_PROFILE_GPROF)

################################################################################
# Find TBB
################################################################################
find_package(TBB REQUIRED)
if(TBB_FOUND)
    include_directories(${TBB_INCLUDE_DIRS})
    link_directories(${TBB_LIBRARY_DIRS})
endif()

################################################################################
# Find LibIberty
################################################################################
find_package(LibIberty REQUIRED)
if(LibIberty_FOUND)
    include_directories(${LibIberty_INCLUDE_DIRS})
endif()

################################################################################
# Find PugiXML
################################################################################
find_package(PugiXML REQUIRED)
if(PUGIXML_FOUND)
    add_definitions(-DPUGIXML_NO_EXCEPTIONS)
    include_directories(${PUGIXML_INCLUDE_DIR})
endif()

################################################################################
# Compilation
################################################################################
include_directories(${findsymbol_SOURCE_DIR}/include)

add_subdirectory(lib/Support)
add_subdirectory(lib/Demangle)
add_subdirectory(lib/ObjectDump)
add_subdirectory(lib/Driver)
add_subdirectory(lib/FindSymbol)

################################################################################
# Installation
################################################################################

# The configuration file should be modifiable by everyone
set(CONFIG_INSTALL_DIR "$ENV{HOME}/.findsymbol")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/config.xml 
        DESTINATION ${CONFIG_INSTALL_DIR}
        PERMISSIONS OWNER_WRITE OWNER_READ
                    GROUP_WRITE GROUP_READ
                    WORLD_WRITE WORLD_READ)
  
# Install bash completion script
if(FS_INSTALL_BASH)      
    set(BASH_COMPLETION_INSTALL_DIR "/etc/bash_completion.d")
    if(FS_INSTALL_BASH_DIR)
        set(BASH_COMPLETION_INSTALL_DIR "${FS_INSTALL_BASH_DIR}")
    endif()
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config/findsymbol_completion 
            DESTINATION ${BASH_COMPLETION_INSTALL_DIR})
endif()
