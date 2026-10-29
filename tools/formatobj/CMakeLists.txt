cmake_minimum_required(VERSION 3.10)
project(formatobj C CXX)

set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)  # static zlib too
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(../../lib/assimp ../../lib/assimp/build)

add_executable(formatobj
	main.cpp
)

target_include_directories(formatobj PRIVATE
	../../lib/assimp/include
)

target_link_libraries(formatobj PRIVATE
	assimp
)