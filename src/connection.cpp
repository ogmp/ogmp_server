#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"

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
				// cout << "Data: " << buffer_.data() << endl;
				// cout << "Nr bytes " << bytes_transferred << endl;
				cout << "First byte " << (int)buffer_[0] << endl;
				
				if(result == request_parser::good) {
					cout << "Request good" << endl;
					request_handler_.handle_request(request_, replies_, this_client_, buffer_.data(), bytes_transferred);
					do_write();
					//std::fill(buffer_.data(), buffer_.data() + bytes_transferred, 0);
					do_read();					
				} else if(result == request_parser::bad) {
					cout << "Request bad" << endl;
					reply bad_request_reply = reply::stock_reply(reply::bad_request);
					replies_.push(bad_request_reply);
					do_write();
					cout << "Closing connection because of bad request." << endl;
					connection_manager_.stop(shared_from_this());
				} else {
					do_read();
				}
			} else if(ec != boost::asio::error::operation_aborted) {
					cout << "connection closed" << endl;
					connection_manager_.stop(shared_from_this());
			}

	});
}

void connection::do_write() {
	auto self(shared_from_this());
	cout << "Writing: " << replies_.size() << endl;
	if(replies_.size() < 1){
		return;
	}
	reply& current_reply = replies_.top();
	
	cout << "Writing socket: " << current_reply.content << endl;
	//cout << "Size: " << boost::asio::buffer_size(current_reply.to_buffers()) << endl;
	
	boost::asio::async_write(socket_, current_reply.to_buffers(),
	[this, self, current_reply](boost::system::error_code ec, std::size_t) {
		//Close the connection if it was an http request, a error on the socket happened or if the signon failed
		if (ec || !this_client_.get_signed_on()) {
			// Initiate graceful connection closure.
			boost::system::error_code ignored_ec;
			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
				ignored_ec);
				cout << "connection closing" << endl;
		}
		if (ec == boost::asio::error::operation_aborted) {
			cout << "connection closed" << endl;
			connection_manager_.stop(shared_from_this());
		}
		if(replies_.size() > 0){
			do_write();
		}
	});
		
	// boost::asio::async_write(socket_, boost::asio::buffer( current_reply.to_buffers(), current_reply.content.size() ) ,
	// 	[this, self, current_reply](boost::system::error_code ec, std::size_t) {
	// 		//Close the connection if it was an http request, a error on the socket happened or if the signon failed
	// 		if (ec || !current_reply.json || !this_client_.get_signed_on()) {
	// 			// Initiate graceful connection closure.
	// 			boost::system::error_code ignored_ec;
	// 			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
	// 				ignored_ec);
	// 			}
	// 			if (ec == boost::asio::error::operation_aborted) {
	// 				connection_manager_.stop(shared_from_this());
	// 			}
	// 			return;
	// 		});
	replies_.pop();
}

} // namespace server
} // namespace http
