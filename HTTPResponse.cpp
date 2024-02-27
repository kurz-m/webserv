#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse() {}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse& cpy) : HTTPBase(cpy) {}

HTTPResponse& HTTPResponse::operator=(const HTTPResponse& other) {
	HTTPBase::operator = (other);
	return *this;
}
