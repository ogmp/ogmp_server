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
remove_blood_(false), blood_delay_(0), cut_throat_(false), cut_torso_(false),
in_animation_(false), combat_stance_time_(-10.0f), backslash_(false), last_knife_time_(0.0f),
plant_rustle_delay_(0.0f), in_plant_(0.0f), state_(0), active_dodging_(false),
active_blocking_(false), startled_(false), on_ground_(false), frozen_(false),
no_freeze_(false), active_block_duration_(0.0f), active_block_recharge_(0.0f),
active_dodge_duration_(0.0f), active_dodge_recharge_(0.0f), ragdoll_time_(0.0f) {
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

void client::set_cut_torso(bool cut_torso) {
	cut_torso_ = cut_torso;
}

void client::set_in_animation(bool in_animation) {
	in_animation_ = in_animation;
}

void client::set_combat_stance_time(float combat_stance_time) {
	combat_stance_time_ = combat_stance_time;
}

void client::set_backslash(bool backslash) {
	backslash_ = backslash;
}

void client::set_last_knife_time(float last_knife_time) {
	last_knife_time_ = last_knife_time;
}

void client::set_plant_rustle_delay(float plant_rustle_delay) {
	plant_rustle_delay_ = plant_rustle_delay;
}

void client::set_in_plant(float in_plant) {
	in_plant_ = in_plant;
}

void client::set_state(int state) {
	state_ = state;
}

void client::set_active_dodging(bool active_dodging) {
	active_dodging_ = active_dodging;
}

void client::set_active_blocking(bool active_blocking) {
	active_blocking_ = active_blocking;
}

void client::set_startled(bool startled) {
	startled_ = startled;
}

void client::set_on_ground(bool on_ground) {
	on_ground_ = on_ground;
}

void client::set_frozen(bool frozen) {
	frozen_ = frozen;
}

void client::set_no_freeze(bool no_freeze) {
	no_freeze_ = no_freeze;
}

void client::set_active_block_duration(float active_block_duration) {
	active_block_duration_ = active_block_duration;
}

void client::set_active_block_recharge(float active_block_recharge) {
	active_block_recharge_ = active_block_recharge;
}

void client::set_active_dodge_duration(float active_dodge_duration) {
	active_dodge_duration_ = active_dodge_duration;
}

void client::set_active_dodge_recharge(float active_dodge_recharge) {
	active_dodge_recharge_ = active_dodge_recharge;
}

void client::set_ragdoll_time(float ragdoll_time) {
	ragdoll_time_ = ragdoll_time;
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

bool client::get_cut_torso() {
	return cut_torso_;
}

bool client::get_in_animation() {
	return in_animation_;
}

float client::get_combat_stance_time() {
	return combat_stance_time_;
}

bool client::get_backslash() {
	return backslash_;
}

float client::get_last_knife_time() {
	return last_knife_time_;
}

float client::get_plant_rustle_delay() {
	return plant_rustle_delay_;
}

float client::get_in_plant() {
	return in_plant_;
}

int client::get_state() {
	return state_;
}

bool client::get_active_dodging() {
	return active_dodging_;
}

bool client::get_active_blocking() {
	return active_blocking_;
}

bool client::get_startled() {
	return startled_;
}

bool client::get_on_ground() {
	return on_ground_;
}

bool client::get_frozen() {
	return frozen_;
}

bool client::get_no_freeze() {
	return no_freeze_;
}

float client::get_active_block_duration() {
	return active_block_duration_;
}

float client::get_active_block_recharge() {
	return active_block_recharge_;
}

float client::get_active_dodge_duration() {
	return active_dodge_duration_;
}

float client::get_active_dodge_recharge() {
	return active_dodge_recharge_;
}

float client::get_ragdoll_time() {
	return ragdoll_time_;
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
