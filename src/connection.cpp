#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include <mutex>

namespace http {
namespace server {

connection::connection(boost::asio::ip::tcp::socket socket,
connection_manager& manager, request_handler& handler, int delay) :
socket_(std::move(socket)), connection_manager_(manager),
request_handler_(handler), remove_delay(delay){
}

void connection::start() {
    m_timer = new boost::asio::deadline_timer(socket_.get_executor());
	do_read();
}

void connection::stop() {
    request_handler_.client_disconnected(this_client_);
	socket_.close();
}

void connection::client_timed_out(const boost::system::error_code& ec){
    // cout << "Error " << ec.message() << endl;
    if(ec != boost::asio::error::operation_aborted){
        if(this_client_){
            log::print("Client timed out " + this_client_->get_username());
        }
        connection_manager_.stop(shared_from_this());
    }
}

void connection::do_read() {
	auto self(shared_from_this());

    m_timer->expires_from_now(boost::posix_time::seconds(remove_delay));
    m_timer->async_wait(boost::bind(&connection::client_timed_out, this, _1));

	socket_.async_read_some(boost::asio::buffer(buffer_),
		[this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
            if(!ec) {
				request_parser::result_type result;
				std::tie(result, std::ignore) = request_parser_.parse(
				request_, buffer_.data(), buffer_.data() + bytes_transferred);
                //result returns good or bad.
                request_handler_.handle_request(request_, replies_, this_client_, buffer_.data(), bytes_transferred);
                do_write();
                m_timer->cancel_one();
                do_read();
            }else if(ec) {
                m_timer->cancel_one();
                // cout << "Error " << ec.message() << endl;
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
			if ( !this_client_ || ec && !this_client_->get_signed_on() || !this_client_->get_signed_on() ) {
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
                return;
			}
            //The player is already signed on, but some error occured.
            else if (ec) {
				connection_manager_.stop(shared_from_this());
                return;
			}
		});
		replies_.erase (replies_.begin());
	}
}

} // namespace server
} // namespace http
