/***************************************************
wincap 相关功能实现
***************************************************/
#ifndef _WINCAP_H_
#define _WINCAP_H_

#ifdef NEOLIB_C_PLUS_PLUS
#define extern "C"{
#endif

#define TCHAR                                   char
#define WINCAP_DEVINFO_LENGTH                   50

#define WINCAP_SRC                              "src"
#define WINCAP_DST                              "dst"
#define WINCAP_HOST                             "host"
#define WINCAP_ARP                              "arp"
#define WINCAP_RARP                             "rarp"
#define WINCAP_IP                               "ip"
#define WINCAP_TCP                              "tcp"
#define WINCAP_UDP                              "udp"
#define WINCAP_ETHER                            "ether"
#define WINCAP_PORT                             "port"

typedef struct tag_WINCAP_DEVS
{
	int     dev_num;
	TCHAR   devinfo[][WINCAP_DEVINFO_LENGTH];
}DevInfo;

typedef struct tag_WINCAP_DEVLINK
{
	int     dev_seq;
	TCHAR   devinfo[WINCAP_DEVINFO_LENGTH];
	struct  tag_WINCAP_DEVLINK *next;
}lDevInfo;


int Wincap_GetDrvInfo(DevInfo *stDevInfo);
int Wincap_GetDrvInfo(lDevInfo **p_stDevInfo);

/*释放获取的设备信息列表*/
void Wincap_Free_DevInfo_S(lDevInfo *p_stDevInfo);
int Wincap_OpenDrvBySeq(int iDev_seq);
int Wincap_SetFilter(TCHAR *sFilterCode);
int Wincap_CapturePacket();


#ifdef NEOLIB_C_PLUS_PLUS
}
#endif
#endif