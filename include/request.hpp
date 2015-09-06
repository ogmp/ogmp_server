#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "header.hpp"
#include <string>
#include <vector>

namespace http {
namespace server {

// A request received from a client.
struct request {
	std::string method;
	std::string uri;
	int http_version_major;
	int http_version_minor;
	std::vector<header> headers;

	std::string post_content;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP
