get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME})
cmake_minimum_required(VERSION 3.0)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wold-style-cast -Wextra -Wundef -Wsign-compare -Wconversion -Wpointer-arith -Weffc++ -Wnon-virtual-dtor")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE PROJECT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(SRC_LIST ${PROJECT_HEADERS} ${PROJECT_SOURCES})

file(GLOB_RECURSE RESOURCES    "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")
#file(GLOB_RECURSE TRANSLATIONS "${CMAKE_CURRENT_SOURCE_DIR}/*.ts")

aux_source_directory(. SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${RESOURCES})

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

# copy config
file(GLOB JSON_CONFIGS "${CMAKE_SOURCE_DIR}/*.json")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${JSON_CONFIGS} ${CMAKE_CURRENT_BINARY_DIR})
