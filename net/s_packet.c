#include <s_list.h>
#include <s_packet.h>
#include <s_mem.h>
#include <s_common.h>

/*
 *	Packet - | size (4 bytes) | ... data ( var size ) ... |
 *
 */

/* -- string encode : len ( 2 bytes ) | ... data (*len* size) ... -- */
#define S_PKT_STRING_MAX_LEN	(1 << 15)

/* -- max packet size : 4M -- */
#define S_PKT_MAX_SZ_BITS	22
#define S_PKT_MAX_SZ		(1 << S_PKT_MAX_SZ_BITS)

struct s_packet {
	int size;
	int pos;	// for read/write
	int ref_count;

	int exp;
	struct s_list list;
};

static struct s_list g_free[32];
static int g_init = 0;

static struct s_packet * iget_free_packet(int size)
{
	if(!g_init) {
		g_init = 1;
		int i;
		for(i = 0; i < 32; ++i) {
			struct s_list * list = &g_free[i];
			s_list_init(list);
		}
	}

	if(size > S_PKT_MAX_SZ) {
		s_log("too big packet size:%d", size);
		return NULL;
	}

	int i;
	i = 4;
	while((1 << i) < size) i++;

	s_log("packet size : %d, %d", size, 1<<i);

	struct s_packet * pkt;

	struct s_list * list = &g_free[i];
	if(!s_list_empty(list)) {
		struct s_list * p = s_list_first(list);
		s_list_del(p);
		pkt = s_list_entry(p, struct s_packet, list);
	} else {
		int real_sz = sizeof(struct s_packet) + 4 + (1 << i);
		pkt = (struct s_packet *)s_malloc(char, real_sz);
	}
	pkt->exp = i;

	return pkt;
}

static void idrop_free_packet(struct s_packet * pkt)
{
	assert(g_init);
	struct s_list * list = &g_free[pkt->exp];
	s_list_insert(list, &pkt->list);
}

struct s_packet * s_packet_create(int size)
{
	struct s_packet * pkt = iget_free_packet(size);
	if(!pkt) {
		return NULL;
	}

	size += 4;

	pkt->size = size;
	pkt->pos = 4;
	pkt->ref_count = 1;

	char * p = (char *)(pkt + 1);
	p[0] = size & 0xFF;
	p[1] = (size >> 8) & 0xFF;
	p[2] = (size >> 16) & 0xFF;
	p[3] = (size >> 24) & 0xFF;

	return pkt;
}

struct s_packet * s_packet_create_from(char * p, int len)
{
	if(len < 4) {
		// too small
		return NULL;
	}
	int sz = (p[0] & 0xFF) | (p[1] << 8 & 0xFF00) | (p[2] << 16 & 0xFF0000) | (p[3] << 24 & 0xFF000000);
	assert(sz > 0);
	if(len < sz) {
		// not complete
		return NULL;
	}

	struct s_packet * pkt = s_packet_create(sz - 4);
	char * data = (char *)(pkt + 1);
	memcpy(data, p, sz);

	pkt->size = sz;
	return pkt;
}

int s_packet_size(struct s_packet * pkt)
{
	return pkt->size;
}

char * s_packet_data_p(struct s_packet * pkt)
{
	return (char*)(pkt + 1);
}

int s_packet_seek(struct s_packet * pkt, int offset)
{
	int sz_can_rw = pkt->size - 4;
	if(offset < 0 || offset >= sz_can_rw) {
		s_log("seek offset:%d of %d not valid!", offset, sz_can_rw);
		return -1;
	}
	pkt->pos = offset + 4;
	return 0;
}

int s_packet_read_char(struct s_packet * pkt, char * c)
{
	if(pkt->pos + 1 > pkt->size) {
		s_log("no data for read_char!");
		return -1;
	}
	char * p = s_packet_data_p(pkt);
	if(c) {
		*c = p[pkt->pos++];
	}
	return 0;
}

