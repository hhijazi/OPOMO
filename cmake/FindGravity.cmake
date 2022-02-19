set(GRAVITY_ROOT_DIR "$ENV{GRAVITY_ROOT_DIR}" CACHE PATH "GRAVITY root directory.")
message("Looking for GRAVITY in ${GRAVITY_ROOT_DIR}")


include(FindPackageHandleStandardArgs)

find_path(GRAVITY_INCLUDE_DIR
	NAMES model.h 
	HINTS ${GRAVITY_ROOT_DIR}/include/gravity
	HINTS ${PROJECT_SOURCE_DIR}/thirdparty/Gravity/include/gravity
)


if(WIN32)

find_library(GRAVITY_LIBRARY 
	libgravity.lib
	HINTS /usr/local/lib
	HINTS ${GRAVITY_ROOT_DIR}/lib
	HINTS ${PROJECT_SOURCE_DIR}/thirdparty/Gravity/lib
)

elseif(APPLE)
find_library(GRAVITY_LIBRARY 
	libgravity.dylib
	HINTS /usr/local/lib
	HINTS ${GRAVITY_ROOT_DIR}/lib
	HINTS ${PROJECT_SOURCE_DIR}/third_party/CoinGRAVITY/build/lib
)

elseif(UNIX)
find_library(GRAVITY_LIBRARY 
	libgravity.so
	HINTS /usr/local/lib
	HINTS ${GRAVITY_ROOT_DIR}/lib
	HINTS ${PROJECT_SOURCE_DIR}/third_party/CoinGRAVITY/build/lib
)
endif(WIN32)

find_package_handle_standard_args(Gravity DEFAULT_MSG GRAVITY_LIBRARY GRAVITY_INCLUDE_DIR)

if(GRAVITY_FOUND)
	message("—- Found GRAVITY include dir under ${GRAVITY_INCLUDE_DIR}")
	message("—- Found GRAVITY lib at ${GRAVITY_LIBRARY}")
    set(GRAVITY_INCLUDE_DIRS ${GRAVITY_INCLUDE_DIR})
    set(GRAVITY_LIBRARIES ${GRAVITY_LIBRARY})
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(GRAVITY_LIBRARIES "${GRAVITY_LIBRARIES};m;pthread")
    endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
else (GRAVITY_FOUND)
 message("Cannot find GRAVITY, will try pulling it from github.")
endif(GRAVITY_FOUND)

mark_as_advanced(GRAVITY_LIBRARY GRAVITY_INCLUDE_DIR)


