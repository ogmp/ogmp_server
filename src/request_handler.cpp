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

bool request_handler::handle_command(string_map& input, stack <reply>& rep) {
	// Remove old clients that are not responding.
	client_map inactive_clients= client_manager_.pop_inactive_clients(start_);
	reply new_reply;
	// Announce quits.
	for(auto& item: inactive_clients) {
		// Add part message to all clients that are in the same
		// group as the timeouted client.
		string_map message;

		message["type"] = "Message";
		message["name"] = "server";
		message["text"] = (item.second)->get_username() + " has left the room.";
		message["notif"] = "true";

		client_manager_.add_command(message, item.second);

		// Also add a part command.
		string_map part;

		part["type"] = "RemoveCharacter";
		part["username"] = (item.second)->get_username();

		client_manager_.add_command(part, item.second);

		// Write the message to the logs.
		log::print(message["text"]);
	}

	// First we check for commands that do not require to be an active player.
	if(input["type"] == "ListUsers") {
		client_map clients= client_manager_.get_clients();

		string_map_vector answer;

		for(auto& item: clients) {
			string_map user;

			user["character"] = (item.second)->get_character();
			user["username"] = (item.second)->get_username();
			user["level"] = (item.second)->get_level();

			answer.push_back(user);
		}

		new_reply.content= encode_output(answer);

		// Stop and send reply.
		return true;
	}

	// Everything that follows now requires a valid uid (except signon).
	client_ptr player= client_manager_.get_client(input["uid"]);

	if(input["type"] != "SignOn") {
		if(!player) {
			string_map answer;

			answer["type"] = "Timeout";

			new_reply.content= encode_output(answer);

			// Stop and send reply.
			return true;
		}
	}

	if(input["type"] == "SignOn") {
		string new_uid= create_new_uid(10);
		string character_dir = "turner";
		double signon_time= difftime(time(0), start_);

		// Check if someone is already using that username.
		client_map all_clients= client_manager_.get_clients();

		for(auto& item: all_clients) {
			if((item.second)->get_username() == input["username"]) {
				string_map answer;

				answer["type"] = "Error";
				answer["reason"] = "Username already existing.";

				new_reply.content= encode_output(answer);

				// Stop and send reply.
				return true;
			}
		}

		// Create a new client.
		client_ptr new_player(new client());

		new_player->set_uid(new_uid);
		new_player->set_level(input["level"]);
		new_player->set_username(input["username"]);
		new_player->set_team(create_new_uid(4));
		new_player->set_last_updated(signon_time + 30); // Give the client more time.

		// Set coordinates to default if not set for compatibility with ogmp_clients < 0.0.3.
		if(input.find("posx") == input.end()) {
			input["posx"] = "0";
		}

		if(input.find("posy") == input.end()) {
			input["posy"] = "0";
		}

		if(input.find("posz") == input.end()) {
			input["posz"] = "0";
		}

		new_player->set_posx(stof(input["posx"]));
		new_player->set_posy(stof(input["posy"]));
		new_player->set_posz(stof(input["posz"]));

		// Set default teleport to the spawn position.
		new_player->set_saved_posx(stof(input["posx"]));
		new_player->set_saved_posy(stof(input["posy"]));
		new_player->set_saved_posz(stof(input["posz"]));

		if(input["character"] == "Guard") {
			character_dir = "guard";
		}else if(input["character"] == "Raider+Rabbit") {
			character_dir = "raider_rabbit";
		}else if(input["character"] == "Pale+Turner") {
			character_dir = "pale_turner";
		}else if(input["character"] == "Guard+2") {
			character_dir = "guard2";
		}else if(input["character"] == "Base+Guard") {
			character_dir = "base_guard";
		}else if(input["character"] == "Cat") {
			character_dir = "cat";
		}else if(input["character"] == "Female+Rabbit+1") {
			character_dir = "female_rabbit_1";
		}else if(input["character"] == "Female+Rabbit+2") {
			character_dir = "female_rabbit_2";
		}else if(input["character"] == "Female+Rabbit+3") {
			character_dir = "female_rabbit_3";
		}else if(input["character"] == "Rat") {
			character_dir = "rat";
		}else if(input["character"] == "Female+Rat") {
			character_dir = "female_rat";
		}else if(input["character"] == "Hooded+Rat") {
			character_dir = "hooded_rat";
		}else if(input["character"] == "Light+Armored+Dog+Big") {
			character_dir = "lt_dog_big";
		}else if(input["character"] == "Light+Armored+Dog+Female") {
			character_dir = "lt_dog_female";
		}else if(input["character"] == "Light+Armored+Dog+Male+1") {
			character_dir = "lt_dog_male_1";
		}else if(input["character"] == "Light+Armored+Dog+Male+2") {
			character_dir = "lt_dog_male_2";
		}else if(input["character"] == "Male+Cat") {
			character_dir = "male_cat";
		}else if(input["character"] == "Female+Cat") {
			character_dir = "female_cat";
		}else if(input["character"] == "Striped+Cat") {
			character_dir = "striped_cat";
		}else if(input["character"] == "Fancy+Striped+Cat") {
			character_dir = "fancy_striped_cat";
		}else if(input["character"] == "Male+Rabbit+1") {
			character_dir = "male_rabbit_1";
		}else if(input["character"] == "Male+Rabbit+2") {
			character_dir = "male_rabbit_2";
		}else if(input["character"] == "Male+Rabbit+3") {
			character_dir = "male_rabbit_3";
		}else if(input["character"] == "Male+Wolf") {
			character_dir = "male_wolf";
		}else if(input["character"] == "Civilian") {
			character_dir = "civ";
		}else if(input["character"] == "Pale+Rabbit+Civilian") {
			character_dir = "pale_rabbit_civ";
		}else if(input["character"] == "Rabbot") {
			character_dir = "rabbot";
		}else if(input["character"] == "Turner") {
			character_dir = "turner";
		}else if(input["character"] == "Wolf") {
			character_dir = "wolf";
		}
		new_player->set_character(character_dir);

		// Add the client to the client list.
		client_manager_.add_client(new_player);

		// Create answer for current client.
		string_map_vector answer;

		// First add a signon command.
		string_map signon;

		signon["type"] = "SignOn";
		signon["uid"] = new_uid;
		signon["refr"] = to_string(config_->get_update_refresh_rate());
		signon["welcome_message"] = config_->get_welcome_message();
		signon["username"] = new_player->get_username();
		signon["team"] = new_player->get_team();
		signon["character"] = character_dir;
		signon["server"] = input["server"];

		answer.push_back(signon);

		// Now add join commands for all other clients in the same group.
		client_map other_clients= client_manager_.get_clients(new_player);

		for(auto& item: other_clients) {
			string_map join;

			join["type"] = "SpawnCharacter";
			join["username"] = (item.second)->get_username();
			join["team"] = (item.second)->get_team();
			join["character"] = (item.second)->get_character();
			join["posx"] = to_string((item.second)->get_posx());
			join["posy"] = to_string((item.second)->get_posy());
			join["posz"] = to_string((item.second)->get_posz());

			answer.push_back(join);
		}

		new_reply.content= encode_output(answer);

		// Send message command to other players.
		string_map message;

		message["type"] = "Message";
		message["name"] = "server";
		message["text"] = new_player->get_username() + " has entered the room.";
		message["notif"] = "true";

		client_manager_.add_command(message, new_player);

		// Write the message to the logs.
		log::print(message["text"]);

		// Send join command to other players.
		string_map join;

		join["type"] = "SpawnCharacter";
		join["username"] = new_player->get_username();
		join["team"] = new_player->get_team();
		join["character"] = new_player->get_character();
		join["posx"] = to_string(new_player->get_posx());
		join["posy"] = to_string(new_player->get_posy());
		join["posz"] = to_string(new_player->get_posz());

		client_manager_.add_command(join, new_player);

		// Stop and send reply.
		return true;
	} else if(input["type"] ==  "Update") {
		// Update player states.
		player->set_posx(stof(input["posx"]));
		player->set_posy(stof(input["posy"]));
		player->set_posz(stof(input["posz"]));
		player->set_dirx(stof(input["dirx"]));
		player->set_dirz(stof(input["dirz"]));

		player->set_crouch((input["crouch"] == "true"));
		player->set_jump((input["jump"] == "true"));
		player->set_attack((input["attack"] == "true"));
		player->set_grab((input["grab"] == "true"));
		player->set_item((input["item"] == "true"));
		player->set_drop((input["drop"] == "true"));
		player->set_roll((input["roll"] == "true"));
		player->set_jumpoffwall((input["offwall"] == "true"));
		player->set_activeblock((input["activeblock"] == "true"));

		if((player->get_time_of_death() < 1) || (difftime(time(0), player->get_time_of_death()) > 10)) {
			// Do not allow players to increase some parts of their health.
			if(stof(input["blood_health"]) < player->get_blood_health()) {
				player->set_blood_health(stof(input["blood_health"]));
			}
			if(stof(input["permanent_health"]) < player->get_permanent_health()) {
				player->set_permanent_health(stof(input["permanent_health"]));
			}
			if(stoi(input["knocked_out"]) > player->get_knocked_out()) {
				player->set_knocked_out(stoi(input["knocked_out"]));
			}
			if(stoi(input["lives"]) < player->get_lives()) {
				player->set_lives(stoi(input["lives"]));
			}
			player->set_blood_damage(stof(input["blood_damage"]));
			player->set_block_health(stof(input["block_health"]));
			player->set_temp_health(stof(input["temp_health"]));
		}

		player->set_blood_delay(stoi(input["blood_delay"]));
		player->set_cut_throat((input["cut_throat"] == "true"));
		player->set_state(stoi(input["state"]));

		player->set_last_updated(difftime(time(0), start_));

		// Prepare the answer.
		string_map_vector answer;

		//If there are new players signing on then first return these commands
		//The other commands will be send on the next update
		if(player->contains_signon()){
			for(auto &command : player->get_signon_commands()){
				answer.push_back(command);
			}
			return true;
		}

		// Add commands from queue if available.
		while(player->get_number_of_commands() != 0) {
			answer.push_back(player->get_command());
		}

		// Check if the player died (we consider unconscious as dead for now).
		if((player->get_permanent_health() <= 0.0f)
		|| (player->get_blood_health() <= 0.0f)
		|| (player->get_temp_health() <= 0.0f)
		|| (player->get_knocked_out() == _dead)
		|| (player->get_lives() < 0)) {
			// Only announce death once.
			if(!player->get_death_changed()) {
				player->set_death_changed(true);
				player->set_time_of_death(time(0));

				// Send message to all players in the group.
				string_map message;

				message["type"] = "Message";
				message["name"] = "server";
				message["text"] = player->get_username() + " has died.";
				message["notif"] = "true";

				client_manager_.add_command(message, player);

				// Also send the message to player himself.
				answer.push_back(message);
			} else {
				// Revive the player after some seconds (experimental).
				if(difftime(time(0), player->get_time_of_death()) > 5) {
					player->set_blood_health(1.0f);
					player->set_permanent_health(1.0f);
					player->set_blood_damage(0.0f);
					player->set_block_health(1.0f);
					player->set_temp_health(1.0f);
					player->set_knocked_out(_awake);
					player->set_lives(1);
					player->set_blood_amount(10.0f);
					player->set_recovery_time(0.0f);
					player->set_roll_recovery_time(0.0f);
					player->set_remove_blood(true);
					player->set_cut_throat(false);
				}
			}
		} else {
			player->set_death_changed(false);
			player->set_remove_blood(false);
		}

		// Send health back to player.
		string_map update_self;

		update_self["type"] = "UpdateSelf";
		update_self["blood_damage"] = to_string(player->get_blood_damage());
		update_self["blood_health"] = to_string(player->get_blood_health());
		update_self["block_health"] = to_string(player->get_block_health());
		update_self["temp_health"] = to_string(player->get_temp_health());
		update_self["permanent_health"] = to_string(player->get_permanent_health());
		update_self["knocked_out"] = to_string(player->get_knocked_out());
		update_self["lives"] = to_string(player->get_lives());
		update_self["blood_amount"] = to_string(player->get_blood_amount());
		update_self["recovery_time"] = to_string(player->get_recovery_time());
		update_self["roll_recovery_time"] = to_string(player->get_roll_recovery_time());
		update_self["remove_blood"] = to_string(player->get_remove_blood());
		update_self["cut_throat"] = to_string(player->get_cut_throat());

		answer.push_back(update_self);

		// Get states of the other clients.
		client_map other_clients= client_manager_.get_clients(player);

		for(auto& item: other_clients) {
			string_map update;

			update["type"] = "Update";
			update["username"] = (item.second)->get_username();
			update["posx"] = to_string((item.second)->get_posx());
			update["posy"] = to_string((item.second)->get_posy());
			update["posz"] = to_string((item.second)->get_posz());
			update["dirx"] = to_string((item.second)->get_dirx());
			update["dirz"] = to_string((item.second)->get_dirz());
			update["crouch"] = to_string((item.second)->get_crouch());
			update["jump"] = to_string((item.second)->get_jump());
			update["attack"] = to_string((item.second)->get_attack());
			update["grab"] = to_string((item.second)->get_grab());
			update["item"] = to_string((item.second)->get_item());
			update["drop"] = to_string((item.second)->get_drop());
			update["roll"] = to_string((item.second)->get_roll());
			update["offwall"] = to_string((item.second)->get_jumpoffwall());
			update["activeblock"] = to_string((item.second)->get_activeblock());
			update["blood_damage"] = to_string((item.second)->get_blood_damage());
			update["blood_health"] = to_string((item.second)->get_blood_health());
			update["block_health"] = to_string((item.second)->get_block_health());
			update["temp_health"] = to_string((item.second)->get_temp_health());
			update["permanent_health"] = to_string((item.second)->get_permanent_health());
			update["knocked_out"] = to_string((item.second)->get_knocked_out());
			update["lives"] = to_string((item.second)->get_lives());
			update["blood_amount"] = to_string((item.second)->get_blood_amount());
			update["recovery_time"] = to_string((item.second)->get_recovery_time());
			update["roll_recovery_time"] = to_string((item.second)->get_roll_recovery_time());
			update["remove_blood"] = to_string((item.second)->get_remove_blood());
			update["blood_delay"] = to_string((item.second)->get_blood_delay());
			update["cut_throat"] = to_string((item.second)->get_cut_throat());
			update["state"] = to_string((item.second)->get_state());

			answer.push_back(update);
		}

		new_reply.content= encode_output(answer);

		// Stop and send reply.
		return true;
	} else if(input["type"] == "SavePosition") {
		// Don't continue if disabled.
		if(!config_->get_allow_teleport()) {
			return true;
		}

		// Create copies of the client coordinates.
		player->set_saved_posx(player->get_posx());
		player->set_saved_posy(player->get_posy());
		player->set_saved_posz(player->get_posz());

		// Stop and send reply.
		return true;
	} else if(input["type"] == "LoadPosition") {
		// Don't continue if disabled.
		if(!config_->get_allow_teleport()) {
			return true;
		}

		// Create answer.
		string_map answer;

		answer["type"] = "LoadPosition";
		answer["posx"] = to_string(player->get_saved_posx());
		answer["posy"] = to_string(player->get_saved_posy());
		answer["posz"] = to_string(player->get_saved_posz());

		new_reply.content= encode_output(answer);

		// Stop and send reply.
		return true;
	} else if(input["type"] == "Message") {
		// Create message.
		string_map message;

		message["type"] = "Message";
		message["name"] = input["name"];
		message["text"] = input["text"];
		message["notif"] = "false";

		// Send message to everyone except the current client.
		client_manager_.add_command(message, player);

		// Write the message to the logs.
		log::print(message["name"] + ": " + message["text"]);

		// Stop and send reply.
		return true;
	}

	return false;
}

