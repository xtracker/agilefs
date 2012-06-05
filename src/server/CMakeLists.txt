add_definitions(-Wall -g)
set(SERVER_SRC_LIST
	agilefs-server.c 
	meta-server.c
	chunks.h chunks.c
	db_ops.h db_ops.c
	xmlparser.h expat-xml.c
	chunks-io.h chunks-io.c
	md5.h md5.c
	queue.c)
add_executable(agilefs-server ${SERVER_SRC_LIST})
target_link_libraries(agilefs-server db expat)
