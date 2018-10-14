cmake_minimum_required(VERSION 3.1)
enable_language(C)
set(CMAKE_C_STANDARD 11) # C11...
set(CMAKE_C_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_C_EXTENSIONS OFF) #...without compiler extensions like gnu++11
project(sample)
set(sample_src
    sample.c
)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)
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
add_executable(sample ${sample_src})
target_link_libraries(sample consoleapp ${Readline_LIBRARY})
