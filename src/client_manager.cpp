#include "client_manager.hpp"

namespace http {
namespace server {

using namespace std;

client_manager::client_manager(config_ptr conf) : config_(conf) {
}

void client_manager::add_client(client_ptr player) {
	boost::unique_lock<boost::mutex> scoped_lock(clients_mutex_);

	clients_[player->get_uid()] = player;
}

void client_manager::add_command(string_map command, client_ptr initiator) {
	boost::unique_lock<boost::mutex> scoped_lock(clients_mutex_);

	for(auto& item: clients_) {
		if(initiator) {
			// Skip initiator.
			if(item.second == initiator) {
				continue;
			}

			// Skip clients that are not on the same level as initiator.
			if((item.second)->get_level() != initiator->get_level()) {
				continue;
			}
		}

		(item.second)->add_command(command);
	}
}

client_ptr client_manager::get_client(string uid) {
	boost::unique_lock<boost::mutex> scoped_lock(clients_mutex_);

	// First check if client exists.
	if(clients_.find(uid) == clients_.end()) {
		return NULL;
	}

	return clients_[uid];
}

client_map client_manager::get_clients(client_ptr initiator) {
	boost::unique_lock<boost::mutex> scoped_lock(clients_mutex_);

	// No initiator means all clients should be returned.
	if(!initiator) {
		return clients_;
	}

	// Since initiator is not empty we have to do some filtering.
	client_map other_clients;

	for(auto& item: clients_) {
		// Skip initiator.
		if(item.second == initiator) {
			continue;
		}

		// Skip clients that are not on the same level as initiator.
		if((item.second)->get_level() != initiator->get_level()) {
			continue;
		}

		other_clients[item.first] = item.second;
	}

	return other_clients;
}

client_map client_manager::pop_inactive_clients(time_t start, client_ptr initiator) {
	boost::unique_lock<boost::mutex> scoped_lock(clients_mutex_);

	client_map inactive_clients;

	for(auto& item: clients_) {
		if((initiator) && ((item.second)->get_uid() == initiator->get_uid())) {
			continue;
		}

		if((item.second)->get_last_updated() < (difftime(time(0), start) - config_->get_remove_delay())) {
			inactive_clients.insert(item);
		}
	}

	for(auto& item: inactive_clients) {
		clients_.erase(item.first);
	}

	return inactive_clients;
}

} // namespace server
} // namespace http
