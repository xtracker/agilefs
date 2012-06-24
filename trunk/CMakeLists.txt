
cmake_minimum_required (VERSION 2.8)

project (AGILEFS)

include_directories (
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src/client 
	${PROJECT_SOURCE_DIR}/src/io
	${PROJECT_SOURCE_DIR}/src/server
	${PROJECT_SOURCE_DIR}/src/common)

add_subdirectory (src objs)

add_definitions (-Wall)
