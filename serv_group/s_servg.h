#ifndef s_servg_h_
#define s_servg_h_

#include <s_net.h>
#include <s_packet.h>
#include <s_list.h>
#include <s_array.h>
#include <s_string.h>
#include <s_config.h>
#include <s_common.h>
#include <s_thread.h>

#include "s_server_group.h"
#include "s_servg_pkt.h"

/* ------ who connect who ----- */
#define s_do_i_conn_him(my_type, my_id, his_type, his_id)	\
	(((my_type) > (his_type)) || (((my_type) == (his_type)) && ((my_id) > (his_id))))

/* ------ Serv Flags ------------ */
#define S_SERV_FLAG_ESTABLISHED (1 << 0)        // server is connected and established
#define S_SERV_FLAG_IN_CONFIG   (1 << 1)        // server is in config.conf

// for rpc call
struct ipacket {
	unsigned int req_id;
	struct s_packet * pkt;
};

struct s_servg_rpc_param {
	unsigned int req_id;
	void * ud;
	S_SERVG_CALLBACK callback;

	int msec_timeout;

	struct timeval tv_timeout;

	struct s_server * serv;
	struct s_list timeout_list;
};

struct s_server {
	struct s_server_group * servg;

	struct s_conn * conn;

	// id and type
	int id; 
	int type;

	// flags
	unsigned int flags;

	// infomation
	struct s_string * ip; 
	int port;

	unsigned int mem;

	// time
	struct timeval tv_connect;
	struct timeval tv_send_identify;
	struct timeval tv_established;
	struct timeval tv_send_ping;
	struct timeval tv_receive_pong;

	struct timeval tv_delay;

	// server can be : wait_for_conn, wait_for_identify, wait_for_ping, wait_for_pong
	struct s_list list;

	// rpc
	struct s_hash * rpc_hash;
	unsigned int req_id;

	// user data
	void * udata;
};

struct s_server_group {
	// this server's id and type
	int id;
	int type;

	// config -- {
		struct s_config * config;

		// password between ipc
		unsigned int ipc_pwd;

		// sec for timeover : default 5
		int sec_for_timeover;
		int sec_for_pingpong;
		int sec_for_reconnect;

	// -- }

	// myself
	struct s_server * myself;
	struct s_queue * myself_q;

	// net
	struct s_net * net;

	// mservs and dservs and clients
	struct s_array * servs[S_SERV_TYPE_MAX];

	struct s_server * min_delay_serv[S_SERV_TYPE_MAX];

	// list for ping/pong
	struct s_list list_wait_for_ping;
	struct s_list list_wait_for_pong;

	// list for connect
	struct s_list list_wait_for_conn;
	struct s_list list_wait_for_identify;

	// callback
	struct s_servg_callback callback;

	// rpc
	struct s_list rpc_timeout_list;
};

struct s_server *
s_servg_get_serv_in_config(struct s_server_group * servg, int type, int id);

struct s_server *
s_servg_create_serv(struct s_server_group * servg, int type, int id);

void
s_servg_reset_serv(struct s_server_group * servg, struct s_server * serv);

int
s_servg_check_list(struct s_server_group * servg);

void
s_servg_do_event(struct s_conn * conn, struct s_packet * pkt, void * ud);

#endif

