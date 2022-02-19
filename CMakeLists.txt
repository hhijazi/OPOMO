cmake_minimum_required(VERSION 3.2)

if(WIN32)
 set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc")
 set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++")
endif()

project(OPOMO)

# if(CMAKE_XCODE_BUILD_SYSTEM VERSION_GREATER_EQUAL 12)
#   cmake_policy(SET CMP0114 NEW)
# else()
#   # This test is very noisy with warnings about this policy if we don't
#   # explicitly set it. Projects shouldn't do this, but for test code this
#   # is reasonable.
#   # cmake_policy(SET CMP0114 OLD)
# endif()


enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(MACOSX_RPATH 1) 
# Debug mode
option(Debug "Turn on debug mode" OFF)

# Coverage
option(Cov "Turn on coverage" OFF)

# Find IPOPT (optional)
# option(Ipopt "Link to IPOPT libraries" ON)

# Find CLP (optional)
option(Gravity "Link to Gravity libraries" ON)

# Find OpenMPI (optional)
option(OpenMPI "Link to OpenMPI libraries" OFF)

# Find OPT_PARSER (optional)
option(OPT_PARSER "Link to OPT_PARSER libraries" ON)

set(THIRDPARTY_INSTALL_PATH ${CMAKE_BINARY_DIR}/third_party)


#Ipopt
# if (Ipopt)
# message(STATUS "Enable IPOPT")
# add_definitions(-DUSE_IPOPT)
find_package(IPOPT QUIET)
if (IPOPT_FOUND)
 message("-- Found IPOPT: ${IPOPT_INCLUDE_DIRS}")
 include_directories(${IPOPT_INCLUDE_DIRS})
 set(LIBS ${LIBS} ${IPOPT_LIBRARIES})
else(IPOPT_FOUND)
# set(ADD_IPOPT TRUE)
if(APPLE)
 set(IPOPT_LIBRARIES ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/build/install/ipopt/build/libipopt.dylib)
elseif(UNIX)
	#set(IPOPT_LIBRARIES ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/thirdparty/Ipopt/libipopt.so ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/thirdparty/Ipopt/libsipopt.so)
	set(IPOPT_LIBRARIES ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/thirdparty/Ipopt/libipopt.so)
	message(${IPOPT_LIBRARIES})
	link_directories(${THIRDPARTY_INSTALL_PATH}/Install/Gravity/thirdparty/Ipopt)
# elseif(WIN32)
# set(IPOPT_LIBRARIES ${PROJECT_SOURCE_DIR}/thirdparty/Ipopt/libipopt.so)
endif(APPLE) 
include_directories("${THIRDPARTY_INSTALL_PATH}/Install/Gravity/thirdparty/Ipopt/include/coin-or")
set(LIBS ${LIBS} ${IPOPT_LIBRARIES})
#  include(ExternalIpopt)
endif (IPOPT_FOUND)
# endif()



if (OpenMPI)
    SET(CMAKE_CXX_COMPILER mpicxx)
    SET(CMAKE_C_COMPILER mpicc)
    message("Compilers used: ${CMAKE_C_COMPILER} and ${CMAKE_CXX_COMPILER}")
    message(STATUS "Enable OpenMPI")
    add_definitions(-DUSE_MPI)
endif()


set_directory_properties(PROPERTIES EP_BASE ${CMAKE_BINARY_DIR}/third_party)
get_directory_property(THIRDPARTY_BASE_PATH EP_BASE)



include(ExternalProject)
#GRAVITY
if (Gravity)
message(STATUS "Enable Gravity")
find_package(Gravity QUIET)
if (GRAVITY_FOUND)
 message("-- Found GRAVITY: ${GRAVITY_INCLUDE_DIRS}")
 include_directories(${GRAVITY_INCLUDE_DIRS})
 set(LIBS ${LIBS} ${GRAVITY_LIBRARIES})
else(GRAVITY_FOUND)
 set(ADD_GRAVITY TRUE)
 message(STATUS "Will download GRAVITY library from https://github.com/coin-or/Gravity")
 include(ExternalGravity)
 link_directories(${THIRDPARTY_INSTALL_PATH}/Install/Gravity/lib)
 include_directories(${THIRDPARTY_INSTALL_PATH}/Install/Gravity/include)
 if(APPLE)
  set(GRAVITY_LIBRARY ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/lib/libgravity.a)
 elseif(UNIX)
  set(GRAVITY_LIBRARY ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/lib/libgravity.so)
 endif(APPLE)
endif()
endif()


# cpp_option_parser
if (OPT_PARSER)
	set(OPT_PARSER_VERSION v1.0)
	message(STATUS "Enable OPT_PARSER")
	add_definitions(-DUSE_OPT_PARSER)
    link_directories(${THIRDPARTY_INSTALL_PATH}/Install/Gravity/build/third_party/Build/opt_parser)	
	set(OPT_PARSER_INCLUDE_DIR ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/build/third_party/Source/opt_parser)
	include_directories(${OPT_PARSER_INCLUDE_DIR})
endif()

# Inlude dir
include_directories(${PROJECT_SOURCE_DIR}/third_party)

# Include Power dirs
include_directories(${PROJECT_SOURCE_DIR}/models)
include_directories(${PROJECT_SOURCE_DIR}/include)


set(LIBS ${LIBS} ${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/lib/Release ${PROJECT_SOURCE_DIR}/lib/Debug)
# Compiler options
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast")

# Build options
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/Release)
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

set(CMAKE_BUILD_TYPE "Release")
if (Debug)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/Debug)
endif()

if(Cov)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fprofile-arcs -lgcov")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-absolute-value -Wno-deprecated-copy -Wno-non-pod-varargs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'\$ORIGIN/lib'")


if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -pthread")
endif()


add_subdirectory(models)

#Get user defined configuration including passing project source directory into c++ code
configure_file(OPOMOConfig.h.in ${PROJECT_SOURCE_DIR}/include/OPOMOConfig.h)
