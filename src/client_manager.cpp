#include "client_manager.hpp"
#include "reply.hpp"

namespace http {
namespace server {

using namespace std;

client_manager::client_manager(config_ptr conf) : config_(conf) {
}

void client_manager::add_client(client_ptr player) {
	boost::unique_lock<boost::mutex> scoped_lock(clients_mutex_);

	clients_[player->get_username()] = player;
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
			if((item.second)->get_level_path() != initiator->get_level_path()) {
				continue;
			}
		}

		(item.second)->add_command(command);
	}
}

int client_manager::get_nr_players(){
	return clients_.size();
}

void client_manager::add_to_inbox(reply& command, client_ptr initiator) {
	boost::unique_lock<boost::mutex> scoped_lock(clients_mutex_);

	for(auto& item: clients_) {
		if(initiator) {
			// Skip initiator.
			if(item.second == initiator) {
				continue;
			}

			// Skip clients that are not on the same level as initiator.
			if((item.second)->get_level_path() != initiator->get_level_path()) {
				continue;
			}
		}

		(item.second)->add_to_inbox(command);
	}
}

void client_manager::remove_client(client_ptr initiator) {
	std::cout << "Removing client!" << std::endl;
	boost::unique_lock<boost::mutex> scoped_lock(clients_mutex_);

	for(auto& item: clients_) {
		if(initiator) {
			if(item.second == initiator) {
				clients_.erase(item.first);
				continue;
			}
		}
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
		if(item.second->get_username() == initiator->get_username()) {
			continue;
		}

		// Skip clients that are not on the same level as initiator.
		if((item.second)->get_level_path() != initiator->get_level_path()) {
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
		if((initiator) && ((item.second)->get_username() == initiator->get_username())) {
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

class TempLevelInfo{
	private:
		string level_name;
		string level_path;
		int nr_players;
	public:
		TempLevelInfo(string level_name_, string level_path_, int nr_players_){
			level_name = level_name_;
			level_path = level_path_;
			nr_players = nr_players_;
		}
		string get_level_name(){
			return level_name;
		}
		string get_level_path(){
			return level_path;
		}
		int get_nr_players(){
			return nr_players;
		}
};

void client_manager::get_player_list(reply& rep, client_ptr initiator) {
	if(!initiator) {
		cout << "The initiator is NULL" << endl;
		return;
	}
	for(auto& current_client: clients_) {
		cout << (current_client.second)->get_level_path() << endl;
		cout << initiator->get_level_path() << endl;
		if((current_client.second)->get_level_path() == initiator->get_level_path()) {
			rep.add_to_buffers((current_client.second)->get_username());
			rep.add_to_buffers((current_client.second)->get_character());
		}
	}
}

void client_manager::get_level_list(reply& rep) {
	vector<TempLevelInfo> temp_levels;
	//Get all the default maps and the nr of players on them
	for (const auto& map : config_->get_map_list()) {
		int nr_players = 0;
		for(auto& current_client: clients_) {
			if( (current_client.second)->get_level_path() == map.second.data() ){
				nr_players++;
			}
		}
		temp_levels.push_back(TempLevelInfo(map.first.data(), map.second.data(), nr_players));
	}
	//Now get the maps that are not default, but players are already on them.
	for(auto& current_client: clients_) {
		bool is_on_default = false;
		for (const auto& map : config_->get_map_list()) {
			if( (current_client.second)->get_level_path() == map.second.data() ){
				is_on_default = true;
				break;
			}
		}
		if(!is_on_default){
			//Check if non default map is already added
			bool already_added = false;
			for(auto& info : temp_levels){
				if(info.get_level_path() == (current_client.second)->get_level_path()){
					already_added = true;
					break;
				}
			}
			if(!already_added){
				int nr_players = 0;
				for(auto& custom_map_current_client: clients_) {
					if((current_client.second)->get_level_path() == (custom_map_current_client.second)->get_level_path()){
						nr_players++;
					}
				}
				temp_levels.push_back(TempLevelInfo((current_client.second)->get_level_name(), (current_client.second)->get_level_path(), nr_players));
			}
		}
	}
	//Now write all the level info to the reply as data.
	for(auto& info : temp_levels){
		rep.add_to_buffers(info.get_level_name());
		rep.add_to_buffers(info.get_level_path());
		rep.add_to_buffers(info.get_nr_players());
	}
}

} // namespace server
} // namespace http
