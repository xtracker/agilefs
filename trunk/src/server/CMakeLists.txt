add_definitions (-Wall -g)

set (SERVER_SRC_LIST
	agilefs-server.c 
	meta-server.c
	chunks.h chunks.c
	db_ops.h db_ops.c
	xmlparser.h expat-xml.c
	chunks-io.h chunks-io.c
	queue.c)

link_directories (${PROJECT_BINARY_DIR}/libs)

add_executable (agilefs-server ${SERVER_SRC_LIST})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

target_link_libraries (agilefs-server db expat hash)
