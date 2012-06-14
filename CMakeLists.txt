cmake_minimum_required(VERSION 2.8)
project (AgileFS)
include_directories(
	include 
	src/client 
	src/io 
	src/server)
add_subdirectory(src bin)
add_definitions(-Wall)
