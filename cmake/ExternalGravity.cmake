# Create download URL derived from version number.
set(GRAVITY_DOWNLOAD_URL https://github.com/coin-or/Gravity.git)

# Download and build the GRAVITY library and add its properties to the third party arguments.
set(GRAVITY_ROOT_DIR ${THIRDPARTY_INSTALL_PATH}/Install/Gravity CACHE INTERNAL "")
ExternalProject_Add(gravity
    DOWNLOAD_DIR ${THIRDPARTY_INSTALL_PATH}
    DOWNLOAD_COMMAND export HTTPS_PROXY=$ENV{HTTPS_PROXY} && git clone -b ACOPF --single-branch ${GRAVITY_DOWNLOAD_URL} && rm -fr ./Install/Gravity && mv Gravity ./Install/Gravity && cd ./Install/Gravity && mkdir build && cd build && cmake -DMP=OFF -DCMAKE_CXX_FLAGS="-Wno-non-pod-varargs" .. && make gravity -j
    URL ${MP_DOWNLOAD_URL}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${MP_ROOT_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
if(WIN32)
set(IPOPT_ROOT_DIR ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/thirdparty/Ipopt CACHE INTERNAL "")
add_custom_command(
  TARGET ipopt POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy 
  ${IPOPT_ROOT_DIR}/libipopt-3.dll ${PROJECT_SOURCE_DIR}/bin/Release/libipopt-3.dll)

add_custom_command(
TARGET ipopt POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy 
${IPOPT_ROOT_DIR}/libblas.dll ${PROJECT_SOURCE_DIR}/bin/Release/libblas.dll)

add_custom_command(
TARGET ipopt POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy 
${IPOPT_ROOT_DIR}/liblapack.dll ${PROJECT_SOURCE_DIR}/bin/Release/liblapack.dll)

add_custom_command(
TARGET ipopt POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy 
${IPOPT_ROOT_DIR}/libgfortran-5.dll ${PROJECT_SOURCE_DIR}/bin/Release/libgfortran-5.dll)
endif()

list(APPEND GLOBAL_THIRDPARTY_LIB_ARGS "-DGRAVITY_ROOT_DIR:PATH=${GRAVITY_ROOT_DIR}")
set(GRAVITY_INCLUDE_DIRS ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/include)
include_directories(${GRAVITY_INCLUDE_DIRS})
find_library(GRAVITY_LIBRARY
        libgravity.a
        HINTS ${GRAVITY_ROOT_DIR}/build/lib
)
set(LIBS ${LIBS} ${GRAVITY_LIBRARY})
