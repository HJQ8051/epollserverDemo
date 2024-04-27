cmake_minimum_required(VERSION 3.15)
project(EpollServer)

aux_source_directory(. SRC_LIST)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)
add_executable(test ${SRC_LIST})