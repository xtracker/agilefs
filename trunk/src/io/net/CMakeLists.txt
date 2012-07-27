set (NET_SRC_LISTS
	sockio.h sockio.c
	socket-collection.h socket-collection.c)

add_executable (test-server ${NET_SRC_LISTS} test_main.c)
add_executable (test-client ${NET_SRC_LISTS} test_client.c)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
