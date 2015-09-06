#include <iostream>
#include "server.hpp"
#include "config.hpp"

int main(int argc, char* argv[]) {
	//We need to set this for the random UID generator in request_handler.cpp
	srand(time(0));

	try {
		// Check command line arguments.
		if(argc != 4) {
			std::cerr << "Usage: http_server <address> <port> <doc_root>\n" <<
			"  For IPv4, try:\n" << 
			"    receiver 0.0.0.0 9000 .\n" << 
			"  For IPv6, try:\n" <<
			"    receiver 0::0 9000 .\n";

			return 1;
		}

		// Read the settings from config.ini and put them in variables.
		http::server::config_ptr conf(new http::server::config("config.ini"));

		std::cout << "------------------------------------" <<
		"\nUpdate refresh rate: " << conf->get_update_refresh_rate() <<
		"\nWelcome message: " << conf->get_welcome_message() <<
		"\nClients are removed after a delay of " << conf->get_remove_delay() << " seconds." <<
		"\nAllow teleport: " << conf->get_allow_teleport() <<
		"\nDebug: " << conf->get_debug() <<
		"\n------------------------------------\n";

		// Initialise the server.
		http::server::server s(conf, argv[1], argv[2], argv[3]);

		// Run the server until stopped.
		s.run();
	} catch(std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
