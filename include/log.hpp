#ifndef HTTP_LOG_HPP
#define HTTP_LOG_HPP

#include <string>

namespace http {
namespace server {

using namespace std;

class log {
	public:
		static string file_;
		static void print(string text);
		static string get_current_time();
		static void set_log_file(string file);
};

} // namespace server
} // namespace http

#endif
