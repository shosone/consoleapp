﻿cmake_minimum_required(VERSION 3.1)
enable_language(C)
set(CMAKE_C_STANDARD 11) # C11...
set(CMAKE_C_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_C_EXTENSIONS OFF) #...without compiler extensions like gnu++11
project(c_sample)
set(option_c_sample_src
    option_sample.c
)
set(prompt_c_sample_src
    prompt_sample.c
)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
include(CheckTermiosAPI)
include(FindReadline)
find_package(Readline)
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_C_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUC)
  # Update if necessary
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-long-long -pedantic")
endif()
include_directories(${Readline_INCLUDE_DIR})
add_executable(option_c_sample ${option_c_sample_src})
target_link_libraries(option_c_sample consoleapp ${Readline_LIBRARY})

CHECK_TERMIOS_API(HAVE_TERMIOS_H)
if(HAVE_TERMIOS_H)
  add_executable(prompt_c_sample ${prompt_c_sample_src})
  target_link_libraries(prompt_c_sample consoleapp ${Readline_LIBRARY})
endif()
