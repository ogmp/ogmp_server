#include "client.hpp"

namespace http {
namespace server {

using namespace std;

client::client() : uid_(""), username_(""), team_(""), character_(""), level_(""),
posx_(0.0f), posy_(0.0f), posz_(0.0f), dirx_(0.0f), dirz_(0.0f), saved_posx_(0.0f),
saved_posy_(0.0f), saved_posz_(0.0f), iscrouching_(false), isjumping_(false),
isattacking_(false), isgrabbing_(false), isusingitem_(false), isdropping_(false),
isrolling_(false), isjumpingoffwall_(false), isactiveblocking_(false),
blood_damage_(0.0f), blood_health_(1.0f), block_health_(1.0f), temp_health_(1.0f),
permanent_health_(1.0f), knocked_out_(_awake), lives_(1), blood_amount_(10.0f),
recovery_time_(0.0f), roll_recovery_time_(0.0f), ragdoll_type_(0), time_of_death_(0),
remove_blood_(false), blood_delay_(0), cut_throat_(false), state_(0), has_signed_on_(false) {
}

void client::set_uid(string uid) {
	uid_ = uid;
}

void client::set_level(string level) {
	level_ = level;
}

void client::set_username(string username) {
	username_ = username;
}

void client::set_team(string team) {
	team_ = team;
}

void client::add_command(string_map command) {
	commands_.push_back(command);
}

void client::set_last_updated(double current_seconds) {
	last_updated_ = current_seconds;
}

void client::set_character(string character) {
	character_ = character;
}

void client::set_posx(float posx) {
	posx_ = posx;
}

void client::set_posy(float posy) {
	posy_ = posy;
}

void client::set_posz(float posz) {
	posz_ = posz;
}

void client::set_saved_posx(float posx) {
	saved_posx_ = posx;
}

void client::set_saved_posy(float posy) {
	saved_posy_ = posy;
}

void client::set_saved_posz(float posz) {
	saved_posz_ = posz;
}

void client::set_crouch(bool crouch) {
	iscrouching_ = crouch;
}

void client::set_jump(bool jump) {
	isjumping_ = jump;
}

void client::set_attack(bool attack) {
	isattacking_ = attack;
}

void client::set_grab(bool grab) {
	isgrabbing_ = grab;
}

void client::set_item(bool item) {
	isusingitem_ = item;
}

void client::set_drop(bool drop) {
	isdropping_ = drop;
}

void client::set_roll(bool roll) {
	isrolling_ = roll;
}

void client::set_jumpoffwall(bool offwall) {
	isjumpingoffwall_ = offwall;
}

void client::set_activeblock(bool activeblock) {
	isactiveblocking_ = activeblock;
}

void client::set_blood_damage(float blood_damage) {
	blood_damage_ = blood_damage;
}

void client::set_blood_health(float blood_health) {
	blood_health_ = blood_health;
}

void client::set_block_health(float block_health) {
	block_health_ = block_health;
}

void client::set_temp_health(float temp_health) {
	temp_health_ = temp_health;
}

void client::set_permanent_health(float permanent_health) {
	permanent_health_ = permanent_health;
}

void client::set_knocked_out(int knocked_out) {
	knocked_out_ = knocked_out;
}

void client::set_death_changed(bool death_changed) {
	death_changed_ = death_changed;
}

void client::set_signed_on(bool signed_on){
	has_signed_on_ = signed_on;
}

void client::set_time_of_death(double current_seconds) {
	time_of_death_ = current_seconds;
}

void client::set_lives(int lives) {
	lives_ = lives;
}

void client::set_blood_amount(float blood_amount) {
	blood_amount_ = blood_amount;
}

void client::set_recovery_time(float recovery_time) {
	recovery_time_ = recovery_time;
}

void client::set_roll_recovery_time(float roll_recovery_time) {
	roll_recovery_time_ = roll_recovery_time;
}

void client::set_ragdoll_type(int ragdoll_type) {
	ragdoll_type_ = ragdoll_type;
}

void client::set_remove_blood(bool remove_blood) {
	remove_blood_ = remove_blood;
}

void client::set_blood_delay(int blood_delay) {
	blood_delay_ = blood_delay;
}

void client::set_cut_throat(bool cut_throat) {
	cut_throat_ = cut_throat;
}

void client::set_state(int state) {
	state_ = state;
}

void client::set_dirx(float dirx) {
	dirx_ = dirx;
}

void client::set_dirz(float dirz) {
	dirz_ = dirz;
}

string client::get_uid() {
	return uid_;
}

string client::get_level() {
	return level_;
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

string client::get_character() {
	return character_;
}

double client::get_last_updated() {
	return last_updated_;
}

int client::get_number_of_commands() {
	return commands_.size();
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

int client::get_lives() {
	return lives_;
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
