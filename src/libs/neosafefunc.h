#ifndef NEOSAFEFUNC

#define NEOSAFEFUNC

///////////////////////////////////////////////export func
 void neosafestrncpy(char *pD,char *pS,int nDestSize);
 int neosafesprintf(char *szBuf,int nDestSize,char *szFormat,...);
 void SafeStrcpy(char *pDest,char *pSource,int nCount);

#endif