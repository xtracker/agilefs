cmake_minimum_required (VERSION 2.8)

project (agilefs)

include_directories (
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src/client 
	${PROJECT_SOURCE_DIR}/src/io
	${PROJECT_SOURCE_DIR}/src/server
	${PROJECT_SOURCE_DIR}/src/common
	${PROJECT_SOURCE_DIR}/src/common/encrypt
	${PROJECT_SOURCE_DIR}/src/common/quicklist
	${PROJECT_SOURCE_DIR}/src/common/quickhash)

add_subdirectory (src objs)

add_definitions (-Wall -g)
