cmake_minimum_required(VERSION 3.10)

project(Bolo VERSION 1.90.0)

set(CMAKE_C_STANDARD 11)
set(C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig)
pkg_check_modules(MINIZIP REQUIRED minizip)
pkg_check_modules(SDL REQUIRED sdl)
pkg_check_modules(GTK REQUIRED gtk+)

add_compile_options(-g -Wall -Wno-missing-braces)

add_subdirectory(gui/linux)
add_subdirectory(server)
