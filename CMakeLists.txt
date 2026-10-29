cmake_minimum_required(VERSION 3.10)
project(baddoom C CXX)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(lib/glfw-3.4)
add_subdirectory(lib/glad)
add_subdirectory(lib/assimp)

add_executable(baddoom
	src/main.cpp
	src/glboilerplate.cpp
	src/model.cpp
	src/physics.cpp
	src/profile.cpp
)

target_include_directories(baddoom PRIVATE
	./include
	lib/glfw-3.4/include
	lib/glm
	lib/stb-master/include
	lib/assimp/include
	lib/tracy
)

target_compile_definitions(baddoom PRIVATE TRACY_ENABLE)

target_link_libraries(baddoom PRIVATE
	glfw
	glad
	assimp
	ws2_32 dbghelp secur32
)