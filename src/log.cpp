#include "log.hpp"
#include <fstream>
#include <ostream>
#include <iostream>

namespace http {
namespace server {

using namespace std;

string log::file_ = "";

void log::print(string text) {
	ofstream out(file_.c_str(), std::ios_base::app);

	if (!out.is_open()) {
		return;
	}

	out << get_current_time() << "\t" << text << "\n";
	out.close();
}

string log::get_current_time() {
	time_t now = time(0);
	char buf[80];
	struct tm tstruct = *localtime(&now);

	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return buf;
}

void log::set_log_file(string file) {
	file_ = file;
}

} // namespace server
} // namespace http