void request_handler::handle_json_command(boost::property_tree::ptree& pt, stack<reply>& rep, client& this_client){
	cout << "type " << pt.get<std::string>("type") << endl;
	
	string message_type = pt.get<std::string>("type");
	boost::property_tree::ptree content = pt.get_child("content");
	
	if(!this_client.get_signed_on()){
		//The client only has access to the signon command at first.
		if(message_type == "SignOn"){
			HandleSignOn(content, rep, this_client);
		}else{
			AddErrorMessage(rep, "Not yet signed on!");
		}
		
	}else{
		if(message_type == "Update"){
			HandleUpdate(content, rep, this_client);
		}
		else if(message_type == "Message"){
			
		}
		else if(message_type == "SavePosition"){
			
		}
		else if(message_type == "LoadPosition"){
			
		}
		
	}
	
}

void request_handler::AddErrorMessage(stack<reply>& rep, string message){
	reply new_reply;
	new_reply.json = true;
	boost::property_tree::ptree answer;
	answer.put("type", "Error");
	answer.put("content.message", message);
	new_reply.content= jsonToString(answer);
	rep.push(new_reply);
}

string request_handler::jsonToString(boost::property_tree::ptree& json){
	std::ostringstream oss;
	write_json(oss, json, false);
	//cout << "Reply: " << oss.str() << endl;
	return oss.str();
}

