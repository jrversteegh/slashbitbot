# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Define board root to find our custom shield
set(BOARD_ROOT ${CMAKE_SOURCE_DIR})

# Default board to microbit v2
if (NOT DEFINED ${BOARD})
  set(BOARD bbc_microbit_v2)
endif()

set(SHIELD kitronik_crb_5693)


find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(slashbitbot)

FILE(GLOB app_sources src/*.cpp)
target_sources(app PRIVATE ${app_sources})
