
#ifndef NEO_NET_MSG_TAKER

#ifdef NEOLIB_C_PLUS_PLUS
#define extern "C"{
#endif

//以太网数据帧头
typedef struct
{
	unsigned short hardware_type;
	unsigned short proto_tyoe;
	unsigned char  hardware_addr_len;
	unsigned char  proto_addr_len;
	unsigned short operation_code;
	unsigned char  src_mac_addr[6];
    unsigned char  src_ip_addr[4];
	unsigned char  dest_mac_addr[6];
	unsigned char  dest_ip_addr[4];

}ETH_MAC_HEADER;
const unsigned long ETH_MAC_HEADER_LEN = sizeof(ETH_MAC_HEADER);  

//ARP标记
const unsigned short ARP_OP_REQUEST    = 1;
const unsigned short ARP_OP_REPLY      = 2;


//ipv4报文头 
typedef struct
{
	unsigned char head_len: 4;
	unsigned char version : 4;
	unsigned char service_type;
	unsigned short total_len;
	unsigned short identifier;
	unsigned short frag_and_flags;
	unsigned char ttl;
	unsigned char protocol;
	unsigned short checksum;
	unsigned long source_ip;
	unsigned long dest_ip;
}IPv4_HEADER;
const unsigned long IPv4_HEADER_LEN = sizeof(IPv4_HEADER);  

#pragma pack(push, 1)
typedef struct
{
	unsigned long      ver_cls_lab;
	unsigned short     payload_len;
	unsigned char      next_header;
	unsigned char      hop_limit;

	struct
	{
		unsigned long long prefix_subnetid;
	    unsigned char      interface_id[8];
	}src_ip;

	struct
	{
		unsigned long long prefix_subnetid;
	    unsigned char      interface_id[8];
	}dst_ip;

}IPv6_HEADER;
#pragma pack(pop)
const unsigned long IPv6_HEADER_LEN = sizeof(IPv6_HEADER); 

//TCP头部
typedef struct 
{
	unsigned short  source_port;
	unsigned short  dest_port;
	unsigned long   seq;
	unsigned long   ack;
	unsigned char   head_len;
	unsigned char   flag;
	unsigned short  window;
	unsigned short  check_sum;
	unsigned short  urgent;
	unsigned int    mss_opt;
	unsigned short  nop_opt;
    unsigned short  sack_opt;
}TCP_HEADER;
const unsigned long TCP_HEADER_LEN = sizeof(TCP_HEADER); 
//udp报头
typedef struct
{
	unsigned short  source_port;
	unsigned short  dest_port;
	unsigned short  length;
	unsigned short  check_sum;
}UDP_HEADER;
const unsigned long UDP_HEADER_LEN = sizeof(UDP_HEADER); 

//ICMP
//基本头部
typedef struct 
{
	unsigned char   tyoe;
	unsigned char   code;
	unsigned short  cksum;
}ICMP_BASE_HEADER;
const unsigned long ICMP_BASE_HEADER_LEN = sizeof(ICMP_BASE_HEADER);  

//ICMP回显报文头
typedef struct
{
	ICMP_BASE_HEADER base_hdr;
	unsigned short   id;
	unsigned short   seq;
} ICMP_ECHO_HEADER;
const unsigned long ICMP_ECHO_HEADER_LEN = sizeof(ICMP_ECHO_HEADER);  

//ICMP差错报文头
typedef struct
{
	ICMP_BASE_HEADER base_hdr;
	unsigned long    unused;
} ICMP_ERROR_HEADER;
const unsigned long ICMP_ERROR_HEADER_LEN = sizeof(ICMP_ERROR_HEADER);  


#ifdef NEOLIB_C_PLUS_PLUS
}
#endif

#endif