void request_handler::HandleSignOn(boost::property_tree::ptree& content, stack<reply>& rep, client& this_client){
	reply new_reply;
	new_reply.json = true;
	boost::property_tree::ptree answer;

	string character_dir = "turner";
	double signon_time= difftime(time(0), start_);

	// Check if someone is already using that username.
	client_map all_clients= client_manager_.get_clients();

	for(auto& item: all_clients) {
		if((item.second)->get_username() == content.get<string>("username")) {
			AddErrorMessage(rep, "Already a user with that username!");
			// Stop and send reply.
			break;
		}
	}

	// Create a new client.
	client_ptr new_player(new client());

	this_client.set_level(content.get<string>("level"));
	this_client.set_username(content.get<string>("username"));
	this_client.set_team(content.get<string>("username"));
	
	this_client.set_posx(stof(content.get<string>("posx")));
	this_client.set_posy(stof(content.get<string>("posy")));
	this_client.set_posz(stof(content.get<string>("posz")));

	// Set default teleport to the spawn position.
	this_client.set_saved_posx(stof(content.get<string>("posx")));
	this_client.set_saved_posy(stof(content.get<string>("posy")));
	this_client.set_saved_posz(stof(content.get<string>("posz")));
	
	if(content.get<string>("character") == "Guard") {
		character_dir = "guard";
	}else if(content.get<string>("character") == "Raider+Rabbit") {
		character_dir = "raider_rabbit";
	}else if(content.get<string>("character") == "Pale+Turner") {
		character_dir = "pale_turner";
	}else if(content.get<string>("character") == "Guard+2") {
		character_dir = "guard2";
	}else if(content.get<string>("character") == "Base+Guard") {
		character_dir = "base_guard";
	}else if(content.get<string>("character") == "Cat") {
		character_dir = "cat";
	}else if(content.get<string>("character") == "Female+Rabbit+1") {
		character_dir = "female_rabbit_1";
	}else if(content.get<string>("character") == "Female+Rabbit+2") {
		character_dir = "female_rabbit_2";
	}else if(content.get<string>("character") == "Female+Rabbit+3") {
		character_dir = "female_rabbit_3";
	}else if(content.get<string>("character") == "Rat") {
		character_dir = "rat";
	}else if(content.get<string>("character") == "Female+Rat") {
		character_dir = "female_rat";
	}else if(content.get<string>("character") == "Hooded+Rat") {
		character_dir = "hooded_rat";
	}else if(content.get<string>("character") == "Light+Armored+Dog+Big") {
		character_dir = "lt_dog_big";
	}else if(content.get<string>("character") == "Light+Armored+Dog+Female") {
		character_dir = "lt_dog_female";
	}else if(content.get<string>("character") == "Light+Armored+Dog+Male+1") {
		character_dir = "lt_dog_male_1";
	}else if(content.get<string>("character") == "Light+Armored+Dog+Male+2") {
		character_dir = "lt_dog_male_2";
	}else if(content.get<string>("character") == "Male+Cat") {
		character_dir = "male_cat";
	}else if(content.get<string>("character") == "Female+Cat") {
		character_dir = "female_cat";
	}else if(content.get<string>("character") == "Striped+Cat") {
		character_dir = "striped_cat";
	}else if(content.get<string>("character") == "Fancy+Striped+Cat") {
		character_dir = "fancy_striped_cat";
	}else if(content.get<string>("character") == "Male+Rabbit+1") {
		character_dir = "male_rabbit_1";
	}else if(content.get<string>("character") == "Male+Rabbit+2") {
		character_dir = "male_rabbit_2";
	}else if(content.get<string>("character") == "Male+Rabbit+3") {
		character_dir = "male_rabbit_3";
	}else if(content.get<string>("character") == "Male+Wolf") {
		character_dir = "male_wolf";
	}else if(content.get<string>("character") == "Civilian") {
		character_dir = "civ";
	}else if(content.get<string>("character") == "Pale+Rabbit+Civilian") {
		character_dir = "pale_rabbit_civ";
	}else if(content.get<string>("character") == "Rabbot") {
		character_dir = "rabbot";
	}else if(content.get<string>("character") == "Turner") {
		character_dir = "turner";
	}else if(content.get<string>("character") == "Wolf") {
		character_dir = "wolf";
	}
	this_client.set_character(character_dir);
	
	// Add the client to the client list.
	client_manager_.add_client(new_player);
	
	answer.put("type", "SignOn");
	answer.put("content.refresh_rate", to_string(config_->get_update_refresh_rate()));
	answer.put("content.welcome_message", config_->get_welcome_message());
	answer.put("content.username", this_client.get_username());
	answer.put("content.team", this_client.get_team());
	answer.put("content.character", character_dir);

	//When the signon is successful 
	this_client.set_signed_on(true);

	new_reply.content= jsonToString(answer);
	rep.push(new_reply);

	client_ptr client_pointer = boost::make_shared<client>(this_client);
	
	// Now add join commands for all other clients in the same group.
	client_map other_clients = client_manager_.get_clients(client_pointer);

	for(auto& item: other_clients) {
		reply spawn_character_reply;
		boost::property_tree::ptree spawn_command;

		spawn_command.put("type", "SpawnCharacter");
		spawn_command.put("username", (item.second)->get_username());
		spawn_command.put("team", (item.second)->get_team());
		spawn_command.put("character", (item.second)->get_character());
		spawn_command.put("posx", to_string((item.second)->get_posx()));
		spawn_command.put("posy", to_string((item.second)->get_posy()));
		spawn_command.put("posz", to_string((item.second)->get_posz()));

		spawn_character_reply.content= jsonToString(spawn_command);
		rep.push(spawn_character_reply);
	}

	// Send message command to other players.
	reply message_reply;
	boost::property_tree::ptree new_player_joined_message;

	new_player_joined_message.put("type", "Message");
	new_player_joined_message.put("name", "server");
	new_player_joined_message.put("text", new_player->get_username() + " has entered the room.");
	new_player_joined_message.put("notif", "true");
	
	message_reply.content= jsonToString(new_player_joined_message);

	//client_manager_.add_command(message, new_player);

	// Write the message to the logs.
	log::print(new_player_joined_message.get<string>("text"));

	// Send join command to other players.
	reply spawn_character_reply;
	boost::property_tree::ptree spawn_command;

	spawn_command.put("type", "SpawnCharacter");
	spawn_command.put("username", this_client.get_username());
	spawn_command.put("team", this_client.get_team());
	spawn_command.put("character", this_client.get_character());
	spawn_command.put("posx", to_string(this_client.get_posx()));
	spawn_command.put("posy", to_string(this_client.get_posy()));
	spawn_command.put("posz", to_string(this_client.get_posz()));

	spawn_character_reply.content= jsonToString(spawn_command);

	//client_manager_.add_command(join, new_player);
}

