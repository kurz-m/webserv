#include "HTTPBase.hpp"

HTTPBase::HTTPBase() {}

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
