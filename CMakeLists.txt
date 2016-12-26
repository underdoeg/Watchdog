cmake_minimum_required(VERSION 3.0)

project(Watchdog)

option(WITH_GTK_GUI "Build with GTK GUI")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


FIND_PACKAGE(Config++ REQUIRED)

FILE(GLOB_RECURSE LIB_SRC_LIST "libs/*")

FILE(GLOB_RECURSE SRC_LIST "src/*.h" "src/*.cpp")

include_directories(${CONFIG++_INCLUDE_DIR} libs/ ${Boost_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC_LIST} ${LIB_SRC_LIST})

target_link_libraries(${PROJECT_NAME} pthread ${CONFIG++_LIBRARY})

#find_package(Boost COMPONENTS system filesystem REQUIRED)
#link_directories(${PROJECT_NAME} ${Boost_LIBRARY_DIRS})
#target_link_libraries(${PROJECT_NAME} pthread ${CONFIG++_LIBRARY} ${Boost_LIBRARIES})
