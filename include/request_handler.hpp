#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include "client.hpp"
#include "client_manager.hpp"
#include "config.hpp"
#include "shared.hpp"
#include <time.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <stack>
#include <boost/make_shared.hpp>

namespace http {
namespace server {

using namespace std;

struct reply;
struct request;

// The common handler for all incoming requests.
class request_handler {
	public:
		request_handler(const request_handler&)= delete;
		request_handler& operator=(const request_handler&)= delete;

		// Construct with a directory containing files to be served.
		explicit request_handler(config_ptr conf, const string& doc_root);

		// Handle a request and produce a reply.
		void handle_request(const request& req, vector <reply>& rep, client_ptr& this_client, char* data_, std::size_t bytes_transferred);
		void client_disconnected(client_ptr& this_client);

		// Turns a string map into an encoded string.
		string encode_output(string_map output);
		string encode_output(string_map_vector output);

		enum message_type : char {
			SignOn = 0,
			Message = 1,
			TimeOut = 2,
			SpawnCharacter = 3,
			RemoveCharacter = 4,
			UpdateGame = 5,
			UpdateSelf = 6,
			SavePosition = 7,
			LoadPosition = 8,
			UpdateCharacter = 9,
			Error = 10,
			ServerInfo = 11,
			LevelList = 12,
			PlayerList = 13
		};

		//Specify how many bytes each variable takes up in the message
		const static int username_size = 10;
		const static int character_size = 10;
		const static int level_size = 10;
		const static int version_size = 10;
		const static int float_size = 10;
		const static int string_size = 20;

	private:
		config_ptr config_;
		string doc_root_;
		client_manager client_manager_;
		time_t start_ = time(0);
		int data_index = 0;
		char* data;

		string_vector seperate_string(string input, string seperator);
		string create_new_uid(size_t length);
		bool url_decode(const string& in, string& out);
		void prepare_reply(vector<reply>& rep, string extension = "");
		void HandleSignOn(vector<reply>& rep, client_ptr& this_client);
		void HandleUpdate(vector<reply>& rep, client_ptr& this_client);
		void HandleChatMessage(vector<reply>& rep, client_ptr& this_client);
		void HandleSavePositionMessage(client_ptr& this_client);
		void HandleLoadPositionMessage(vector<reply>& rep, client_ptr& this_client);
		void HandleServerInfo(vector<reply>& rep, client_ptr& this_client);
		void HandleLevelList(vector<reply>& rep, client_ptr& this_client);
		void HandlePlayerList(vector<reply>& rep, client_ptr& this_client);
		void HandleUnknownMessage(vector<reply>& rep);
		void AddErrorMessage(vector<reply>& rep, string message);
		string GetString();
		float GetFloat();
		bool GetBool();
		int GetInt();
		void AddUpdateSelf(vector<reply>& rep, client_ptr& this_client);
		void AddChangedVariables(reply& rep, client_ptr& this_client);
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP
