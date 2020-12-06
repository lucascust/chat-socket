project(CHATDISTRIBUIDO)

include(GNUInstallDirs)

set (EXECUTABLE_NAME "chat_distribuido")

cmake_minimum_required(VERSION 3.19)

# set sources and headers files set sources from plugin on google style.
file(GLOB_RECURSE
     SW_SRCS
     ./src/*.cpp
     ./src/*/*.cxx
     ./src/*/*.c)
# set headers from plugin on google style.
file(GLOB_RECURSE
     SW_HDRS
     ./include/*.h
     ./include/*/*.hxx
     ./include/*/*.hpp
		 ./include/*/*.hh)
		 
add_executable("client" "./src/client.cpp")
add_executable("server" "./src/server.cpp")
