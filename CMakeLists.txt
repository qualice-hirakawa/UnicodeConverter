# Copyright (c) Hirakawa.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.20)

project (UnicodeConverter
  VERSION 1.0
  DESCRIPTION "UnicodeConverter Library"
  LANGUAGES CXX)

add_library(UnicodeConverter STATIC
  include/UnicodeConverter.hpp
  src/UnicodeConverter.cpp
)

target_include_directories(UnicodeConverter
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(UnicodeConverter)
