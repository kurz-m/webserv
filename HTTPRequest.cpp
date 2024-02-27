#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest() {}

HTTPRequest::~HTTPRequest() {}

HTTPRequest::HTTPRequest(const HTTPRequest& cpy) : HTTPBase(cpy) {}

HTTPRequest& HTTPRequest::operator=(const HTTPRequest& other) {
	HTTPBase::operator = (other);
	return *this;
}
