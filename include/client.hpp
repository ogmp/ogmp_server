#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include "shared.hpp"
//#include "reply.hpp"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace http {
namespace server {

using namespace std;

struct reply;

class client {
	public:
		client();

		void set_uid(string uid);
		void set_level_name(string level);
		void set_level_path(string level);
		void set_username(string username);
		void set_team(string team);
		void add_command(string_map command);
		void add_to_inbox(reply& command);
		void set_last_updated(double current_seconds);
		void set_character(string character);

		void set_posx(float posx);
		void set_posy(float posy);
		void set_posz(float posz);
		void set_saved_posx(float posx);
		void set_saved_posy(float posy);
		void set_saved_posz(float posz);
		void set_dirx(float dirx);
		void set_dirz(float dirz);

		void set_crouch(bool crouch);
		void set_jump(bool jump);
		void set_attack(bool attack);
		void set_grab(bool grab);
		void set_item(bool item);
		void set_drop(bool drop);
		void set_roll(bool roll);
		void set_jumpoffwall(bool offwall);
		void set_activeblock(bool offwall);
		void set_signed_on(bool signedon);

		void set_blood_damage(float blood_damage);
		void set_blood_health(float blood_health);
		void set_block_health(float block_health);
		void set_temp_health(float temp_health);
		void set_permanent_health(float permanent_health);
		void set_knocked_out(int knocked_out);
		void set_death_changed(bool death_changed);
		void set_time_of_death(double current_seconds);
		void set_blood_amount(float blood_amount);
		void set_recovery_time(float recovery_time);
		void set_roll_recovery_time(float roll_recovery_time);
		void set_ragdoll_type(int ragdoll_type);
		void set_remove_blood(bool remove_blood);
		void set_blood_delay(int blood_delay);
		void set_cut_throat(bool cut_throat);
		void set_state(int state);

		string get_uid();
		string get_level_name();
		string get_level_path();
		string get_username();
		string get_team();
		string_map get_command();
		reply get_inbox_message();
		int get_number_of_commands();
		int get_number_of_inbox_messages();
		double get_last_updated();
		string get_character();

		float get_posx();
		float get_posy();
		float get_posz();
		float get_saved_posx();
		float get_saved_posy();
		float get_saved_posz();
		float get_dirx();
		float get_dirz();

		bool get_crouch();
		bool get_jump();
		bool get_attack();
		bool get_grab();
		bool get_item();
		bool get_drop();
		bool get_roll();
		bool get_jumpoffwall();
		bool get_activeblock();

		float get_blood_damage();
		float get_blood_health();
		float get_block_health();
		float get_temp_health();
		float get_permanent_health();
		int get_knocked_out();
		bool get_death_changed();
		double get_time_of_death();
		float get_blood_amount();
		float get_recovery_time();
		float get_roll_recovery_time();
		int get_ragdoll_type();
		bool get_remove_blood();
		int get_blood_delay();
		bool get_cut_throat();
		int get_state();
		bool get_signed_on();

		bool contains_signon();
		vector <string_map> get_signon_commands();

	private:
		string uid_, level_path_, level_name_, username_, team_, character_;
		float posx_, posy_, posz_, saved_posx_, saved_posy_, saved_posz_, dirx_, dirz_, blood_damage_, blood_health_, block_health_, temp_health_, permanent_health_, blood_amount_, recovery_time_, roll_recovery_time_;
		bool iscrouching_, isjumping_, isattacking_, isgrabbing_, isusingitem_, isdropping_, isrolling_, isjumpingoffwall_, isactiveblocking_, death_changed_, remove_blood_, cut_throat_, has_signed_on_;
		double last_updated_, time_of_death_;
		int knocked_out_, ragdoll_type_, blood_delay_, state_;
		string_map_vector commands_;
		vector<reply> inbox_;
};

typedef boost::shared_ptr<client> client_ptr;

} // namespace server
} // namespace http

#endif
