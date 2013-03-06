#include "s_do_net_event.h"

void s_do_net_event_m(struct s_server * serv, struct s_packet * pkt, void * ud)
{
	struct s_mserver * mserv = (struct s_mserver *)ud;
	s_used(mserv);

	int fun;
	if(s_packet_read_int(pkt, &fun) < 0) {
		s_log("read type error!");
		return;
	}

	switch(fun) {

		default:
			s_log("[Error M] un-expected packet, fun:%d", fun);
			break;
	}
}
