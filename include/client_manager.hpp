#ifndef HTTP_CLIENT_MANAGER_HPP
#define HTTP_CLIENT_MANAGER_HPP

#include "client.hpp"
#include "config.hpp"
#include "shared.hpp"
#include <string>
#include <map>
#include <boost/thread/mutex.hpp>

namespace http {
namespace server {

using namespace std;

struct reply;

typedef map<string, client_ptr> client_map;

class client_manager {
	public:
		client_manager(config_ptr conf);
		void add_client(client_ptr player);
		void add_command(string_map command, client_ptr initiator = NULL);
		void add_to_inbox(reply& command, client_ptr initiator = NULL);
		void remove_client(client_ptr initiator);
		void get_level_list(reply& rep);
		void get_player_list(reply& rep, client_ptr initiator = NULL);
		int get_nr_players();
		client_ptr get_client(string uid);
		client_map get_clients(client_ptr initiator = NULL);
		client_map pop_inactive_clients(time_t start, client_ptr initiator = NULL);

	private:
		config_ptr config_;
		client_map clients_;
		boost::mutex clients_mutex_;
};

} // namespace server
} // namespace http

#endif
