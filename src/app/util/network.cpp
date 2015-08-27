// network.cpp : 定义控制台应用程序的入口点。
#ifdef NEOLIB_C_PLUS_PLUS
#define extern "C"{
#endif
#include <stdio.h>
#include <tchar.h>
#include "../include/neo_net_wincap.h"
#include "../include/neo_net_index,h.h"
int _tmain(int argc, _TCHAR* argv[])
{
	//slDevInfo sinfo={0};
	lDevInfo *sinfo=NULL;
	//Wincap_GetDrvInfo(&sinfo);
	//for(int i=0;i<info.dev_num;i++)
	//	printf("%s", info.devinfo);
	printf("%d \r\n", sizeof(unsigned long long));
	while(1);
	return 0;
}

#ifdef NEOLIB_C_PLUS_PLUS
}
#endif
