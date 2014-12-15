cmake_minimum_required(VERSION 2.8.11)
cmake_policy(SET CMP0015 NEW)

## Project information ##
project(index_operations CXX C)
set(CMAKE_BUILD_TYPE "Release")

## Download test files if they don't exist
execute_process(COMMAND go run tools/glob_test_files.go OUTPUT_VARIABLE TEST_FILES)
add_custom_command(
	OUTPUT ${TEST_FILES}
	COMMAND go run tools/download_files.go WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

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
message(${LIB_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CXX_FLAGS} ${MY_CXX_OPT_FLAGS}")

## add custom include/lib paths
include_directories("../../external/FastPFor/headers")
link_directories("../../build/external/FastPFor")

## Build binary targets
file(GLOB index_config_files "${CMAKE_CURRENT_SOURCE_DIR}/config/indexes/*.config")
set(INDEX_NAMES "")
foreach(index_type ${index_config_files})
	file(STRINGS ${index_type} CONFIG_CONTENT)
	set(compile_defs "")
	foreach(keyvalue ${CONFIG_CONTENT})
		string(REGEX REPLACE "^[ ]+" "" keyvalue ${keyvalue})
		string(REGEX MATCH "^[^=]+" key ${keyvalue})
		set(key_str "${key}_STR")
		string(REPLACE "${key}=" "" valuestr ${keyvalue})
		string(REPLACE "\"" "" value ${valuestr})
		set(${key} "${value}")
		set(${key_str} "${valuestr}")
		list(APPEND compile_defs ${key}=${valuestr})
	endforeach(keyvalue)

	list(APPEND INDEX_NAMES ${NAME_STR})

	add_executable(bin/build-${NAME} src/build.cpp)
	target_link_libraries(bin/build-${NAME} sdsl divsufsort divsufsort64 pthread FastPFor)
	set_property(TARGET bin/build-${NAME} PROPERTY COMPILE_DEFINITIONS ${compile_defs})

	add_executable(bin/measure-${NAME} src/measure.cpp)
	target_link_libraries(bin/measure-${NAME} sdsl divsufsort divsufsort64 pthread FastPFor)
	set_property(TARGET bin/measure-${NAME} PROPERTY COMPILE_DEFINITIONS ${compile_defs})

endforeach(index_type)

## build indexes first
add_custom_target(build-indexes
	COMMAND go run tools/build_indexes.go ${INDEX_NAMES}
	DEPENDS ${TEST_FILES}
)

# ## Run the benchmarks
add_custom_target(run-benchmark ALL
	COMMAND go run tools/run_benchmark.go ${INDEX_NAMES}
	DEPENDS ${TEST_FILES}
)
add_dependencies(run-benchmark build-indexes)
# ## Visualize results
add_custom_target(visualize-results ALL
	COMMAND go run tools/visualize_results.go
)
add_dependencies(visualize-results run-benchmark)
