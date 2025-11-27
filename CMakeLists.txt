cmake_minimum_required(VERSION 3.10)
project(projet_mnt)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PROJ REQUIRED proj)

add_executable(projet_mnt
    src/main.cpp
    src/terraindata.cpp
    src/projector.cpp
    src/terrainprojected.cpp
)

target_include_directories(projet_mnt PUBLIC
    ${PROJ_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(projet_mnt PUBLIC
    ${PROJ_LIBRARIES}
)
