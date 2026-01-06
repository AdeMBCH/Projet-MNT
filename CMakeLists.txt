cmake_minimum_required(VERSION 3.10)
project(projet_mnt)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PROJ REQUIRED proj)

add_executable(create_raster
    src/main.cpp
    src/terraindata.cpp
    src/projector.cpp
    src/terrainprojected.cpp
    src/mesh2D.cpp
    src/grid.cpp
    src/trianglelocator.cpp
    src/rasterise.cpp
    src/ppm.cpp
    src/ombrage.cpp
    src/colormap.cpp
    src/fourier.cpp

)

target_include_directories(create_raster PUBLIC
    ${PROJ_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(create_raster PUBLIC
    ${PROJ_LIBRARIES}
)

target_compile_definitions(create_raster PRIVATE
    RESOURCES_DIR="${CMAKE_SOURCE_DIR}/resources"
)
