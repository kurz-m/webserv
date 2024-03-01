#include "HTTPResponse.hpp"
#include <fstream>
#include <sstream>

static const std::string proto_ = "HTML/1.1";

HTTPResponse::HTTPResponse() {}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse &cpy) : HTTPBase(cpy) {}

HTTPResponse &HTTPResponse::operator=(const HTTPResponse &other) {
  HTTPBase::operator=(other);
  return *this;
}

void HTTPResponse::resolve_uri_() {
  if (parsed_header_.at("URI") == "/") {
    parsed_header_.at("URI") = "./status-pages/index.html"; //TODO: use webroot
  }
}

void HTTPResponse::make_header_() {
  std::ifstream file(parsed_header_.at("URI").c_str());
	if (file.is_open()) {
		// something
	}
	else {
		file.open("./status-pages/404.html");
    status_code_ = 404;
	}
  body_.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  std::ostringstream oss;
  oss << proto_ << " " << status_code_ << " " << status_map_.at(status_code_) << "\r\n";
  oss << "Content-Length: " << body_.length() << "\r\n";
  oss << "\r\n\r\n";
	buffer_ = oss.str();
  buffer_ += body_;
	file.close();
}

void HTTPResponse::prepare_for_send() {
  resolve_uri_();
	make_header_();
}
