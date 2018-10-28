#[=======================================================================[.rst:
CheckIncludeFile
----------------

Provides a macro to check if a header file can be included in ``C``.

.. command:: CHECK_TERMIOS_API

  ::

    CHECK_TERMIOS_API(<variable>)

  Check termios api and store the result in an internal cache entry named
  ``<variable>``.

#]=======================================================================]

include_guard(GLOBAL)

macro(CHECK_TERMIOS_API VARIABLE)
  if(MSVC)
    set(${VARIABLE} "" CACHE INTERNAL "Have Termios API")
  else()
    include(CheckIncludeFile)
    CHECK_INCLUDE_FILE(termios.h ${VARIABLE})
    if(${VARIABLE})
      set(${VARIABLE} 1 CACHE INTERNAL "Have Termios API")
    else()
      set(${VARIABLE} "" CACHE INTERNAL "Have Termios API")
    endif()
  endif()
endmacro()
