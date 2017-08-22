#include "client.hpp"
#include "reply.hpp"

namespace http {
namespace server {

using namespace std;

client::client() : uid_(""), username_(""), team_(""), character_(""), level_path_(""), level_name_(""),
posx_(0.0f), posy_(0.0f), posz_(0.0f), dirx_(0.0f), dirz_(0.0f), saved_posx_(0.0f),
saved_posy_(0.0f), saved_posz_(0.0f), iscrouching_(false), isjumping_(false),
isattacking_(false), isgrabbing_(false), isusingitem_(false), isdropping_(false),
isrolling_(false), isjumpingoffwall_(false), isactiveblocking_(false),
blood_damage_(0.0f), blood_health_(1.0f), block_health_(1.0f), temp_health_(1.0f),
permanent_health_(1.0f), knocked_out_(_awake), blood_amount_(10.0f),
recovery_time_(0.0f), roll_recovery_time_(0.0f), ragdoll_type_(0), time_of_death_(0),
remove_blood_(false), blood_delay_(0), cut_throat_(false), state_(0), has_signed_on_(false) {
	variable_states.fill(false);
}

void client::set_all_variables_old(){
	variable_states.fill(false);
}

bool client::is_variable_new(int index){
	return variable_states[index];
}

void client::set_uid(string _uid) {
	uid_ = _uid;
}

void client::set_level_path(string _level_path) {
	level_path_ = _level_path;
}

void client::set_level_name(string _level_name) {
	level_name_ = _level_name;
}

void client::set_username(string _username) {
	username_ = _username;
}

void client::set_team(string _team) {
	team_ = _team;
}

void client::add_command(string_map _command) {
	commands_.push_back(_command);
}

void client::add_to_inbox(reply& _command) {
	inbox_.push_back(_command);
}

void client::set_last_updated(double _current_seconds) {
	last_updated_ = _current_seconds;
}

void client::set_character(string _character) {
	character_ = _character;
}

void client::set_posx(float _posx) {
	variable_states[position_x] = true;
	posx_ = _posx;
}

void client::set_posy(float _posy) {
	variable_states[position_y] = true;
	posy_ = _posy;
}

void client::set_posz(float _posz) {
	variable_states[position_z] = true;
	posz_ = _posz;
}

void client::set_saved_posx(float _posx) {
	saved_posx_ = _posx;
}

void client::set_saved_posy(float _posy) {
	saved_posy_ = _posy;
}

void client::set_saved_posz(float _posz) {
	saved_posz_ = _posz;
}

void client::set_crouch(bool _crouch) {
	variable_states[crouch] = true;
	iscrouching_ = _crouch;
}

void client::set_jump(bool _jump) {
	variable_states[jump] = true;
	isjumping_ = _jump;
}

void client::set_attack(bool _attack) {
	variable_states[attack] = true;
	isattacking_ = _attack;
}

void client::set_grab(bool _grab) {
	variable_states[grab] = true;
	isgrabbing_ = _grab;
}

void client::set_item(bool _item) {
	variable_states[item] = true;
	isusingitem_ = _item;
}

void client::set_drop(bool _drop) {
	variable_states[drop] = true;
	isdropping_ = _drop;
}

void client::set_roll(bool _roll) {
	isrolling_ = _roll;
}

void client::set_jumpoffwall(bool _offwall) {
	isjumpingoffwall_ = _offwall;
}

void client::set_activeblock(bool _activeblock) {
	isactiveblocking_ = _activeblock;
}

void client::set_blood_damage(float _blood_damage) {
	variable_states[blood_damage] = true;
	blood_damage_ = _blood_damage;
}

void client::set_blood_health(float _blood_health) {
	variable_states[blood_health] = true;
	blood_health_ = _blood_health;
}

void client::set_block_health(float _block_health) {
	variable_states[block_health] = true;
	block_health_ = _block_health;
}

void client::set_temp_health(float _temp_health) {
	variable_states[temp_health] = true;
	temp_health_ = _temp_health;
}

void client::set_permanent_health(float _permanent_health) {
	variable_states[permanent_health] = true;
	permanent_health_ = _permanent_health;
}

void client::set_knocked_out(int _knocked_out) {
	variable_states[knocked_out] = true;
	knocked_out_ = _knocked_out;
}

void client::set_death_changed(bool _death_changed) {
	death_changed_ = _death_changed;
}

void client::set_signed_on(bool _signed_on){
	has_signed_on_ = _signed_on;
}

void client::set_time_of_death(double _current_seconds) {
	time_of_death_ = _current_seconds;
}

void client::set_blood_amount(float _blood_amount) {
	variable_states[blood_amount] = true;
	blood_amount_ = _blood_amount;
}

void client::set_recovery_time(float _recovery_time) {
	variable_states[recovery_time] = true;
	recovery_time_ = _recovery_time;
}

void client::set_roll_recovery_time(float _roll_recovery_time) {
	variable_states[roll_recovery_time] = true;
	roll_recovery_time_ = _roll_recovery_time;
}

void client::set_ragdoll_type(int _ragdoll_type) {
	variable_states[ragdoll_type] = true;
	ragdoll_type_ = _ragdoll_type;
}

void client::set_remove_blood(bool _remove_blood) {
	variable_states[remove_blood] = true;
	remove_blood_ = _remove_blood;
}

void client::set_blood_delay(int _blood_delay) {
	variable_states[blood_delay] = true;
	blood_delay_ = _blood_delay;
}

void client::set_cut_throat(bool _cut_throat) {
	variable_states[cut_throat] = true;
	cut_throat_ = _cut_throat;
}

void client::set_state(int _state) {
	variable_states[state] = true;
	state_ = _state;
}

void client::set_dirx(float _dirx) {
	variable_states[direction_x] = true;
	dirx_ = _dirx;
}

void client::set_dirz(float _dirz) {
	variable_states[direction_z] = true;
	dirz_ = _dirz;
}

string client::get_uid() {
	return uid_;
}

string client::get_level_name() {
	return level_name_;
}

string client::get_level_path() {
	return level_path_;
}

string client::get_username() {
	return username_;
}

string client::get_team() {
	return team_;
}

string_map client::get_command() {
	string_map command = commands_.back();
	commands_.pop_back();
	return command;
}
reply client::get_inbox_message() {
	reply message = inbox_.back();
	inbox_.pop_back();
	return message;
}

string client::get_character() {
	return character_;
}

double client::get_last_updated() {
	return last_updated_;
}

int client::get_number_of_commands() {
	return commands_.size();
}

int client::get_number_of_inbox_messages() {
	return inbox_.size();
}

float client::get_posx() {
	return posx_;
}

float client::get_posy() {
	return posy_;
}

float client::get_posz() {
	return posz_;
}

float client::get_saved_posx() {
	return saved_posx_;
}

float client::get_saved_posy() {
	return saved_posy_;
}

float client::get_saved_posz() {
	return saved_posz_;
}

float client::get_dirx() {
	return dirx_;
}

float client::get_dirz() {
	return dirz_;
}

bool client::get_crouch() {
	return iscrouching_;
}

bool client::get_jump() {
	return isjumping_;
}

bool client::get_attack() {
	return isattacking_;
}

bool client::get_grab() {
	return isgrabbing_;
}

bool client::get_item() {
	return isusingitem_;
}

bool client::get_drop() {
	return isdropping_;
}

bool client::get_roll() {
	return isrolling_;
}

bool client::get_jumpoffwall() {
	return isjumpingoffwall_;
}

bool client::get_activeblock() {
	return isactiveblocking_;
}

float client::get_blood_damage() {
	return blood_damage_;
}

float client::get_blood_health() {
	return blood_health_;
}

float client::get_block_health() {
	return block_health_;
}

float client::get_temp_health() {
	return temp_health_;
}

float client::get_permanent_health() {
	return permanent_health_;
}

int client::get_knocked_out() {
	return knocked_out_;
}

bool client::get_death_changed() {
	return death_changed_;
}

double client::get_time_of_death() {
	return time_of_death_;
}

float client::get_blood_amount() {
	return blood_amount_;
}

float client::get_recovery_time() {
	return recovery_time_;
}

float client::get_roll_recovery_time() {
	return roll_recovery_time_;
}

int client::get_ragdoll_type() {
	return ragdoll_type_;
}

bool client::get_remove_blood() {
	return remove_blood_;
}

int client::get_blood_delay() {
	return blood_delay_;
}

bool client::get_cut_throat() {
	return cut_throat_;
}

int client::get_state() {
	return state_;
}

bool client::get_signed_on(){
	return has_signed_on_;
}

void client::add_history(vector<int> new_history){
	update_history new_update_history;
	new_update_history = std::make_pair(history_index, new_history);
	client_update_history.insert(client_update_history.begin(), new_update_history);
	history_index++;
	if(history_index > max_history){
		history_index = 0;
	}
	if(client_update_history.size() > max_history){
		client_update_history.pop_back();
	}
	// cout << "client history " << endl;
	// for(int i = 0; i < client_update_history.size(); i++){
	// 	cout << "Index " << i << client_update_history.at(i).first << endl;
	// }
	// cout << "-------------------" << endl;
}

int client::get_last_update_key(){
	if(client_update_history.size() < 1){
		return -1;
	}
	return client_update_history.front().first;
}

int client::get_key(std::string username){
	auto search = keys.find(username);
    if(search != keys.end()) {
		return search->second;
    }
    else {
		return -1;
    }
}

void client::set_key(std::string username, int value){
	keys[username] = value;
}

vector<int> client::get_missing_update_variable_types(int last_update_key){
	vector<int> variable_types;

	for (int i = 0; i < client_update_history.size(); i++){
		//Get the last updated history
		if(client_update_history.at(i).first == last_update_key){
			//Check if it's the newest update, if so return nothing.
			if(client_update_history.at(i) == client_update_history.front()){
				// cout << get_username() << " Already received the latest update" << endl;
				break;
			}
			//Skip the lastest update with -1 because that one is already received.
  			for (int y = (i - 1); y >= 0; y--){
				update_history current_history = client_update_history.at(y);
				if(y != 0){
					cout << "OMG missing message send! at index " << y << endl;
				}
				// cout << get_username()  << " Adding update at index! " << (y) << " update key " << current_history.first << endl;
				//Go over all the variable types in the new histories.
				for(std::vector<int>::iterator it = current_history.second.begin() ; it != current_history.second.end(); ++it){
					//Check if the variable has already been added, if so skip it.
					std::vector<int>::iterator find_itr;
					find_itr = find(variable_types.begin(), variable_types.end(), *it);
					if(find_itr == variable_types.end()){
						// cout << get_username() << " Variable type " << *it << " added " << endl;
						//std::cout << "Element not found in myvector: " << *it << '\n';
						variable_types.push_back(*it);
					}
				}
			}
			break;
		}
	}
	return variable_types;
}

bool client::contains_signon(){
	bool contains = false;
	for(auto &command : commands_){
		if(command["type"] == "SignOn"){
			contains = true;
		}
	}
	return contains;
}

vector <string_map> client::get_signon_commands(){
	vector <string_map> return_commands;
	for (int i = 0; i<commands_.size(); i++){
		if(commands_.at(i)["type"] == "SignOn"){
			return_commands.push_back(commands_.at(i));
			commands_.erase(commands_.begin()+i);
		}
	}
	return return_commands;
}

} // namespace server
} // namespace http
