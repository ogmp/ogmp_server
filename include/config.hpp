#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>

namespace http {
namespace server {

using namespace std;

class config {
	public:
		config(string filename);
		float get_update_refresh_rate(); 
		string get_welcome_message();
		string get_server_name();
		double get_remove_delay();
		bool get_allow_teleport();
		bool get_debug();
		bool get_allow_other_maps();
		string get_log_file();
		vector<pair<string,string>> get_map_list();

	private:
		float update_refresh_rate_;
		string welcome_message_, log_file_, server_name_;
		double remove_delay_;
		bool allow_teleport_, debug_, allow_other_maps_;
		vector<pair<string, string>> map_list_;
};

typedef boost::shared_ptr<config> config_ptr;

} // namespace server
} // namespace http

#endif
