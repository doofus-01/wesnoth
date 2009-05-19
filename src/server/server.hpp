#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#include "../network.hpp"
#include "ban.hpp"
#include "player.hpp"
#include "room.hpp"
#include "simple_wml.hpp"
#include "user_handler.hpp"
#include <boost/scoped_ptr.hpp>

class server
{
public:
	server(int port, const std::string& config_file, size_t min_threads,size_t max_threads);
	void run();
private:
	void send_error(network::connection sock, const char* msg, const char* error_code ="") const;
	void send_error_dup(network::connection sock, const std::string& msg) const;

	// The same as send_error(), we just add an extra child to the response
	// telling the client the chosen username requires a password.
	void send_password_request(network::connection sock, const char* msg,
			const std::string& user, const char* error_code ="",
			bool force_confirmation =false);

	const network::manager net_manager_;
	network::server_manager server_;
	wesnothd::ban_manager ban_manager_;

	std::deque<std::pair<std::string, std::string> > ip_log_;

	boost::scoped_ptr<user_handler> user_handler_;
	std::map<network::connection,std::string> seeds_;

	/** std::map<network::connection,player>. */
	wesnothd::player_map players_;
	std::set<network::connection> ghost_players_;

	std::vector<wesnothd::game*> games_;
	wesnothd::game not_logged_in_;

	/** The lobby is implemented as a room. */
	wesnothd::room lobby_;

	/** server socket/fifo. */
	boost::scoped_ptr<input_stream> input_;

	const std::string config_file_;
	config cfg_;

	/** Read the server config from file 'config_file_'. */
	config read_config() const;

	// settings from the server config
	std::vector<std::string> accepted_versions_;
	std::map<std::string,config> redirected_versions_;
	std::map<std::string,config> proxy_versions_;
	std::vector<std::string> disallowed_names_;
	std::string admin_passwd_;
	std::set<network::connection> admins_;
	std::string motd_;
	size_t default_max_messages_;
	size_t default_time_period_;
	size_t concurrent_connections_;
	bool graceful_restart;
	time_t lan_server_;
	time_t last_user_seen_time_;
	std::string restart_command;
	size_t max_ip_log_size_;
	std::string uh_name_;
	bool deny_unregistered_login_;
	bool save_replays_;
	std::string replay_save_path_;

	/** Parse the server config into local variables. */
	void load_config();

	bool ip_exceeds_connection_limit(const std::string& ip) const;
	std::string is_ip_banned(const std::string& ip) const;

	simple_wml::document version_query_response_;
	simple_wml::document login_response_;
	simple_wml::document join_lobby_response_;
	simple_wml::document games_and_users_list_;

	metrics metrics_;

	time_t last_ping_;
	time_t last_stats_;
	void dump_stats(const time_t& now);

	time_t last_uh_clean_;
	void clean_user_handler(const time_t& now);

	void process_data(const network::connection sock,
	                  simple_wml::document& data);
	void process_login(const network::connection sock,
	                   simple_wml::document& data);

	/** Handle queries from clients. */
	void process_query(const network::connection sock,
	                   simple_wml::node& query);

	/** Process commands from admins and users. */
	std::string process_command(const std::string& cmd, const std::string& issuer_name);

	/** Handle private messages between players. */
	void process_whisper(const network::connection sock,
	                     simple_wml::node& whisper) const;

	/** Handle nickname registration related requests from clients. */
	void process_nickserv(const network::connection sock, simple_wml::node& data);
	void process_data_lobby(const network::connection sock,
	                        simple_wml::document& data);
	void process_data_game(const network::connection sock,
	                       simple_wml::document& data);
	void delete_game(std::vector<wesnothd::game*>::iterator game_it);

	void update_game_in_lobby(const wesnothd::game* g, network::connection exclude=0);

	void start_new_server();
};



#endif
