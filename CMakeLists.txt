cmake_minimum_required(VERSION 3.16)
project(MovieTracker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Concurrent Svg Multimedia)

file(GLOB_RECURSE ALL_SOURCES
    src/*.cpp
    include/**/*.hpp
    external/tinyfiledialogs/*.cpp
)

# Separate main.cpp so the object library doesn't define main()
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

file(GLOB_RECURSE INCLUDE_DIRS LIST_DIRECTORIES true include/*)
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

foreach(DIR ${INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        include_directories(${DIR})
    endif()
endforeach()

set(QT_LIBS
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Concurrent
    Qt6::Svg
    Qt6::Multimedia
)

# Object library — compiled once, shared by the app and the test suite
add_library(MovieTrackerLib OBJECT ${ALL_SOURCES})
target_include_directories(MovieTrackerLib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${INCLUDE_DIRS}
)
target_link_libraries(MovieTrackerLib ${QT_LIBS})

option(SHIPPED "Build for app bundle deployment" OFF)
option(BUILD_TESTS "Build test suite" OFF)

add_executable(MovieTracker src/main.cpp $<TARGET_OBJECTS:MovieTrackerLib>)
target_include_directories(MovieTracker PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${INCLUDE_DIRS}
)
target_link_libraries(MovieTracker ${QT_LIBS})

if(SHIPPED)
    target_compile_definitions(MovieTracker PRIVATE SHIPPED)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
