cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(ShowUnlistedLevelsInLevelCells VERSION 1.0.0)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    
    src/levelInfos.hpp
    src/levelInfos.cpp

    src/queueRequests.hpp
    src/queueRequests.cpp

    src/levelClass.hpp
    src/levelClass.cpp

    src/gdLevelSearch.hpp
    src/gdLevelSearch.cpp

    src/utils.hpp
    src/utils.cpp

    src/settings/resetCurrentQueue.cpp
    src/settings/resetSavedInfos.cpp
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# Set up dependencies, resources, and link Geode.
setup_geode_mod(${PROJECT_NAME})
