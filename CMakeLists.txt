cmake_minimum_required(VERSION 2.8.11)

project(solar-system)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set qt specific options
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# set c++ 14
set(CMAKE_CXX_STANDARD 14)

find_package(Qt5Widgets)
find_package(Qt5OpenGL)
find_package(Qt5Core)
find_package(OpenGL)
find_package(assimp)

# Source Files
FILE(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
# Header files
FILE(GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/src/*.h")

INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src")

# Libraries
set(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})
set(LIBRARIES ${LIBRARIES} ${OPENGL_LIBRARIES})
set(LIBRARIES ${LIBRARIES} ${ASSIMP_LIBRARIES})

add_executable(solar-system ${SOURCE_FILES} ${HEADER_FILES})
qt5_use_modules(solar-system Widgets OpenGL)
target_link_libraries(solar-system ${LIBRARIES})
