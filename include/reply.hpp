#ifndef HTTP_REPLY_HPP
#define HTTP_REPLY_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "request_handler.hpp"

namespace http {
namespace server {

/// A reply to be sent to a client.
struct reply {
	/// The content to be sent in the reply.
	std::vector<char> buffer;

	/// Convert the reply into a vector of buffers. The buffers do not own the
	/// underlying memory blocks, therefore the reply object must remain valid and
	/// not be changed until the write operation has completed.
	std::vector<boost::asio::const_buffer> to_buffers();
	void add_to_buffers(char* content, int size);
	void add_to_buffers(char content);
	void add_to_buffers(request_handler::message_type content);
	void add_to_buffers(float content);
	void add_to_buffers(std::string content);
	void add_to_buffers(bool content);
	void add_to_buffers(int content);
	void add_plain_text(std::string content);
	vector<unsigned char> intToByteArray(int value);
	void floatToByteArray(float f);
	void add_size_byte();
};

} // namespace server
} // namespace http

#endif // HTTP_REPLY_HPP
