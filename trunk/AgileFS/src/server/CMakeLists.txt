add_definitions(-Wall -O2)
set(SERVER_SRC_LIST agilefs-server.c 
	chunks.h chunks.c
	db_ops.h db_ops.c)
add_executable(agilefs-server ${SERVER_SRC_LIST})
target_link_libraries(agilefs-server db)
