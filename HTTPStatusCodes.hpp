#ifndef __HTTPSTATUSCODES_HPP__
#define __HTTPSTATUSCODES_HPP__

#include <map>
#include <string>

static std::map<int, std::string> create_map() {
	std::map<int, std::string> map;
	map[200] = "OK";
	map[201] = "Created";
	map[204] = "No Content";
	map[400] = "Bad Request";
	map[401] = "Unauthorized";
	map[403] = "Forbidden";
	map[404] = "Not Found";
	map[500] = "Internal Server Error";
	return map;
}

const std::map<int, std::string> status_code = create_map();

#endif // __HTTPSTATUSCODES_HPP__