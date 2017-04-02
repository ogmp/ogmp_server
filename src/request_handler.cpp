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
	} else if(input["type"] ==  "Update") {
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

	
}

void request_handler::AddErrorMessage(stack<reply>& rep, string message){
	reply new_reply;
	boost::property_tree::ptree answer;
	answer.put("type", "Error");
	answer.put("content.message", message);
	new_reply.json = true;
	new_reply.content= jsonToString(answer);
	rep.push(new_reply);
}

string request_handler::jsonToString(boost::property_tree::ptree& json){
	std::ostringstream oss;
	write_json(oss, json, false);
	//cout << "Reply: " << oss.str() << endl;
	return oss.str();
}

string request_handler::GetString(int size){
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

void request_handler::HandleSignOn(stack<reply>& rep, client& this_client){
	reply new_reply;

	string username = GetString(username_size);
	string character = GetString(character_size);
	string levelname = GetString(level_size);
	string version = GetString(version_size);
	
	float posx = GetFloat();
	float posy = GetFloat();
	float posz = GetFloat();
	
	cout << " Username " << username;
	cout << " character " << character;
	cout << " levelname " << levelname;
	cout << " version " << version;
	cout << " posx " << posx;
	cout << " posy " << posy;
	cout << " posz " << posz;
	cout << endl;

	string character_dir = "turner";
	double signon_time= difftime(time(0), start_);
	
	// Check if someone is already using that username.
	client_map all_clients= client_manager_.get_clients();
	
	for(auto& item: all_clients) {
		if((item.second)->get_username() == username) {
			AddErrorMessage(rep, "Already a user with that username!");
			// Stop and send reply.
			break;
		}
	}
	
	this_client.set_level(levelname);
	this_client.set_username(username);
	this_client.set_team(username);
	
	this_client.set_posx(posx);
	this_client.set_posy(posy);
	this_client.set_posz(posz);
	
	// Set default teleport to the spawn position.
	this_client.set_saved_posx(posx);
	this_client.set_saved_posy(posy);
	this_client.set_saved_posz(posz);
	
	if(character == "Guard") {
		character_dir = "guard";
	}else if(character == "Raider+Rabbit") {
		character_dir = "raider_rabbit";
	}else if(character == "Pale+Turner") {
		character_dir = "pale_turner";
	}else if(character == "Guard+2") {
		character_dir = "guard2";
	}else if(character == "Base+Guard") {
		character_dir = "base_guard";
	}else if(character == "Cat") {
		character_dir = "cat";
	}else if(character == "Female+Rabbit+1") {
		character_dir = "female_rabbit_1";
	}else if(character == "Female+Rabbit+2") {
		character_dir = "female_rabbit_2";
	}else if(character == "Female+Rabbit+3") {
		character_dir = "female_rabbit_3";
	}else if(character == "Rat") {
		character_dir = "rat";
	}else if(character == "Female+Rat") {
		character_dir = "female_rat";
	}else if(character == "Hooded+Rat") {
		character_dir = "hooded_rat";
	}else if(character == "Light+Armored+Dog+Big") {
		character_dir = "lt_dog_big";
	}else if(character == "Light+Armored+Dog+Female") {
		character_dir = "lt_dog_female";
	}else if(character == "Light+Armored+Dog+Male+1") {
		character_dir = "lt_dog_male_1";
	}else if(character == "Light+Armored+Dog+Male+2") {
		character_dir = "lt_dog_male_2";
	}else if(character == "Male+Cat") {
		character_dir = "male_cat";
	}else if(character == "Female+Cat") {
		character_dir = "female_cat";
	}else if(character == "Striped+Cat") {
		character_dir = "striped_cat";
	}else if(character == "Fancy+Striped+Cat") {
		character_dir = "fancy_striped_cat";
	}else if(character == "Male+Rabbit+1") {
		character_dir = "male_rabbit_1";
	}else if(character == "Male+Rabbit+2") {
		character_dir = "male_rabbit_2";
	}else if(character == "Male+Rabbit+3") {
		character_dir = "male_rabbit_3";
	}else if(character == "Male+Wolf") {
		character_dir = "male_wolf";
	}else if(character == "Civilian") {
		character_dir = "civ";
	}else if(character == "Pale+Rabbit+Civilian") {
		character_dir = "pale_rabbit_civ";
	}else if(character == "Rabbot") {
		character_dir = "rabbot";
	}else if(character == "Turner") {
		character_dir = "turner";
	}else if(character == "Wolf") {
		character_dir = "wolf";
	}
	this_client.set_character(character_dir);
	
	client_ptr client_pointer = boost::make_shared<client>(this_client);
	
	// Add the client to the client list.
	client_manager_.add_client(client_pointer);
	
	new_reply.add_to_buffers(SignOn);
	new_reply.add_to_buffers('g');
	// new_reply.add_to_buffers(config_->get_update_refresh_rate());
	
	// answer.put("type", "SignOn");
	// answer.put("content.refresh_rate", to_string(config_->get_update_refresh_rate()));
	// answer.put("content.welcome_message", config_->get_welcome_message());
	// answer.put("content.username", this_client.get_username());
	// answer.put("content.team", this_client.get_team());
	// answer.put("content.character", character_dir);
	
	//When the signon is successful 
	this_client.set_signed_on(true);
	// 
	// new_reply.json = true;
	// new_reply.content= jsonToString(answer);
	rep.push(new_reply);
	// 
	// 
	// // Now add join commands for all other clients in the same group.
	// client_map other_clients = client_manager_.get_clients(client_pointer);
	// 
	// cout << "The client manager has " << other_clients.size() << " clients" << endl;
	// 
	// for(auto& item: other_clients) {
	// 	reply spawn_character_reply;
	// 	boost::property_tree::ptree spawn_command;
	// 	
	// 	//cout << "Adding SpawnCharacter command!" << endl;
	// 
	// 	spawn_command.put("type", "SpawnCharacter");
	// 	spawn_command.put("content.username", (item.second)->get_username());
	// 	spawn_command.put("content.team", (item.second)->get_team());
	// 	spawn_command.put("content.character", (item.second)->get_character());
	// 	spawn_command.put("content.posx", to_string((item.second)->get_posx()));
	// 	spawn_command.put("content.posy", to_string((item.second)->get_posy()));
	// 	spawn_command.put("content.posz", to_string((item.second)->get_posz()));
	// 
	// 	spawn_character_reply.json = true;
	// 	spawn_character_reply.content= jsonToString(spawn_command);
	// 	//rep.push(spawn_character_reply);
	// }
	// 
	// // Send message command to other players.
	// reply message_reply;
	// boost::property_tree::ptree new_player_joined_message;
	// 
	// new_player_joined_message.put("type", "Message");
	// new_player_joined_message.put("name", "server");
	// new_player_joined_message.put("text", client_pointer->get_username() + " has entered the room.");
	// new_player_joined_message.put("notif", "true");
	// 
	// message_reply.json = true;
	// message_reply.content= jsonToString(new_player_joined_message);
	// 
	// //client_manager_.add_command(message, new_player);
	// 
	// // Write the message to the logs.
	// log::print(new_player_joined_message.get<string>("text"));
	// 
	// // Send join command to other players.
	// reply spawn_character_reply;
	// boost::property_tree::ptree spawn_command;
	// 
	// spawn_command.put("type", "SpawnCharacter");
	// spawn_command.put("content.username", this_client.get_username());
	// spawn_command.put("content.team", this_client.get_team());
	// spawn_command.put("content.character", this_client.get_character());
	// spawn_command.put("content.posx", to_string(this_client.get_posx()));
	// spawn_command.put("content.posy", to_string(this_client.get_posy()));
	// spawn_command.put("content.posz", to_string(this_client.get_posz()));
	// 
	// spawn_character_reply.json = true;
	// spawn_character_reply.content= jsonToString(spawn_command);

	//client_manager_.add_command(join, new_player);
}

void request_handler::HandleUpdate(boost::property_tree::ptree& content, stack<reply>& rep, client& this_client){
	// Update player states.
	this_client.set_posx(stof(content.get<string>("posx")));
	this_client.set_posy(stof(content.get<string>("posy")));
	this_client.set_posz(stof(content.get<string>("posz")));
	this_client.set_dirx(stof(content.get<string>("dirx")));
	this_client.set_dirz(stof(content.get<string>("dirz")));

	this_client.set_crouch((content.get<string>("crouch") == "true"));
	this_client.set_jump((content.get<string>("jump") == "true"));
	this_client.set_attack((content.get<string>("attack") == "true"));
	this_client.set_grab((content.get<string>("grab") == "true"));
	this_client.set_item((content.get<string>("item") == "true"));
	this_client.set_drop((content.get<string>("drop") == "true"));
	this_client.set_roll((content.get<string>("roll") == "true"));
	this_client.set_jumpoffwall((content.get<string>("offwall") == "true"));
	this_client.set_activeblock((content.get<string>("activeblock") == "true"));
	
	if((this_client.get_time_of_death() < 1) || (difftime(time(0), this_client.get_time_of_death()) > 10)) {
		// Do not allow this_clients to increase some parts of their health.
		if(stof(content.get<string>("blood_health")) < this_client.get_blood_health()) {
			this_client.set_blood_health(stof(content.get<string>("blood_health")));
		}
		if(stof(content.get<string>("permanent_health")) < this_client.get_permanent_health()) {
			this_client.set_permanent_health(stof(content.get<string>("permanent_health")));
		}
		if(stoi(content.get<string>("knocked_out")) > this_client.get_knocked_out()) {
			this_client.set_knocked_out(stoi(content.get<string>("knocked_out")));
		}
		if(stoi(content.get<string>("lives")) < this_client.get_lives()) {
			this_client.set_lives(stoi(content.get<string>("lives")));
		}
		this_client.set_blood_damage(stof(content.get<string>("blood_damage")));
		this_client.set_block_health(stof(content.get<string>("block_health")));
		this_client.set_temp_health(stof(content.get<string>("temp_health")));
	}

	this_client.set_blood_delay(stoi(content.get<string>("blood_delay")));
	this_client.set_cut_throat((content.get<string>("cut_throat") == "true"));
	this_client.set_state(stoi(content.get<string>("state")));

	client_ptr client_pointer = boost::make_shared<client>(this_client);

	// Add commands from queue if available.
	while(this_client.get_number_of_commands() != 0) {
		reply command_reply;
		boost::property_tree::ptree command_tree;
		
		string_map message = this_client.get_command();
		
		command_tree.put("type", message["type"]);
		command_tree.put("content.text", message["text"]);
		command_tree.put("content.notif", message["notif"]);
		
		command_reply.json = true;
		command_reply.content= jsonToString(command_tree);
		rep.push(command_reply);
	}

	// Check if the player died (we consider unconscious as dead for now).
	if((this_client.get_permanent_health() <= 0.0f)
	|| (this_client.get_blood_health() <= 0.0f)
	|| (this_client.get_temp_health() <= 0.0f)
	|| (this_client.get_knocked_out() == _dead)
	|| (this_client.get_lives() < 0)) {
		// Only announce death once.
		if(!this_client.get_death_changed()) {
			this_client.set_death_changed(true);
			this_client.set_time_of_death(time(0));

			// Send message to all players in the group.
			string_map message;

			message["type"] = "Message";
			message["name"] = "server";
			message["text"] = this_client.get_username() + " has died.";
			message["notif"] = "true";

			client_manager_.add_command(message, client_pointer);

			// Also send the message to player himself.
			this_client.add_command(message);
		} else {
			// Revive the player after some seconds (experimental).
			if(difftime(time(0), this_client.get_time_of_death()) > 5) {
				this_client.set_blood_health(1.0f);
				this_client.set_permanent_health(1.0f);
				this_client.set_blood_damage(0.0f);
				this_client.set_block_health(1.0f);
				this_client.set_temp_health(1.0f);
				this_client.set_knocked_out(_awake);
				this_client.set_lives(1);
				this_client.set_blood_amount(10.0f);
				this_client.set_recovery_time(0.0f);
				this_client.set_roll_recovery_time(0.0f);
				this_client.set_remove_blood(true);
				this_client.set_cut_throat(false);
			}
		}
	} else {
		this_client.set_death_changed(false);
		this_client.set_remove_blood(false);
	}
	
	// Send health back to player.
	reply updateself_reply;
	boost::property_tree::ptree updateself_tree;
	
	updateself_tree.put("type", "UpdateSelf");
	updateself_tree.put("content.blood_damage", to_string(this_client.get_blood_damage()));
	updateself_tree.put("content.blood_health", to_string(this_client.get_blood_health()));
	updateself_tree.put("content.block_health", to_string(this_client.get_block_health()));
	updateself_tree.put("content.temp_health", to_string(this_client.get_temp_health()));
	updateself_tree.put("content.permanent_health", to_string(this_client.get_permanent_health()));
	updateself_tree.put("content.knocked_out", to_string(this_client.get_knocked_out()));
	updateself_tree.put("content.lives", to_string(this_client.get_lives()));
	updateself_tree.put("content.blood_amount", to_string(this_client.get_blood_amount()));
	updateself_tree.put("content.recovery_time", to_string(this_client.get_recovery_time()));
	updateself_tree.put("content.roll_recovery_time", to_string(this_client.get_roll_recovery_time()));
	updateself_tree.put("content.remove_blood", to_string(this_client.get_remove_blood()));
	updateself_tree.put("content.cut_throat", to_string(this_client.get_cut_throat()));

	updateself_reply.json = true;
	updateself_reply.content= jsonToString(updateself_tree);
	//TODO maybe just send this when needed.
	//rep.push(updateself_reply);

	// Get states of the other clients.
	client_map other_clients = client_manager_.get_clients(client_pointer);

	for(auto& item: other_clients) {
		reply update_reply;
		boost::property_tree::ptree update_tree;
		
		update_tree.put("type", "Update");

		update_tree.put("content.username" , (item.second)->get_username());
		update_tree.put("content.posx" , to_string((item.second)->get_posx()));
		update_tree.put("content.posy" , to_string((item.second)->get_posy()));
		update_tree.put("content.posz" , to_string((item.second)->get_posz()));
		update_tree.put("content.dirx" , to_string((item.second)->get_dirx()));
		update_tree.put("content.dirz" , to_string((item.second)->get_dirz()));
		update_tree.put("content.crouch" , to_string((item.second)->get_crouch()));
		update_tree.put("content.jump" , to_string((item.second)->get_jump()));
		update_tree.put("content.attack" , to_string((item.second)->get_attack()));
		update_tree.put("content.grab" , to_string((item.second)->get_grab()));
		update_tree.put("content.item" , to_string((item.second)->get_item()));
		update_tree.put("content.drop" , to_string((item.second)->get_drop()));
		update_tree.put("content.roll" , to_string((item.second)->get_roll()));
		update_tree.put("content.offwall" , to_string((item.second)->get_jumpoffwall()));
		update_tree.put("content.activeblock" , to_string((item.second)->get_activeblock()));
		update_tree.put("content.blood_damage" , to_string((item.second)->get_blood_damage()));
		update_tree.put("content.blood_health" , to_string((item.second)->get_blood_health()));
		update_tree.put("content.block_health" , to_string((item.second)->get_block_health()));
		update_tree.put("content.temp_health" , to_string((item.second)->get_temp_health()));
		update_tree.put("content.permanent_health" , to_string((item.second)->get_permanent_health()));
		update_tree.put("content.knocked_out" , to_string((item.second)->get_knocked_out()));
		update_tree.put("content.lives" , to_string((item.second)->get_lives()));
		update_tree.put("content.blood_amount" , to_string((item.second)->get_blood_amount()));
		update_tree.put("content.recovery_time" , to_string((item.second)->get_recovery_time()));
		update_tree.put("content.roll_recovery_time" , to_string((item.second)->get_roll_recovery_time()));
		update_tree.put("content.remove_blood" , to_string((item.second)->get_remove_blood()));
		update_tree.put("content.blood_delay" , to_string((item.second)->get_blood_delay()));
		update_tree.put("content.cut_throat" , to_string((item.second)->get_cut_throat()));
		update_tree.put("content.state" , to_string((item.second)->get_state()));

		update_reply.json = true;
		update_reply.content= jsonToString(update_tree);
		rep.push(update_reply);
	}
}

void request_handler::handle_request(const request& req, stack<reply>& rep, client& this_client, char* data_, std::size_t bytes_transferred) {
	cout << "Printing message: " << endl;
	cout << req.content << endl;
	
	for(uint i = 0; i < bytes_transferred; i++){
		cout << (int)data[i] << " ";
	}
	cout << endl;
	
	data_index = 1;
	data = data_;
	switch(data[0]){
		case SignOn :
		{
			cout << "Received signon message" << endl;
			HandleSignOn(rep, this_client);
			break;
		}
		case UpdateGame :
		{
			cout << "Received UpdateGame message" << endl;
			break;
		}
		case UpdateSelf :
		{
			cout << "Received UpdateSelf message" << endl;
			break;
		}
		case Message :
		{
			cout << "Received Message message" << endl;
			break;
		}
		default :
		{
			cout << "Received Unknown message" << endl;
		}
	}
	return;
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
