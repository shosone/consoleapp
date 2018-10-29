cmake_minimum_required(VERSION 3.1)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11) # C11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
project(cpp_sample)
set(option_cpp_sample_src
    option_sample.cpp
)
set(prompt_cpp_sample_src
    prompt_sample.cpp
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
add_executable(option_cpp_sample ${option_cpp_sample_src})
target_link_libraries(option_cpp_sample consoleapp ${Readline_LIBRARY})

CHECK_TERMIOS_API(HAVE_TERMIOS_H)
if(HAVE_TERMIOS_H)
  add_executable(prompt_cpp_sample ${prompt_cpp_sample_src})
  target_link_libraries(prompt_cpp_sample consoleapp ${Readline_LIBRARY})
endif()