int s_packet_read_short(struct s_packet * pkt, short * s)
{
	if(pkt->pos + 2 > pkt->size) {
		s_log("no data for read_short(pos:%d, size:%d!", pkt->pos, pkt->size);
		return -1;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	if(s) {
		*s = (p[0] & 0xFF) | (p[1] << 8 & 0xFF00);
	}
	pkt->pos += 2;
	return 0;
}

int s_packet_read_ushort(struct s_packet * pkt, unsigned short * s)
{
	if(pkt->pos + 2 > pkt->size) {
		s_log("no data for read_ushort(pos:%d, size:%d)!", pkt->pos, pkt->size);
		return -1;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	if(s) {
		*s = (p[0] & 0xFF) | (p[1] << 8 & 0xFF00);
	}
	pkt->pos += 2;
	return 0;
}

int s_packet_read_int(struct s_packet * pkt, int * i)
{
	if(pkt->pos + 4 > pkt->size) {
		s_log("no data for read_int!(pos:%d, size:%d)", pkt->pos, pkt->size);
		return -1;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	if(i) {
		*i = (p[0] & 0xFF) | (p[1] << 8 & 0xFF00) | (p[2] << 16 & 0xFF0000) | (p[3] << 24 & 0xFF000000);
	}
	pkt->pos += 4;
	return 0;
}

int s_packet_read_uint(struct s_packet * pkt, unsigned int * i)
{
	if(pkt->pos + 4 > pkt->size) {
		s_log("no data for read_uint!(pos:%d, size:%d)", pkt->pos, pkt->size);
		return -1;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	if(i) {
		*i = (p[0] & 0xFF) | (p[1] << 8 & 0xFF00) | (p[2] << 16 & 0xFF0000) | (p[3] << 24 & 0xFF000000);
	}
	pkt->pos += 4;
	return 0;
}

struct s_string * s_packet_read_string(struct s_packet * pkt)
{
	short len;
	if(s_packet_read_short(pkt, &len) < 0) {
		s_log("read string : no len!");
		return NULL;
	}
	if(len < 0) {
		s_log("read string : too long(%d)!", len);
		return NULL;
	}
	if(pkt->pos + len > pkt->size) {
		s_log("no data for string:%d", len);
		return NULL;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	struct s_string * str = s_string_create_len(p, len);
	pkt->pos += len;
	return str;
}

int s_packet_eof(struct s_packet * pkt)
{
	if(pkt->pos >= pkt->size) {
		return 1;
	}
	return 0;
}

int s_packet_write_char(struct s_packet * pkt, char c)
{
	if(pkt->pos + 1 > pkt->size) {
		s_log("no room for write char(%c)!", c);
		return -1;
	}
	char * p = s_packet_data_p(pkt);
	p[pkt->pos++] = c;
	return 0;
}

int s_packet_write_short(struct s_packet * pkt, short s)
{
	if(pkt->pos + 2 > pkt->size) {
		s_log("no room for write short(%d)!", s);
		return -1;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	p[0] = s & 0xFF;
	p[1] = (s >> 8) & 0xFF;

	pkt->pos += 2;
	return 0;
}

int s_packet_write_ushort(struct s_packet * pkt, unsigned short s)
{
	if(pkt->pos + 2 > pkt->size) {
		s_log("no room for write short(%d)!", s);
		return -1;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	p[0] = s & 0xFF;
	p[1] = (s >> 8) & 0xFF;

	pkt->pos += 2;
	return 0;
}

int s_packet_write_int(struct s_packet * pkt, int i)
{
	if(pkt->pos + 4 > pkt->size) {
		s_log("no room for write int(%d)!", i);
		return -1;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	p[0] = i & 0xFF;
	p[1] = (i >> 8) & 0xFF;
	p[2] = (i >> 16) & 0xFF;
	p[3] = (i >> 24) & 0xFF;

	pkt->pos += 4;
	return 0;
}

int s_packet_write_uint(struct s_packet * pkt, unsigned int i)
{
	if(pkt->pos + 4 > pkt->size) {
		s_log("no room for write int(%d)!", i);
		return -1;
	}
	char * p = s_packet_data_p(pkt) + pkt->pos;
	p[0] = i & 0xFF;
	p[1] = (i >> 8) & 0xFF;
	p[2] = (i >> 16) & 0xFF;
	p[3] = (i >> 24) & 0xFF;

	pkt->pos += 4;
	return 0;
}

int s_packet_write_string(struct s_packet * pkt, struct s_string * s)
{
	int len = s_string_len(s);
	const char * p = s_string_data_p(s);

	return s_packet_write_s(pkt, p, len);
}

int s_packet_write_s(struct s_packet * pkt, const char * pstr, int len)
{
	if(len >= S_PKT_STRING_MAX_LEN) {
		s_log("string is too long(%d) for write in packet!", len);
		return -1;
	}
	if(pkt->pos + 2 + len > pkt->size) {
		s_log("no room for write string:%s", pstr);
		return -1;
	}
	if(s_packet_write_short(pkt, len) < 0) {
		s_log("write len error?");
		return -1;
	}
	char * p = (char*)s_packet_data_p(pkt) + pkt->pos;
	memcpy(p, pstr, len);
	pkt->pos += len;

	return 0;
}

void s_packet_grab(struct s_packet * pkt)
{
	pkt->ref_count++;
}

void s_packet_drop(struct s_packet * pkt)
{
	pkt->ref_count--;
	if(pkt->ref_count == 0) {
		idrop_free_packet(pkt);
	}

	if(pkt->ref_count < 0) {
		s_log("double-free packet!");
	}
}

