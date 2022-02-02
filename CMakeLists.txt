include(FetchContent)
# set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Disable Updates")

cmake_minimum_required(VERSION 3.18.0)
project(cpp_sudoku_solver VERSION 0.0.1)

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE Generator: ${CMAKE_GENERATOR}")
message(STATUS "Build Test: ${PLL_ENABLE_BUILD_TESTING}")

if (NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Wextra -Wno-unused-function -Wno-unused-parameter")
endif()

FetchContent_Declare(
  minisat
  GIT_REPOSITORY https://github.com/master-keying/minisat.git
  GIT_TAG master
)

FetchContent_MakeAvailable(minisat)

file(GLOB_RECURSE CPP_SUDOKU_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} "src/*.cpp")

add_executable(cpp_sudoku_solver ${CPP_SUDOKU_SOURCES})
target_link_libraries(cpp_sudoku_solver MiniSat::libminisat)
target_include_directories(cpp_sudoku_solver PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(cpp_sudoku_solver PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)
