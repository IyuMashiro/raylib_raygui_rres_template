#设定参与编译的文件集合,以下提供三种方案
#set(SRC sol.cpp)
#file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)


#设定可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设定头文件路径
#include_directories(${PROJECT_SOURCE_DIR}/include/raylib)
#include_directories(${PROJECT_SOURCE_DIR}/include/raygui)
#include_directories(${PROJECT_SOURCE_DIR}/include/rres)



#设定库路径
#link_directories(${PROJECT_SOURCE_DIR}/lib)
#link_directories(${PROJECT_SOURCE_DIR}/lib/raylib)

#设置库输出路径宏
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
  endif()
endif()

set(RAYGUI_VERSION 4.0)
find_package(raygui ${RAYGUI_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raygui_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raygui
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raygui/archive/refs/heads/master.zip
  )
  FetchContent_GetProperties(raygui)
  if (NOT raygui_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raygui)
    set(BUILD_RAYGUI_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raygui_SOURCE_DIR}/projects/CMake ${raygui_BINARY_DIR})
  endif()
endif()

set(RRES_VERSION 1.2.0)
find_package(rres ${RRES_VERSION} QUIET) # QUIET or REQUIRED
if (NOT rres_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    rres
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/rres/archive/refs/heads/master.zip
  )
  FetchContent_GetProperties(rres)
  if (NOT rres_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(rres)
    set(BUILD_RRES_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${rres_SOURCE_DIR}/projects/CMake ${rres_BINARY_DIR})
  endif()
endif()

option(SUPPORT_FILEFORMAT_WAV "WAV Support" TRUE)
option(SUPPORT_FILEFORMAT_OGG "OGG Support" TRUE)
option(SUPPORT_FILEFORMAT_MP3 "MP3 Support" TRUE)
option(SUPPORT_FILEFORMAT_QOA "QOA Support" TRUE)
option(SUPPORT_FILEFORMAT_FLAC "FLAC Support" TRUE)
option(SUPPORT_FILEFORMAT_XM "XM Support" TRUE)
option(SUPPORT_FILEFORMAT_MOD "MOD Support" TRUE)

# Our Project
add_executable(${PROJECT_NAME} ${SRC})
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib raygui rres)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
