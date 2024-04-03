#define the minimum cmake version
cmake_minimum_required(VERSION 3.20)

#name the project and some other stuff
project(
	webserv
	DESCRIPTION "A simple web server"
	HOMEPAGE_URL "https://github.com/kurz-m/webserv"
	VERSION 0.2
	LANGUAGES CXX
)

#export compile commands into a .json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#set cmake source directory to current directory
set(CMAKE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

#predefine some settings for the compiler
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)


if(NOT DEFINED LOG_LEVEL)
	set(LOG_LEVEL 4)
endif()

#specify different build options
set(CMAKE_CXX_FLAGS_RELEASE -O3)

#add compile flags
add_definitions(-D__LOG_LEVEL__=${LOG_LEVEL})
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -MMD -MP -march=native")

add_executable(webserv
	EventLogger.cpp
	EventLogger.hpp
	HTTP.cpp
	HTTP.hpp
	HTTPBase.cpp
	HTTPBase.hpp
	HTTPRequest.cpp
	HTTPRequest.hpp
	HTTPResponse.cpp
	HTTPResponse.hpp
	ISocket.cpp
	ISocket.hpp
	Lexer.cpp
	Lexer.hpp
	Parser.cpp
	Parser.hpp
	Server.cpp
	Server.hpp
	Settings.cpp
	Settings.hpp
	Socket.cpp
	Socket.hpp
	SocketConnect.cpp
	SocketConnect.hpp
	SocketListen.cpp
	SocketListen.hpp
	Token.cpp
	Token.hpp
	main.cpp
)

set_target_properties(webserv
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
