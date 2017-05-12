#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include <mutex>

namespace http {
namespace server {

connection::connection(boost::asio::ip::tcp::socket socket,
connection_manager& manager, request_handler& handler) :
socket_(std::move(socket)), connection_manager_(manager),
request_handler_(handler) {
}

void connection::start() {
	do_read();
}

void connection::stop() {
	socket_.close();
}

void connection::do_read() {
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(buffer_),
		[this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
			if(!ec) {
				request_parser::result_type result;
				std::tie(result, std::ignore) = request_parser_.parse(
				request_, buffer_.data(), buffer_.data() + bytes_transferred);
				if(result == request_parser::good) {
					request_handler_.handle_request(request_, replies_, this_client_, buffer_.data(), bytes_transferred);
					do_write();
					do_read();
				} else if(result == request_parser::bad) {
					do_write();
					// cout << "Closing connection because of bad request." << endl;
					connection_manager_.stop(shared_from_this());
				} else {
					do_read();
				}
			} else if(ec != boost::asio::error::operation_aborted) {
					// cout << "connection closed" << endl;
					request_handler_.client_disconnected(this_client_);
					connection_manager_.stop(shared_from_this());
			}

	});
}

void connection::do_write() {
	auto self(shared_from_this());
	while(replies_.size() > 0){
		reply& current_reply = replies_.front();
		current_reply.add_size_byte();
		boost::asio::async_write(socket_, current_reply.to_buffers(),
		[this, self, current_reply](boost::system::error_code ec, std::size_t) {
			//Close the connection if it was an http request, a error on the socket happened or if the signon failed
			if (ec || !this_client_ || !this_client_->get_signed_on()) {
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
					ignored_ec);
			}
			if (ec == boost::asio::error::operation_aborted) {
				request_handler_.client_disconnected(this_client_);
				connection_manager_.stop(shared_from_this());
			}
		});
		replies_.erase (replies_.begin());
	}
}

} // namespace server
} // namespace http
