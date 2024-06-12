cmake_minimum_required(VERSION 3.6)
project(MagnumRenderer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/executable/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/executable/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/executable/${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MagnumRender/*
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/HMath/include
)

find_package(Magnum REQUIRED
    GL
    MeshTools
    Primitives
    SceneGraph
    Shaders
    Trade
    Text
    Sdl2Application
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Magnum::Application
    Magnum::GL
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::Shaders
    Magnum::Trade
    Magnum::Text
    Magnum::SceneGraph
    Magnum::Sdl2Application
)

file(GLOB_RECURSE EXECUTABLE_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_executable(TestMagnumRenderer ${EXECUTABLE_SOURCE_FILES})
target_link_libraries(TestMagnumRenderer PRIVATE ${PROJECT_NAME})
target_link_libraries(TestMagnumRenderer PRIVATE
    Magnum::Application
    Magnum::GL
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::Shaders
    Magnum::Trade
    Magnum::Text
    Magnum::SceneGraph
    Magnum::Sdl2Application
)