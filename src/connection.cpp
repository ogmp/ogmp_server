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
    m_timer = new boost::asio::deadline_timer(socket_.get_io_service());
	do_read();
}

void connection::stop() {
    request_handler_.client_disconnected(this_client_);
	socket_.close();
}

void connection::client_timed_out(){
    if(!ignore_timeout){
        cout << "Client timed out" << endl;
        connection_manager_.stop(shared_from_this());
    }
    ignore_timeout = false;
}

void connection::do_read() {
	auto self(shared_from_this());
    
    m_timer->expires_from_now(boost::posix_time::seconds(15));
    m_timer->async_wait(boost::bind(&connection::client_timed_out, this));
    
    read_more = false;
	socket_.async_read_some(boost::asio::buffer(buffer_),
		[this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
            if(!ec) {
				request_parser::result_type result;
				std::tie(result, std::ignore) = request_parser_.parse(
				request_, buffer_.data(), buffer_.data() + bytes_transferred);
                //result returns good or bad.
                request_handler_.handle_request(request_, replies_, this_client_, buffer_.data(), bytes_transferred);
                do_write();
            }
            else if (ec) {
				connection_manager_.stop(shared_from_this());
			}
            read_more = true;
	});
    while (socket_.get_io_service().run_one()){
        if (read_more){
            ignore_timeout = true;
            m_timer->cancel();
            if(socket_.is_open()){
                do_read();
            }
            break;
        }
    }
}

void connection::do_write() {
	auto self(shared_from_this());
	while(replies_.size() > 0){
		reply& current_reply = replies_.front();
		current_reply.add_size_byte();
		boost::asio::async_write(socket_, current_reply.to_buffers(),
		[this, self, current_reply](boost::system::error_code ec, std::size_t) {
			//Close the connection if it was an http request, a error on the socket happened or if the signon failed
			if ( !this_client_ || ec && !this_client_->get_signed_on() || !this_client_->get_signed_on() ) {
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			}
            //The player is already signed on, but some error occured.
            else if (ec) {
				connection_manager_.stop(shared_from_this());
			}
		});
		replies_.erase (replies_.begin());
	}
}

} // namespace server
} // namespace http
