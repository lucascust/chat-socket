project(CHATDISTRIBUIDO)

include(GNUInstallDirs)
cmake_minimum_required(VERSION 3.17)


set (EXECUTABLE_NAME "chat_distribuido")

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
		 
add_executable(${EXECUTABLE_NAME} ${SW_SRCS} ${SW_HDRS} "src/server.cpp" "src/client.cpp" "src/main.cpp")
