cmake_minimum_required(VERSION 3.10)
project(project)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PROJ REQUIRED proj)

add_executable(projet_mnt
  src/main.cpp
)

target_include_directories(projet_mnt PUBLIC
  ${PROJ_INCLUDEDIR}/include
)

target_link_libraries(projet_mnt PUBLIC
  ${PROJ_LIBRARIES}
)
