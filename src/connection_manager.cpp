#include "connection_manager.hpp"
#include "connection.hpp"
#include <thread>

namespace http {
namespace server {

connection_manager::connection_manager() {
}

void connection_manager::start(connection_ptr c) {
	
	std::thread connection_thread(&connection::start, c);
	//connection_thread.join();
	connection_thread.detach();
}

} // namespace server
} // namespace http
