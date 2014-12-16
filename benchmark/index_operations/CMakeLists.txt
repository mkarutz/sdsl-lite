cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0015 NEW)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

## Check dependencies
find_program(GO_EXEC go)
if(GO_EXEC)
	message(STATUS "GO environment found at ${GO_EXEC}")
else(GO_EXEC)
	message( FATAL_ERROR "Cannot find GO environment. Please install GO." )
endif(GO_EXEC)

## Project information ##
project(index_operations CXX C)
set(CMAKE_BUILD_TYPE "Release")

## Download test files if they don't exist
execute_process(COMMAND go run tools/glob_test_files.go OUTPUT_VARIABLE TEST_FILES)
execute_process(COMMAND go run tools/download_files.go WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

### create necessary dirs
file(MAKE_DIRECTORY indexes)
file(MAKE_DIRECTORY bin)
file(MAKE_DIRECTORY results)

## Setup include and lib directories from Make.helper
file(STRINGS "../../Make.helper" MAKEHELPER)
foreach(keyvalue ${MAKEHELPER})
	string(REGEX REPLACE "^[ ]+" "" keyvalue ${keyvalue})
	string(REGEX MATCH "^[^=]+" key ${keyvalue})
	string(REGEX REPLACE " " "" keystripped ${key})
	string(REPLACE "${key}=" "" value ${keyvalue})
	string(REPLACE "$(CODE_COVER)" "" value ${value})
	string(STRIP ${value} value)
	set(${keystripped} "${value}")
endforeach(keyvalue)
include_directories(${INC_DIR})
link_directories(${LIB_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CXX_FLAGS} ${MY_CXX_OPT_FLAGS}")

## add custom include/lib paths
include_directories("../../external/FastPFor/headers")
link_directories("../../build/external/FastPFor")

## Build binary targets
execute_process(
	COMMAND go run tools/generate_build_targets.go WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
set(BUILD_BINARIES "")
include(./config/indexes/parsed_index_configs.cmake)
set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/config/indexes/parsed_index_configs.cmake")

## build indexes first
add_custom_target(build-indexes ALL
	COMMAND go run tools/build_indexes.go
	DEPENDS ${BINARIES} ${TEST_FILES}
)
set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/bin/*.x ${CMAKE_CURRENT_SOURCE_DIR}/indexes/*.idx")

## Run the benchmarks
add_custom_target(run-benchmark ALL
	COMMAND go run tools/run_benchmark.go ${INDEX_NAMES}
	DEPENDS ${TEST_FILES}
)
add_dependencies(run-benchmark build-indexes)

## Visualize results
include(FindR)
if(R_EXEC) 
	message(STATUS "R environment found at ${R_EXEC}")
	find_or_install_r_package(ggplot2)
	find_or_install_r_package(scales)
	find_or_install_r_package(grid)
	add_custom_target(visualize-results ALL
		COMMAND go run tools/visualize_results.go
		DEPENDS run-benchmark
	)
	add_dependencies(visualize-results run-benchmark)
else(R_EXEC)
	message( SEND_ERROR "R not found. No graphs will be produced." )
endif(R_EXEC)


## add 'distclean' target to cleanup everything!
IF (UNIX)
  ADD_CUSTOM_TARGET (distclean @echo Cleanup up all generated files)
  SET(DISTCLEANED
   config/indexes/parsed_index_configs.cmake
   cmake.depends
   cmake.check_depends
   CMakeCache.txt
   CMakeFiles
   cmake.check_cache
   indexes/*.idx
   bin/*.x
   *.cmake
   Makefile
   core core.*
   gmon.out
   *~
  )
  
  ADD_CUSTOM_COMMAND(
    DEPENDS clean
    COMMAND rm
    ARGS    -Rf CMakeTmp ${DISTCLEANED}
    TARGET  distclean
  )
ENDIF(UNIX)