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

list(APPEND GLOBAL_THIRDPARTY_LIB_ARGS "-DGRAVITY_ROOT_DIR:PATH=${GRAVITY_ROOT_DIR}")
set(GRAVITY_INCLUDE_DIRS ${THIRDPARTY_INSTALL_PATH}/Install/Gravity/include)
include_directories(${GRAVITY_INCLUDE_DIRS})
find_library(GRAVITY_LIBRARY
        libgravity.a
        HINTS ${GRAVITY_ROOT_DIR}/build/lib
)
set(LIBS ${LIBS} ${GRAVITY_LIBRARY})
