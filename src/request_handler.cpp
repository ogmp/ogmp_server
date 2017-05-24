#include "request_handler.hpp"
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include "header.hpp"
#include "log.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <time.h>
#include <map>
#include <boost/format.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <stack>

namespace http {
namespace server {

using namespace std;

template <typename T>

struct my_id_translator
{
    typedef T internal_type;
    typedef T external_type;

    boost::optional<T> get_value(const T &v) { return  v.substr(1, v.size() - 2) ; }
    boost::optional<T> put_value(const T &v) { return '"' + v +'"'; }
};

request_handler::request_handler(config_ptr conf, const string& doc_root) :
config_(conf), doc_root_(doc_root), client_manager_(conf) {
}

string_vector request_handler::seperate_string(string input, string seperator) {
	string_vector return_vector;
	size_t pos= 0;
	string token;

	while((pos= input.find(seperator)) != string::npos) {
		token= input.substr(0, pos);
		return_vector.push_back(token);
		input.erase(0, pos + seperator.length());
	}

	return_vector.push_back(input);

	return return_vector;
}

string request_handler::create_new_uid(size_t length) {
	auto randchar= []() -> char {
		const char charset[]=
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index= sizeof(charset) - 1;

		// It should be noted that rand() is not secure.
		return charset[rand() % max_index];
	};

	string new_uid(length,0);
	generate_n(new_uid.begin(), length, randchar);

	return new_uid;
}

bool request_handler::url_decode(const string& in, string& out) {
	out.clear();
	out.reserve(in.size());
	for(size_t i= 0; i < in.size(); ++i) {
		if(in[i] == '%') {
			if(i + 3 <= in.size()) {
				int value= 0;
				istringstream is(in.substr(i + 1, 2));
				if(is >> hex >> value) {
					out+= static_cast<char>(value);
					i+= 2;
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else if(in[i] == '+') {
			out+= ' ';
		} else {
			out+= in[i];
		}
	}
	return true;
}

void request_handler::AddErrorMessage(vector<reply>& rep, string message){
	reply new_reply;
	new_reply.add_to_buffers(Error);
	new_reply.add_to_buffers(message);
	rep.push_back(new_reply);
}

string request_handler::GetString(){
	int size = (int)data[data_index];
	data_index++;
	string output;
	for(int i = 0; i < size; i++, data_index++){
		output += data[data_index];
	}
	return output;
}

float request_handler::GetFloat(){
	char b[] = {data[data_index + 3], data[data_index + 2], data[data_index + 1], data[data_index]};
	float f;
	memcpy(&f, &b, sizeof(f));
	data_index += 4;
	return f;
}

bool request_handler::GetBool(){
	bool value = false;
	if(data[data_index] == 1){
		value = true;
	}
	data_index++;
	return value;
}

int request_handler::GetInt(){
	int value = (int)data[data_index];
	data_index++;
	return value;
}

void request_handler::HandleSignOn(vector<reply>& rep, client_ptr& this_client){
    if(this_client){
        log::print( "!! Client tried signing on, while already being signed on... weird. " + this_client->get_username());
        return;
    }
	client new_client;
	this_client = boost::make_shared<client>(new_client);
	reply new_reply;

	string username = GetString();
	string character = GetString();
	string levelname = GetString();
	string levelpath = GetString();
	string version = GetString();
	
	float posx = GetFloat();
	float posy = GetFloat();
	float posz = GetFloat();
    
    int minimum_length = 5;
    if(levelname.length() < minimum_length || levelpath.length() < minimum_length){
        AddErrorMessage(rep, "The level you are trying to use is not valid!");
        log::print( "Client tried to connect with invalid values " + levelname + " " + levelpath);
        return;
    }
	
	//Check if the level is a default level and set the name from there.
	for (const auto& map : config_->get_map_list()) {
		if( levelpath == map.second.data() ){
			levelname = map.first.data();
			break;
		}
	}
	
	if(!config_->get_allow_other_maps()){
		vector<pair<string, string>> allowed_maps = config_->get_map_list();
		bool allowed = false;
		for(auto& item: allowed_maps) {
			if(levelpath == item.second){
				allowed = true;
				break;
			}
		}
		if(!allowed){
			AddErrorMessage(rep, "This level is not allowed on this server!");
			return;
		}
	}

	string character_dir = "turner";
	double signon_time= difftime(time(0), start_);
	
	// Check if someone is already using that username.
	client_map all_clients= client_manager_.get_clients();
	
	for(auto& item: all_clients) {
		if((item.second)->get_username() == username) {
			AddErrorMessage(rep, "Already a user with that username!");
			// Stop and send reply.
			return;
		}
	}
	
	this_client->set_level_name(levelname);
	this_client->set_level_path(levelpath);
	this_client->set_username(username);
	this_client->set_team(username);

	this_client->set_posx(posx);
	this_client->set_posy(posy);
	this_client->set_posz(posz);
	
	// Set default teleport to the spawn position.
	this_client->set_saved_posx(posx);
	this_client->set_saved_posy(posy);
	this_client->set_saved_posz(posz);

	this_client->set_character(character);
	//When the signon is successful 
	this_client->set_signed_on(true);
	
	// Add the client to the client list.
	client_manager_.add_client(this_client);
	
	new_reply.add_to_buffers(SignOn);
	new_reply.add_to_buffers(config_->get_update_refresh_rate());
	new_reply.add_to_buffers(this_client->get_username());
	new_reply.add_to_buffers(config_->get_welcome_message());
	new_reply.add_to_buffers(this_client->get_team());
	new_reply.add_to_buffers(this_client->get_character());
	new_reply.add_to_buffers(this_client->get_level_name());
	
	rep.push_back(new_reply);
	
	// Now add join commands for all other clients in the same group.
	client_map other_clients = client_manager_.get_clients(this_client);
	
	for(auto& item: other_clients) {
		reply spawn_character_reply;

		spawn_character_reply.add_to_buffers(SpawnCharacter);
		spawn_character_reply.add_to_buffers((item.second)->get_username());
		spawn_character_reply.add_to_buffers((item.second)->get_username());
		spawn_character_reply.add_to_buffers((item.second)->get_character());
		spawn_character_reply.add_to_buffers((item.second)->get_posx());
		spawn_character_reply.add_to_buffers((item.second)->get_posy());
		spawn_character_reply.add_to_buffers((item.second)->get_posz());
		rep.push_back(spawn_character_reply);
	}
	
	// Send message command to other players.
	reply message_reply;
	
	message_reply.add_to_buffers(Message);
	message_reply.add_to_buffers((string)"server");
	message_reply.add_to_buffers(this_client->get_username() + " has entered the room.");
	message_reply.add_to_buffers(true);
	client_manager_.add_to_inbox(message_reply, this_client);
	
	// Write the message to the logs.
	log::print(this_client->get_username() + " has entered the room.");

	// Send join command to other players.
	reply spawn_character_reply;
	
	spawn_character_reply.add_to_buffers(SpawnCharacter);
	spawn_character_reply.add_to_buffers(this_client->get_username());
	spawn_character_reply.add_to_buffers(this_client->get_team());
	spawn_character_reply.add_to_buffers(this_client->get_character());
	spawn_character_reply.add_to_buffers(this_client->get_posx());
	spawn_character_reply.add_to_buffers(this_client->get_posy());
	spawn_character_reply.add_to_buffers(this_client->get_posz());

	client_manager_.add_to_inbox(spawn_character_reply, this_client);
}

void request_handler::HandleUpdate(vector<reply>& rep, client_ptr& this_client){
	// Update player states.

	this_client->set_posx(GetFloat());
	this_client->set_posy(GetFloat());
	this_client->set_posz(GetFloat());
	this_client->set_dirx(GetFloat());
	this_client->set_dirz(GetFloat());

	this_client->set_crouch(GetBool());
	this_client->set_jump(GetBool());
	this_client->set_attack(GetBool());
	this_client->set_grab(GetBool());
	this_client->set_item(GetBool());
	this_client->set_drop(GetBool());
	this_client->set_roll(GetBool());
	this_client->set_jumpoffwall(GetBool());
	this_client->set_activeblock(GetBool());
	
	float blood_damage = GetFloat();
	float blood_health = GetFloat();
	float block_health = GetFloat();
	float temp_health = GetFloat();
	float permanent_health = GetFloat();
	int knocked_out = GetInt();
	float blood_amount = GetFloat();
	
	float recovery_time = GetFloat();
	float roll_recovery_time = GetFloat();
	this_client->set_ragdoll_type(GetInt());
	int blood_delay = GetInt();
	bool cut_throat = GetBool();
	int state = GetInt();
	
	if((this_client->get_time_of_death() < 1) || (difftime(time(0), this_client->get_time_of_death()) > 10)) {
		// Do not allow this_clients to increase some parts of their health.
		if(blood_health < this_client->get_blood_health()) {
			this_client->set_blood_health(blood_health);
		}
		if(permanent_health < this_client->get_permanent_health()) {
			this_client->set_permanent_health(permanent_health);
		}
		if(knocked_out > this_client->get_knocked_out()) {
			this_client->set_knocked_out(knocked_out);
		}
		this_client->set_blood_damage(blood_damage);
		this_client->set_block_health(block_health);
		this_client->set_temp_health(temp_health);
	}

	this_client->set_blood_delay(blood_delay);
	this_client->set_cut_throat(cut_throat);
	this_client->set_state(state);

	// Add commands from queue if available.
	while(this_client->get_number_of_inbox_messages() != 0) {
		reply message = this_client->get_inbox_message();
		rep.push_back(message);
	}

	// Check if the player died (we consider unconscious as dead for now).
	if((this_client->get_permanent_health() <= 0.0f)
	|| (this_client->get_blood_health() <= 0.0f)
	|| (this_client->get_temp_health() <= 0.0f)
	|| (this_client->get_knocked_out() != _awake)) {
		// Only announce death once.
		if(!this_client->get_death_changed()) {
			this_client->set_death_changed(true);
			this_client->set_time_of_death(time(0));

			// Send message to all players in the group.
			reply died_message;
			
			died_message.add_to_buffers(Message);
			died_message.add_to_buffers((string)"server");
			died_message.add_to_buffers(this_client->get_username() + " has died.");
			died_message.add_to_buffers(true);

			client_manager_.add_to_inbox(died_message, this_client);

			// Also send the message to player himself.
			this_client->add_to_inbox(died_message);
		} else {
			// Revive the player after some seconds (experimental).
			if(difftime(time(0), this_client->get_time_of_death()) > 5) {
				this_client->set_blood_health(1.0f);
				this_client->set_permanent_health(1.0f);
				this_client->set_blood_damage(0.0f);
				this_client->set_block_health(1.0f);
				this_client->set_temp_health(1.0f);
				this_client->set_knocked_out(_awake);
				this_client->set_blood_amount(10.0f);
				this_client->set_recovery_time(0.0f);
				this_client->set_roll_recovery_time(0.0f);
				this_client->set_remove_blood(true);
				this_client->set_cut_throat(false);
			}
		}
	} else {
		this_client->set_death_changed(false);
		this_client->set_remove_blood(false);
	}
	
	// Send health back to player.
	reply updateself_reply;
	
	updateself_reply.add_to_buffers(UpdateSelf);
	updateself_reply.add_to_buffers(this_client->get_blood_damage());
	updateself_reply.add_to_buffers(this_client->get_blood_health());
	updateself_reply.add_to_buffers(this_client->get_block_health());
	updateself_reply.add_to_buffers(this_client->get_temp_health());
	updateself_reply.add_to_buffers(this_client->get_permanent_health());
	updateself_reply.add_to_buffers(this_client->get_blood_amount());
	updateself_reply.add_to_buffers(this_client->get_recovery_time());
	updateself_reply.add_to_buffers(this_client->get_roll_recovery_time());
	updateself_reply.add_to_buffers(this_client->get_ragdoll_type());
	updateself_reply.add_to_buffers(this_client->get_remove_blood());
	updateself_reply.add_to_buffers(this_client->get_cut_throat());

	//TODO maybe just send this when needed.
	rep.push_back(updateself_reply);

	// Get states of the other clients.
	client_map other_clients = client_manager_.get_clients(this_client);

	for(auto& item: other_clients) {
		reply update_reply;
		
		update_reply.add_to_buffers(UpdateCharacter);
		update_reply.add_to_buffers((item.second)->get_username());
		update_reply.add_to_buffers((item.second)->get_posx());
		update_reply.add_to_buffers((item.second)->get_posy());
		update_reply.add_to_buffers((item.second)->get_posz());
		update_reply.add_to_buffers((item.second)->get_dirx());
		update_reply.add_to_buffers((item.second)->get_dirz());
		update_reply.add_to_buffers((item.second)->get_crouch());
		update_reply.add_to_buffers((item.second)->get_jump());
		update_reply.add_to_buffers((item.second)->get_attack());
		update_reply.add_to_buffers((item.second)->get_grab());
		update_reply.add_to_buffers((item.second)->get_item());
		update_reply.add_to_buffers((item.second)->get_drop());
		update_reply.add_to_buffers((item.second)->get_roll());
		update_reply.add_to_buffers((item.second)->get_jumpoffwall());
		update_reply.add_to_buffers((item.second)->get_activeblock());
		update_reply.add_to_buffers((item.second)->get_blood_damage());
		update_reply.add_to_buffers((item.second)->get_blood_health());
		update_reply.add_to_buffers((item.second)->get_block_health());
		update_reply.add_to_buffers((item.second)->get_temp_health());
		update_reply.add_to_buffers((item.second)->get_permanent_health());
		update_reply.add_to_buffers((item.second)->get_knocked_out());
		update_reply.add_to_buffers((item.second)->get_blood_amount());
		update_reply.add_to_buffers((item.second)->get_recovery_time());
		update_reply.add_to_buffers((item.second)->get_roll_recovery_time());
		update_reply.add_to_buffers((item.second)->get_ragdoll_type());
		update_reply.add_to_buffers((item.second)->get_remove_blood());
		update_reply.add_to_buffers((item.second)->get_blood_delay());
		update_reply.add_to_buffers((item.second)->get_cut_throat());
		update_reply.add_to_buffers((item.second)->get_state());

		rep.push_back(update_reply);
	}
}

void request_handler::HandleChatMessage(vector<reply>& rep, client_ptr& this_client){
	// Send message to all players in the group.
	reply chat_message;
	string chat_message_source = GetString();
	string chat_message_content = GetString();
	chat_message.add_to_buffers(Message);
	chat_message.add_to_buffers(this_client->get_username());
	chat_message.add_to_buffers(chat_message_content);
	chat_message.add_to_buffers(false);
	
	log::print(this_client->get_username() + ": " + chat_message_content);

	client_manager_.add_to_inbox(chat_message, this_client);
	this_client->add_to_inbox(chat_message);
}

void request_handler::HandleSavePositionMessage(client_ptr& this_client){
	// Don't continue if disabled.
	if(!config_->get_allow_teleport()) {
		return;
	}

	// Create copies of the client coordinates.
	this_client->set_saved_posx(this_client->get_posx());
	this_client->set_saved_posy(this_client->get_posy());
	this_client->set_saved_posz(this_client->get_posz());
	
}

void request_handler::HandleLoadPositionMessage(vector<reply>& rep, client_ptr& this_client){
	// Don't continue if disabled.
	if(!config_->get_allow_teleport()) {
		return;
	}

	reply load_position_message;
	load_position_message.add_to_buffers(LoadPosition);
	load_position_message.add_to_buffers(this_client->get_saved_posx());
	load_position_message.add_to_buffers(this_client->get_saved_posy());
	load_position_message.add_to_buffers(this_client->get_saved_posz());
	rep.push_back(load_position_message);
}

void request_handler::handle_request(const request& req, vector<reply>& rep, client_ptr& this_client, char* data_, std::size_t bytes_transferred) {
	data_index = 1;
	data = data_;
	switch(data[0]){
		case SignOn :
		{
			// cout << "Received signon message" << endl;
			HandleSignOn(rep, this_client);
			break;
		}
		case UpdateGame :
		{
			// cout << "Received UpdateGame message" << endl;
			HandleUpdate(rep, this_client);
			break;
		}
		case UpdateSelf :
		{
			// cout << "Received UpdateSelf message" << endl;
			break;
		}
		case Message :
		{
			// cout << "Received Message message" << endl;
			HandleChatMessage(rep, this_client);
			break;
		}
		case SavePosition :
		{
			// cout << "Received SavePosition message" << endl;
			HandleSavePositionMessage(this_client);
			break;
		}
		case LoadPosition :
		{
			// cout << "Received LoadPosition message" << endl;
			HandleLoadPositionMessage(rep, this_client);
			break;
		}
		case ServerInfo :
		{
			// cout << "Received ServerInfo message" << endl;
			HandleServerInfo(rep, this_client);
			break;
		}
		case LevelList :
		{
			// cout << "Received LevelList message" << endl;
			HandleLevelList(rep, this_client);
			break;
		}
		case PlayerList :
		{
			// cout << "Received PlayerList message" << endl;
			HandlePlayerList(rep, this_client);
			break;
		}
		default :
		{
			// cout << "Received Unknown message" << endl;
			HandleUnknownMessage(rep);
		}
	}
	return;
}

void request_handler::HandleUnknownMessage(vector<reply>& rep){
	reply unkown_message;
	unkown_message.add_plain_text("Go away!");
	rep.push_back(unkown_message);
}

void request_handler::HandlePlayerList(vector<reply>& rep, client_ptr& this_client){
	reply playerlist_message;
	playerlist_message.add_to_buffers(PlayerList);
	client_manager_.get_player_list(playerlist_message, this_client);
	rep.push_back(playerlist_message);
}

void request_handler::HandleServerInfo(vector<reply>& rep, client_ptr& this_client){
	reply serverinfo_message;
	serverinfo_message.add_to_buffers(ServerInfo);
	serverinfo_message.add_to_buffers(config_->get_server_name());
	serverinfo_message.add_to_buffers(client_manager_.get_nr_players());
	rep.push_back(serverinfo_message);
}

void request_handler::HandleLevelList(vector<reply>& rep, client_ptr& this_client){
	reply serverinfo_message;
	serverinfo_message.add_to_buffers(LevelList);
	client_manager_.get_level_list(serverinfo_message);
	rep.push_back(serverinfo_message);
}

void request_handler::client_disconnected(client_ptr& this_client) {
	if(this_client == NULL){
		return;
	}
	// Send disconnect message to other players.
	reply disconnect_message;
	
	disconnect_message.add_to_buffers(Message);
	disconnect_message.add_to_buffers((string)"server");
	disconnect_message.add_to_buffers(this_client->get_username() + " has left the room.");
	disconnect_message.add_to_buffers(true);
	client_manager_.add_to_inbox(disconnect_message, this_client);
	log::print(this_client->get_username() + " has left the room.");

	// Send remove character message to other players.
	reply remove_character;
	
	remove_character.add_to_buffers(RemoveCharacter);
	remove_character.add_to_buffers(this_client->get_username());
	client_manager_.add_to_inbox(remove_character, this_client);
	client_manager_.remove_client(this_client);
}

void request_handler::prepare_reply(vector<reply>& rep, string extension) {
	
}

string request_handler::encode_output(string_map output) {
	stringstream answer;

	for(auto it = output.begin(); it != output.end(); ++it) {
		// Add separator for all but the first element.
		if(it != output.begin()) {
			answer << "&";
		}

		// Encode the value and append it.
		string value = it->second;

		// Percent has to be encoded first!
		boost::replace_all(value, "%", "%25");
		boost::replace_all(value, "\"", "%22");
		boost::replace_all(value, "#", "%23");
		boost::replace_all(value, "&", "%26");
		boost::replace_all(value, "'", "%27");
		boost::replace_all(value, "=", "%3d");
		boost::replace_all(value, "\\", "%5c");

		answer << it->first << "=" << value;
	}

	return answer.str();
}

string request_handler::encode_output(string_map_vector output) {
	stringstream answer;

	for(auto it = output.begin(); it != output.end(); ++it) {
		// Add separator for all but the first element.
		if(it != output.begin()) {
			answer << "#";
		}

		// Add the output from the other encode method.
		answer << encode_output(*it);
	}

	return answer.str();
}

} // namespace server
} // namespace http
