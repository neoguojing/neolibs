
#ifdef NEOLIB_C_PLUS_PLUS
#define extern "C"{
#endif

#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"ws2_32.lib")

#include <pcap.h>
#include <tchar.h>
#include <WinSock2.h>

#include "../include/neo_net_wincap.h"
#include "../../include/neo_net_common_head.h"

#define DATA_READ_TIME_OUT                           300
#define ADAPTER_MIX_MODE                             1

//static lDevInfo *g_stDevInfo = NULL;
static pcap_t *g_p_hAdapterHandle;//������
static pcap_if_t *g_pAdapter;
/*����ʵ��������Ϣ�Ļ�ȡ  δʵ��*/
int Wincap_GetDrvInfo(IN OUT DevInfo *stDevInfo)
{
	pcap_if_t *pAdaptersList = NULL;
	TCHAR errbuf[PCAP_ERRBUF_SIZE];

	pcap_findalldevs(&pAdaptersList, errbuf);
	INT iAdapterCount = 0;
	for(pcap_if_t *pAdapter = pAdaptersList; pAdapter != NULL; pAdapter = pAdapter->next)
	{
		if(pAdapter->description)
			memcpy(*(stDevInfo->devinfo+iAdapterCount), pAdapter->description,  WINCAP_DEVINFO_LENGTH-1);
		    
		else
			memcpy(*(stDevInfo->devinfo+iAdapterCount), pAdapter->name, WINCAP_DEVINFO_LENGTH-1);
		iAdapterCount++;
	}
    stDevInfo->dev_num = iAdapterCount;
	return iAdapterCount;
}

/* ������Ϊ�������շ��ʵ���ָ�룬��Ҫ���ָ���ֵ��
����ͨ������ָ��ĵ�ַ����ã������޷����ָ��ı��Ժ��ֵ*/
int Wincap_GetDrvInfo(lDevInfo **p_stDevInfo)
{
	pcap_if_t *pAdaptersList = NULL;
	TCHAR errbuf[PCAP_ERRBUF_SIZE];

	pcap_findalldevs(&pAdaptersList, errbuf);

	INT iAdapterCount = 0;
	lDevInfo pHead    = {0};
	lDevInfo *pNext   = &pHead;
	lDevInfo *pCurrent= &pHead;
	
	for(pcap_if_t *pAdapter = pAdaptersList; pAdapter != NULL; pAdapter = pAdapter->next)
	{
		lDevInfo *temp =(lDevInfo *) malloc(sizeof(lDevInfo));
		if(pAdapter->description)
			memcpy(temp->devinfo,pAdapter->description,WINCAP_DEVINFO_LENGTH-1);    
		else
			memcpy(temp->devinfo,pAdapter->name,WINCAP_DEVINFO_LENGTH-1);

		temp->dev_seq = iAdapterCount;
		temp->next=NULL;

		pNext          = temp;
		pCurrent->next = pNext;
		pCurrent       = pNext;
	    pNext          = pNext->next;
		iAdapterCount++;
	}
    
	*p_stDevInfo = pHead.next;
	return iAdapterCount;
}

int Wincap_OpenDrvBySeq(int iDev_seq)
{ 
	pcap_if_t *pAdaptersList = NULL;
	TCHAR errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t *pAdapter      = NULL;

	pcap_findalldevs(&pAdaptersList, errbuf);
	if(pAdaptersList == NULL)
	{
	    return NEO_NET_WINCAP_GETDEVLIST_FAIL;
	}
	pAdapter = pAdaptersList;
	for(int i = 0; i<= iDev_seq; i++ )
	{
	    pAdapter = pAdapter->next;
	}

	g_pAdapter = pAdapter;
	/*����ģʽ������*/
	pcap_t *p_hAdapterHandle = NULL;
	p_hAdapterHandle = pcap_open_live(
		pAdapter->name,
		65535,
		ADAPTER_MIX_MODE,
		DATA_READ_TIME_OUT,
		errbuf);
	if(p_hAdapterHandle == NULL)
	{
	    return NEO_NET_FAIL;
	}

	g_p_hAdapterHandle = p_hAdapterHandle;
	return NEO_NET_SUCCESS;
}

int Wincap_SetFilter(TCHAR *sFilterCode)
{
	u_int uNetMask;
	if( g_pAdapter->addresses)
		uNetMask = ((sockaddr_in *)( g_pAdapter->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		uNetMask = 0xffffffff;

	bpf_program fpCode;
	if(pcap_compile(g_p_hAdapterHandle, &fpCode, sFilterCode, TRUE, uNetMask) < 0) 
		return NEO_NET_WINCAP_COMPILEFILTER_FAIL;

	if(pcap_setfilter(g_p_hAdapterHandle, &fpCode) < 0)
		return NEO_NET_WINCAP_COMPILEFILTER_FAIL;
	return NEO_NET_SUCCESS;
}
/*����0 ��ʾ��ʱ
      1 ��ʾ����
	  С��0 ��ʾʧ��*/
int Wincap_CapturePacket(const u_char *pkt_data)
{
	struct pcap_pkthdr *header;
    int iResult = pcap_next_ex(g_p_hAdapterHandle, &header, &pkt_data);

	return iResult;
}
 void Wincap_Free_DevInfo_S(OUT lDevInfo *p_stDevInfo)
 {
	 lDevInfo *pTemp = NULL;
	 while( p_stDevInfo )
	 {
		 pTemp       = p_stDevInfo;
		 p_stDevInfo = p_stDevInfo->next;
		
         free(pTemp);	 
	 }
 }
#ifdef NEOLIB_C_PLUS_PLUS
}
#endif
