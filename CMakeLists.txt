cmake_minimum_required(VERSION 3.16)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 17)

get_filename_component(source_dir ${CMAKE_CURRENT_LIST_DIR}/src ABSOLUTE CACHE)

file(GLOB_RECURSE source_files LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} CONFIGURE_DEPENDS "${source_dir}/*.c*")
file(GLOB_RECURSE header_files LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_LIST_DIR} CONFIGURE_DEPENDS "${source_dir}/*.h*")


add_executable(ThreadPool ${source_files} ${header_files})