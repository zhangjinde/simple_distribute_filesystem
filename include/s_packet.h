#ifndef s_packet_h_
#define s_packet_h_

#include <s_string.h>

struct s_packet;

struct s_packet *
s_packet_create(int size);

struct s_packet *
s_packet_create_from(char * p, int len);

#define s_packet_easy(fun, sz)	({	\
		struct s_packet * __pkt = s_packet_create(sz);	\
		s_packet_set_fun(__pkt, fun);	\
		__pkt; })


int
s_packet_size(struct s_packet * pkt);

char *
s_packet_data_p(struct s_packet * pkt);

int
s_packet_seek(struct s_packet * pkt, int offset);

int
s_packet_tell(struct s_packet * pkt);


/*
 *	fun & req
 *
 */
unsigned int
s_packet_get_fun(struct s_packet * pkt);

int
s_packet_set_fun(struct s_packet * pkt, unsigned int fun);

unsigned int
s_packet_get_req(struct s_packet * pkt);

int
s_packet_set_req(struct s_packet * pkt, unsigned int req);


/*
 *	read & write
 *
 */
int
s_packet_read_char(struct s_packet * pkt, char * c);

int
s_packet_read_short(struct s_packet * pkt, short * s);

int
s_packet_read_ushort(struct s_packet * pkt, unsigned short * us);

int
s_packet_read_int(struct s_packet * pkt, int * i);

int
s_packet_read_uint(struct s_packet * pkt, unsigned int * u);

int
s_packet_read_string(struct s_packet * pkt, struct s_string ** pp);

int
s_packet_read_to_end(struct s_packet * pkt, char * buf, int * size);

int
s_packet_eof(struct s_packet * pkt);

int
s_packet_write_char(struct s_packet * pkt, char c);

int
s_packet_write_short(struct s_packet * pkt, short s);

int
s_packet_write_ushort(struct s_packet * pkt, unsigned short s);

int
s_packet_write_int(struct s_packet * pkt, int i);

int
s_packet_write_uint(struct s_packet * pkt, unsigned int u);

int
s_packet_write_s(struct s_packet * pkt, const char * p, int count);

int
s_packet_write_string(struct s_packet * pkt, struct s_string * s);

#define s_packet_read(pkt, out, type, error)	\
	if(s_packet_read_##type(pkt, out) < 0) {	\
		s_log("s_packet_read_" #type "error!");	\
		goto error;\
	}

#define s_packet_write(pkt, in, type, error)	\
	if(s_packet_write_##type(pkt, in) < 0) {	\
		s_log("s_packet_write_" #type "error!");	\
		goto error;\
	}

#define s_packet_size_for_string(s)	(2 + s_string_len(s))
#define s_packet_size_for_int(i)	4
#define s_packet_size_for_uint(u)	4
#define s_packet_size_for_short(u)	2
#define s_packet_size_for_ushort(u)	2
#define s_packet_size_for_id(i)		8

#define s_packet_write_id(pkt, id)	{	\
	s_packet_write_int(pkt, (id)->x);	\
	s_packet_write_int(pkt, (id)->y);	\
}

#define s_packet_start_write(pkt_out, __fun)	\
	do {	\
		struct s_packet * __pkt = NULL;	\
		int __i;	\
		int __sz = 0;	\
		for(__i = 0; __i < 2; ++__i) {	\
			if(__i == 1) {	\
				__pkt = s_packet_easy(__fun, __sz);	\
				pkt_out = __pkt;	\
			}

#define s_packet_wint(__i)	\
	if(__pkt) {	\
		s_packet_write_int(__pkt, __i);	\
	} else 	{	\
		__sz += s_packet_size_for_int(__i);	\
	}

#define s_packet_wuint(__i)	\
	if(__pkt) {	\
		s_packet_write_uint(__pkt, __i);	\
	} else 	{	\
		__sz += s_packet_size_for_uint(__i);	\
	}

#define s_packet_wstring(__s)	\
	if(__pkt) {	\
		s_packet_write_string(__pkt, __s);	\
	} else 	{	\
		__sz += s_packet_size_for_string(__s);	\
	}

#define s_packet_end_write()	\
		}	\
	} while(0)


/*
 *	Increase Reference Count
 *
 */
void
s_packet_grab(struct s_packet * pkt);


/*
 *	Decrease Reference Count
 *
 */
void
s_packet_drop(struct s_packet * pkt);

void
s_packet_dump(struct s_packet * pkt);

#endif
