include_directories(${PROJECT_INCLUDE_DIR})


file(GLOB HEADERS ${PROJECT_INCLUDE_DIR} *.h)
file(GLOB SOURCES . *.cpp ${PROJECT_INCLUDE_DIR}/*.h)

# add_dependencies(opomo gravity opt_parser)

file(GLOB Power_SOURCES ${PROJECT_SOURCE_DIR}/models/*.cpp)
file(GLOB ACOPF_SOURCES ${PROJECT_SOURCE_DIR}/models/ACOPF/*.cpp)
file(GLOB SOCOPF_SOURCES ${PROJECT_SOURCE_DIR}/models/SOCOPF/*.cpp)
file(GLOB DCOPF_SOURCES ${PROJECT_SOURCE_DIR}/models/DCOPF/*.cpp)

add_executable(acopf ${Power_SOURCES} ${ACOPF_SOURCES})
add_executable(socopf ${Power_SOURCES} ${SOCOPF_SOURCES})
add_executable(dcopf ${Power_SOURCES} ${DCOPF_SOURCES})
#message(${IPOPT_LIBRARIES})
target_link_libraries(acopf libgravity.a liboption_parser.a ${IPOPT_LIBRARIES})
target_link_libraries(socopf libgravity.a liboption_parser.a ${IPOPT_LIBRARIES})
target_link_libraries(dcopf libgravity.a liboption_parser.a ${IPOPT_LIBRARIES})
add_test(NAME acopf_test COMMAND acopf)



