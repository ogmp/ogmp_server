#include "config.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace http {
namespace server {

using namespace std;

config::config(string filename) {
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(filename, pt);

	update_refresh_rate_ = pt.get<float>("ServerSettings.update_refresh_rate");
	welcome_message_ = pt.get<string>("ServerSettings.welcome_message");
	remove_delay_ = pt.get<double>("ServerSettings.remove_delay");
	allow_teleport_ = pt.get<bool>("ServerSettings.allow_teleport");
	debug_ = pt.get<bool>("ServerSettings.debug");
}

float config::get_update_refresh_rate() {
	return update_refresh_rate_;
}

string config::get_welcome_message() {
	return welcome_message_;
}

double config::get_remove_delay() {
	return remove_delay_;
}

bool config::get_allow_teleport() {
	return allow_teleport_;
}

bool config::get_debug() {
	return debug_;
}

} // namespace server
} // namespace http
