cmake_minimum_required(VERSION 3.4)
project(test_rbtree)

if(${PROJECT_BINARY_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
	message(FATAL_ERROR "DO NOT run cmake in source directory")
endif()

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

add_definitions(-DUNIT_TESTING)

include_directories(..)
link_libraries(cmockery)

set(SOURCE_FILES
	_test_common.c
	test_create_destroy.c
	test_remove.c
	test_insert.c
	test_enumeration.c
	test_rotating.c
	test_find.c
	test_main.c
	../rbtree.c
)

add_executable(test_rbtree ${SOURCE_FILES})
target_link_libraries(test_rbtree cmockery)

message("system name: ${CMAKE_SYSTEM_NAME}")
message("build type: ${CMAKE_BUILD_TYPE}")
