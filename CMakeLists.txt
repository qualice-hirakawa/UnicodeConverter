# Copyright (c) Hirakawa.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.20)

project (UnicodeConverter
  VERSION 1.0
  DESCRIPTION "UnicodeConverter Library"
  LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE include/UnicodeConverter.hpp)
set_target_properties(${PROJECT_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)