void request_handler::HandleUpdate(boost::property_tree::ptree& content, stack<reply>& rep, client& this_client){
	reply new_reply;
	new_reply.json = true;
	boost::property_tree::ptree answer;
	answer.put("type", "Update");
	answer.put("content.message", "update message");
	
	new_reply.content= jsonToString(answer);
	rep.push(new_reply);
}

void request_handler::handle_request(const request& req, stack<reply>& rep, client& this_client) {
	if(req.json){
		cout << "It's JSON!" << "\n";
		std::stringstream ss;
		ss << req.content;
		boost::property_tree::ptree pt;
		read_json(ss, pt);
		handle_json_command(pt , rep, this_client);
		return;
	}
	// Decode url to path.
	string request_path;
	if(!url_decode(req.uri, request_path)) {
		rep.push(reply::stock_reply(reply::bad_request));
		return;
	}

	if(config_->get_debug()) {
		cout << "req.uri: " << req.uri << "\trequest_path: " << request_path << "\n";
	}

	// Parse post content.
	if(!req.content.empty()) {
		if(config_->get_debug()) {
			cout << "post: " << req.content << "\n";
		}
		string_map input;
		string content= req.content;

		string_vector separated_content;
		separated_content = seperate_string(content, "&");

		for(auto& item: separated_content) {
			string_vector name_value= seperate_string(item, "=");

			if (name_value.size() == 2) {
				input[name_value[0]]= name_value[1];
			}
		}

		try {
			if(handle_command(input, rep)) {
				return prepare_reply(rep);
			}
		} catch(...) {
			std::cerr << "handle_command error: " << content << "\n";
			rep.push(reply::stock_reply(reply::bad_request));
			return;
		}

		// If we are still in this method there was no command handler.
		rep.push(reply::stock_reply(reply::not_found));
		return;
	}

	// Request path must be absolute and not contain "..".
	if(request_path.empty() || request_path[0] != '/' || request_path.find("..") != string::npos) {
		rep.push(reply::stock_reply(reply::bad_request));
		return;
	}

	// If path ends in slash (i.e. is a directory) then add "index.html".
	if(request_path[request_path.size() - 1] == '/') {
		request_path+= "index.html";
	}

	// Determine the file extension.
	size_t last_slash_pos= request_path.find_last_of("/");
	size_t last_dot_pos= request_path.find_last_of(".");
	string extension;

	if(last_dot_pos != string::npos && last_dot_pos > last_slash_pos) {
		extension= request_path.substr(last_dot_pos + 1);
	}

	// Open the file to send back.
	string full_path= doc_root_ + request_path;
	ifstream is(full_path.c_str(), ios::in | ios::binary);

	if(!is) {
		rep.push(reply::stock_reply(reply::not_found));
		return;
	}

	// Fill out the reply to be sent to the client.
	rep.top().status = reply::ok;
	char buf[512];

	while(is.read(buf, sizeof(buf)).gcount() > 0) {
		rep.top().content.append(buf, is.gcount());
	}

	return prepare_reply(rep, extension);
}

void request_handler::prepare_reply(stack<reply>& rep, string extension) {
	reply& top_reply = rep.top();
	top_reply.status= reply::ok;

	top_reply.headers.resize(2);
	top_reply.headers[0].name= "Content-Length";
	top_reply.headers[0].value= to_string(top_reply.content.size());
	top_reply.headers[1].name= "Content-Type";
	top_reply.headers[1].value= mime_types::extension_to_type(extension);
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
