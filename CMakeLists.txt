cmake_minimum_required(VERSION 3.1)
enable_language(C)
set(CMAKE_C_STANDARD 11) # C11...
set(CMAKE_C_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_C_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
# 1. INTERFACE_INCLUDE_DIRECTORIES にソースディレクトリとビルドディレクトリを自動追加する。
project(consoleapp)

set(consoleapp_src
    src/option.c
    src/prompt.c
)
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
# 2. ライブラリのビルド指定
add_library(consoleapp STATIC ${consoleapp_src})
# 3. インストールするヘッダファイルを指定しておく。
# 指定したファイルは、INSTALL(TARGETS) の PUBLIC_HEADER で指定した
# ディレクトリにインストールされる。
set_target_properties(consoleapp PROPERTIES
    PUBLIC_HEADER "src/option.h;src/prompt.h;src/consoleapp.h"
)
# 5. ターゲットのインストール指定
install(TARGETS consoleapp
    EXPORT consoleapp-export                         # 5.a EXPORT オプションで エクスポート名を指定する
    LIBRARY        DESTINATION lib            # 5.b ライブラリのインストール先
    ARCHIVE        DESTINATION lib
    INCLUDES       DESTINATION include        # 5.c エクスポート時にincludeパスに含めるパスを指定する
    PUBLIC_HEADER  DESTINATION include/consoleapp)   # ヘッダのインストール先

# 6. consoleapp-config.cmake を自動生成してインストールする。
install(EXPORT consoleapp-export                     # 6.a
    FILE consoleapp-config.cmake                     # 6.b ファイル名を指定する
    DESTINATION share/cmake/consoleapp/              # 6.c インストール先
    EXPORT_LINK_INTERFACE_LIBRARIES)          # 6.d 同時にリンクすべきライブラリをエクスポートする

add_subdirectory(sample)
