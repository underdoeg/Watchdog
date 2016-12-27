cmake_minimum_required(VERSION 3.0)

project(Watchdog)

option(WITH_GTK_GUI "Build with GTK GUI")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


find_package(Config++ REQUIRED)

file(GLOB_RECURSE LIB_SRC_LIST "libs/*")

file(GLOB SRC_LIST "src/*.h" "src/*.cpp")

include_directories(${CONFIG++_INCLUDE_DIR} libs/ ${Boost_INCLUDE_DIRS})

#handle optional gtk gui
if(WITH_GTK_GUI)
    file(GLOB GTK_SRC_LIST "src/gtk/*.h" "src/gtk/*.cpp")
    list(APPEND SRC_LIST ${GTK_SRC_LIST})
    add_definitions(-DWITH_GTK_GUI)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

    include_directories(${GTKMM_INCLUDE_DIRS})
    link_directories(${GTKMM_LIBRARY_DIRS})
    link_libraries(${GTKMM_LIBRARIES})

endif()

link_libraries(pthread ${CONFIG++_LIBRARY})

#add boost
find_package(Boost COMPONENTS system filesystem REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})
link_libraries(pthread ${CONFIG++_LIBRARY} ${Boost_LIBRARIES})

add_executable(${PROJECT_NAME} ${SRC_LIST} ${LIB_SRC_LIST})
