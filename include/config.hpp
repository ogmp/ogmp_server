#ifndef HTTP_CONFIG_HPP
#define HTTP_CONFIG_HPP

#include <string>
#include <boost/shared_ptr.hpp>

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
		string get_log_file();

	private:
		float update_refresh_rate_;
		string welcome_message_, log_file_, server_name_;
		double remove_delay_;
		bool allow_teleport_, debug_;
};

typedef boost::shared_ptr<config> config_ptr;

} // namespace server
} // namespace http

#endif
