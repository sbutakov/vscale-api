cmake_minimum_required(VERSION 3.6)
set(LIBRARY_NAME vscale)
project(vscale)

set(PROJECT_VERSION_MAJOR 0.0.1)
set(PROJECT_VERSION_MINOR 0.1)
set(LIBRARY_INSTALL_PATH /usr/local/lib)
set(HEADERS_INSTALL_PATH /usr/local/include)
link_directories( "${LIBRARY_INSTALL_PATH}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2 -Wall -pedantic -pedantic-errors")
set(SOURCE_FILES src/vscale.cpp)
include_directories(include)

add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${LIBRARY_NAME} curl jsoncpp)

set_target_properties(${LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION_MAJOR} SOVERSION ${PROJECT_VERSION_MINOR})
install(TARGETS ${LIBRARY_NAME} DESTINATION ${LIBRARY_INSTALL_PATH})
install( DIRECTORY include/ DESTINATION ${HEADERS_INSTALL_PATH} FILES_MATCHING PATTERN "*.h" )