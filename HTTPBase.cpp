#include "HTTPBase.hpp"

std::map<int, std::string> HTTPBase::create_map_() {
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

const std::map<int, std::string> HTTPBase::status_map_ = HTTPBase::create_map_();

HTTPBase::HTTPBase() {
}

HTTPBase::HTTPBase(const HTTPBase& cpy)
{
	*this = cpy;
}

HTTPBase& HTTPBase::operator=(const HTTPBase& other)
{
	if (this != &other) {
		buffer_ = other.buffer_;
		parsed_header_ = other.parsed_header_;
		body_ = other.body_;
	}
	return *this;
}

HTTPBase::~HTTPBase() {}

void HTTPBase::parse_buffer_() {}
