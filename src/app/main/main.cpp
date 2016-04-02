#include <iostream>
/*
#include "../../libs/neodebug.h"
#include "../../libs/neomemmanager.h"
#include "../../libs/neoqueue.h"
#include "../../libs/neolog.h"
#include "../../libs/neothread.h"
#include "../../libs/neobaselib.h"
#include "../../libs/factory.h"
#include "../../libs/singleton.h"
#include "../../libs/bridge.h"
#include "../../libs/adapter.h"
#include "../../libs/decorator.h"
#include "../../libs/composite.h"
#include "../../libs/proxy.h"
#include "../../libs/observer.h"
#include "../../libs/state.h"
#include "../../libs/memento.h"
#include "../../libs/neoreflect.h"
#include "../../libs/chainofresp.h"
#include "../../libs/iterator.h"

*/

#include "../../libs/neomessager.h"
#include "../../libs/neohandler.h"
#include "../../libs/neolooper.h"
#include "../../libs/neobasicthread.h"
#include "../../libs/neoprocesssync.h"
#include "../../libs/neosocketserver.h"
#include "../../libs/neosocketclient.h"

#include "../../include/neodebug.h"
#include "../../include/neomemmanager.h"
#include "../../include/neoqueue.h"
#include "../../include/neolog.h"
#include "../../include/neothread.h"
#include "../../include/neobaselib.h"
#include "../../include/factory.h"
#include "../../include/singleton.h"
#include "../../include/bridge.h"
#include "../../include/adapter.h"
#include "../../include/decorator.h"
#include "../../include/composite.h"
#include "../../include/proxy.h"
#include "../../include/observer.h"
#include "../../include/state.h"
#include "../../include/memento.h"
#include "../../include/neoreflect.h"
#include "../../include/chainofresp.h"
#include "../../include/iterator.h"

using namespace NEOLIB;
using namespace std;

#ifndef WIN32
	class pro{
	public:
		pro()
		{
			printf("pro was created!\n");
		}
	
	};
#endif

int main(int argc,char **argv,char *env[])
{
	char str[50] = "\0";
	char buffer[50] = "\0";
	int len = SafePrintf(str,15,"safeprintftest");
	printf("SafePrintf-->>%s length=%d\r\n", str,len);

	OutputFileOrScreen("log.txt","a+","%s",str);

	len = GetTimeStamp(str,26);
	printf("GetTimeStamp-->>%s length=%d\r\n", str,len);

	len = Debug_bin_ASC(buffer,str,37);
	printf("Debug_bin_ASC-->>\n%s length=%d\r\n", buffer,len);
	len = Debug_bin_HEX(buffer,str,14);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	printf("Debug_bin_HEX-->>\n%s length=%d\r\n", buffer,len);
	printf("************************************************\n");
	Debug_bin(buffer,50);

	CNEOLowDebug *debug = new CNEOLowDebug(".","neolibs");
	CNEOMemPoolWithLock *mempool = new CNEOMemPoolWithLock(debug);
	
	void * p = mempool->Malloc(4,"test mem");
	mempool->Malloc(5,"test mem6");
	mempool->Malloc(16,"test mem2");
	mempool->Malloc(64,"test mem3");
	mempool->PrintInfo();
	mempool->PrintTree();
	mempool->Free(p);
	mempool->PrintInfo();
	mempool->PrintTree();
	mempool->Malloc(4,"test mem6");
	mempool->PrintInfo();
	mempool->PrintTree();
	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEODynamicBuffer>>>>>>>>>>>>>>>>>>>>>>>>\n");
	CNEODynamicBuffer *dbufer = new CNEODynamicBuffer(mempool);
	dbufer->SetSize(500);
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();
	dbufer->InsertSpaceToHead(100);
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();
	dbufer->AddSpaceToTail(100);
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();
	dbufer->CutHead(100);
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();
	//dbufer->CutHead(700);
	//dbufer->m_pMemPool->PrintInfo();
	//dbufer->m_pMemPool->PrintTree();
	dbufer->SetInt(16);
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();
	printf("%d \n",dbufer->GetInt());
	dbufer->SetShort(5);
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();
	printf("%d \n",dbufer->GetShort());
	dbufer->SetChar(25);
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();
	printf("%d \n",dbufer->GetChar());

	char a[50] = "1532dsdsdasdsadwqqewaeaw";
	dbufer->AddData(a,sizeof(a));
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();
	dbufer->InsertDataToHead(a,sizeof(a));
	dbufer->m_pMemPool->PrintInfo();
	dbufer->m_pMemPool->PrintTree();

	char b[200] = {""};
	dbufer->BinCopyTo(b,200);
	printf("%s\n",b);

	dbufer->Printf("%s\n",a);
	char c[50] = "ewdadwadawa";
	printf("%d\n",dbufer->memcmp(c,20));
	printf("%d\n",dbufer->strcmp(c));

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOStaticBuffer>>>>>>>>>>>>>>>>>>>>>>>>\n");
	CNEOStaticBuffer *sbufer = new CNEOStaticBuffer(mempool);
	sbufer->SetSize(1024);
	sbufer->InsertSpaceToHead(100);
	sbufer->AddSpaceToTail(200);
	sbufer->CutHead(200);
	sbufer->CutTail(200);
	sbufer->SetInt(16);
	printf("%d\n",sbufer->GetInt());
	sbufer->SetShort(35);
	printf("%d\n",sbufer->GetShort());
	sbufer->SetChar(100);
	printf("%d\n",sbufer->GetChar());
	sbufer->AddData(a,sizeof(a));
	printf("%s\n",sbufer->m_pData);
	sbufer->InsertDataToHead(c,sizeof(c));
	printf("%s\n",sbufer->m_pData);
	char d[200] = {""};
	sbufer->BinCopyTo(d,199);
	printf("%s\n",d);

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOPopBuffer>>>>>>>>>>>>>>>>>>>>>>>>\n");
	char pop[1000] = {""};
	const char * pop1 = "abb";
	CNEOPopBuffer *pbufer = new CNEOPopBuffer(pop,1000);
	pbufer->AddLast(pop1,strlen(pop1));
	pbufer->PrintInside();
	pbufer->AddLast("asdcvf",6);
	pbufer->PrintInside();
	pbufer->AddLast("1234567890",10);
	pbufer->PrintInside();
	pbufer->AddLast("1234567890",10);
	pbufer->PrintInside();
	pbufer->AddLast(dbufer);
	pbufer->PrintInside();
	pbufer->AddLast(sbufer);
	pbufer->PrintInside();
	printf("%d\n",pbufer->GetTokenCount());
	printf("%d\n",pbufer->GetAllBytes());
	printf("%d\n",pbufer->GetFirstTokenLength());
	char out[200] = {""};
	pbufer->GetFirst(out,200);
	printf("%s\n",out);
	pbufer->GetFirst(dbufer);
	pbufer->GetFirst(sbufer);
	pbufer->DeleteFirst();
	pbufer->PrintInside();
	pbufer->GetAndDeleteFirst(out,200);
	printf("%s\n",out);
	pbufer->PrintInside();
	pbufer->MoveAllData(NULL,NULL);
	pbufer->PrintInside();

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOMemQueue>>>>>>>>>>>>>>>>>>>>>>>>\n");
	CNEOMemQueue *mbufer = new CNEOMemQueue(debug,mempool,"test");
	mempool->PrintInfo();
	mempool->PrintTree();
	printf("%d\n",mbufer->GetTokenCount());
	mbufer->AddLast(pop1,1);
	mempool->PrintInfo();
	mempool->PrintTree();
	printf("%d\n",mbufer->GetTokenCount());
	mbufer->AddLast("asdcvf",6);
	mempool->PrintInfo();
	mempool->PrintTree();
	printf("%d\n",mbufer->GetTokenCount());
	mbufer->AddLast("12345678901234567890123456789012345678901234567890",50);
	mempool->PrintInfo();
	mempool->PrintTree();
	printf("%d\n",mbufer->GetTokenCount());
	printf("%d\n",mbufer->GetFirstLength());
	mbufer->PrintInside();
	mbufer->GetFirst(out,200);
	printf("%s\n",out);
	mbufer->GetFirst(dbufer);
	mbufer->DeleteFirst();
	mbufer->PrintInside();
	mbufer->AddLast("12345678901234567890123456789012345678901234567890",50);
	mbufer->PrintInside();
	mbufer->GetAndDeleteFirst(out,200);
	printf("%s\n",out);
	mbufer->PrintInside();
	mbufer->GetAndDeleteFirst(out,200);
	printf("%s\n",out);
	mbufer->PrintInside();
	mbufer->AddLast("abcdefghij",10);
	mbufer->PrintInside();
	char out1[100]="";
	mbufer->PopFromFirst(out1,100);
	printf("%s\n",out1);
	mbufer->PrintInside();
	mbufer->PushToLast(out1,100);
	mbufer->PrintInside();
	mbufer->WriteToFile("queue");
	mbufer->PrintInside();
	mbufer->ReadFromFile("queue");
	mbufer->PrintInside();

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOMemQueueWithLock>>>>>>>>>>>>>>>>>>>>>>>>\n");
	CNEOMemQueueWithLock *mlbufer = new CNEOMemQueueWithLock(debug,mempool,"test");
	printf("%d\n",mlbufer->GetTokenCount());
	mlbufer->AddLast(pop1,1);
	printf("%d\n",mlbufer->GetTokenCount());
	mlbufer->AddLast("asdcvf",6);
	printf("%d\n",mlbufer->GetTokenCount());
	mlbufer->AddLast("12345678901234567890123456789012345678901234567890",50);
	mempool->PrintInfo();
	mempool->PrintTree();
	printf("%d\n",mlbufer->GetTokenCount());
	mlbufer->PrintInside();
	mlbufer->GetFirst(out,200);
	printf("%s\n",out);
	mlbufer->GetFirst(dbufer);
	mlbufer->DeleteFirst();
	mlbufer->PrintInside();
	mlbufer->AddLast("12345678901234567890123456789012345678901234567890",50);
	mlbufer->PrintInside();
	mlbufer->GetAndDeleteFirst(out,200);
	printf("%s\n",out);
	mlbufer->PrintInside();
	mlbufer->GetAndDeleteFirst(out,200);
	printf("%s\n",out);
	mlbufer->PrintInside();
	char out2[100]="";
	mlbufer->PopFromFirst(out2,100);
	printf("%s\n",out2);
	mlbufer->PrintInside();
	mlbufer->PushToLast(out2,100);
	mlbufer->PrintInside();
	mlbufer->WriteToFile("queue1");
	mlbufer->PrintInside();
	mlbufer->ReadFromFile("queue1");
	mlbufer->PrintInside();

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOLog>>>>>>>>>>>>>>>>>>>>>>>>\n");
	CNEOLog *plog = new CNEOLog(debug,mempool,".","test");
	plog->_XGDebugForBin("_XGDebugForBin",15);
	plog->_XGSysLog("_XGSysLog:%d",15);
	plog->_XGDebug("_XGDebug:%s","hahahhah");
	plog->_XGDebug2("_XGDebug2:%s","asdsadada");

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOThreadPool>>>>>>>>>>>>>>>>>>>>>>>>\n");
	CNEOThreadPool *pthreadpool = new CNEOThreadPool(debug);
	/*pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	Sleep(OPEN_THREAD_DELAY);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	Sleep(OPEN_THREAD_DELAY);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	Sleep(OPEN_THREAD_DELAY);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	Sleep(OPEN_THREAD_DELAY);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);*/
	Sleep(OPEN_THREAD_DELAY);
	pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);
	Sleep(OPEN_THREAD_DELAY);
	//pthreadpool->ThreadPoolRegTask(printThreadInfo,pthreadpool);

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOTaskPool>>>>>>>>>>>>>>>>>>>>>>>>\n");
	CNEOTaskPool *ptaskpool = new CNEOTaskPool(debug,mempool,plog);
	ptaskpool->PrintInfo();
	ptaskpool->RegisterATask(printTaskPoolInfo,ptaskpool);

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOTaskRunInfo>>>>>>>>>>>>>>>>>>>>>>>>\n");
	SNEOTaskRunInfo taskInfo;
	CNEOTaskRunInfo *pTaskRunInfo = new CNEOTaskRunInfo(&taskInfo);
	Sleep(OPEN_THREAD_DELAY);

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOTaskRun>>>>>>>>>>>>>>>>>>>>>>>>\n");
    CNEOTaskRun *pTaskRun = new CNEOTaskRun(mempool,plog,ptaskpool);

	printf(">>>>>>>>>>>>>>>>>>>>>>>>CNEOBaseLibrary>>>>>>>>>>>>>>>>>>>>>>>>\n");
	//CNEOBaseLibrary *pBaseLib = new CNEOBaseLibrary("baselib",".","log",NULL);
	CNEOBaseLibrary *pBaseLib = CNEOBaseLibrary::getInstance("baselib",".","log",NULL);
	CNEOBaseLibrary *pBaseLib1 = CNEOBaseLibrary::getInstance("baselib",".","log",NULL);


	printf(">>>>>>>>>>>>>>>>>>>>>>Factory>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
#ifdef WIN32
	class pro{
	public:
		pro()
		{
			printf("pro was created!\n");
		}
	
	};
#endif

	Factory<pro> factory;
	factory.createProduct1();
	Factory<pro,pro,pro,pro,pro> factory5;
	factory5.createProduct1();
	factory5.createProduct4();

	printf(">>>>>>>>>>>>>>>>>>>>>>Singleton>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	SingletonWithLock<CNEOBaseLibrary>::getInstance()->m_szAppName;

	printf(">>>>>>>>>>>>>>>>>>>>>>bridge>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	AbstractBridgeImp* imp = new ConcreteAbstractionImpA();
	AbstractBridge* abs = new RefinedAbstraction(imp);
	abs->Print(); 
	imp->Print();

    printf(">>>>>>>>>>>>>>>>>>>>>>adapter>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    //classadapter
    AdapterTargetTest* adt = new AdpterTest1();
    adt->request1();
    adt->request2();

    //objectadapter
    AdapteeTest* ade = new AdapteeTest;
    AdapterTargetTest* adt2 = new AdpterTest2(ade);
    adt2->request1();
    adt2->request2();

    printf(">>>>>>>>>>>>>>>>>>>>>>decorator>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    Component* com = new ConcreteComponent();

    //以下相当于把Component（七天大圣）装饰成为鱼儿
    Component *dec1 = new ConcreteDecorator(com);
    dec1->Print();
    delete dec1;

    printf(">>>>>>>>>>>>>>>>>>>>>>composite>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    Leaf* l = new Leaf(); 
    l->Print();
    Composite<ComponentTest>* com1 = new Composite<ComponentTest>();
    com1->Add(l);
    com1->Print();
    ComponentTest* ll = com1->GetChild(0);
    ll->Print();

    printf(">>>>>>>>>>>>>>>>>>>>>>proxy>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    Subject *sub = new ConcreteSubject();
    ProxyTest *proxy = new ProxyTest(sub);
    proxy->PreRequest();
    proxy->Request();
    proxy->PostRequest();

    printf(">>>>>>>>>>>>>>>>>>>>>>observer>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    Watched *watched = new Watched();
    Watcher *watcher = new Watcher(watched);
    Watcher1 *watcher1 = new Watcher1(watched);
    //watched改变数据，会通过Watcher打印出来
    watched->SetData("hello world!");
    watched->SetData("grow up!");
    watched->SetData("grow up!");

    printf(">>>>>>>>>>>>>>>>>>>>>>observer>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    StateTestA *sa = new StateTestA();
    ContextTest *ct = new ContextTest();
    ct->SetState(sa);
    ct->Print();

    printf(">>>>>>>>>>>>>>>>>>>>>>memento>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    Originator<string> *o = new Originator<string>();
    Caretaker *caretaker = new Caretaker();

    o->SetState("on");
    caretaker->SetMemento(o->CreateBackups());
    o->Print();
    caretaker->GetMemento()->Print();
    o->SetState("off");
    o->Print();
    o->RestoreBackups(caretaker->GetMemento());
    o->Print();
    caretaker->GetMemento()->Print();
     printf(">>>>>>>>>>>>>>>>>>>>>>neoreflect>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    NEODynBaseClass *pVar1 = (NEODynBaseClass*)ClassFactory::getInstance()->GetClassByName("NEODynBaseClass") ;  
    NEODynBaseClass *pVar = (NEODynBaseClass*)ClassFactory::getInstance()->GetClassByName("CKHelloClass") ;
    pVar->RegistProperty();  
   
    int pValue = 123456 ;  
   
    pVar->m_PropertyMap["setm_pValue"](pVar, &pValue) ;  
    pVar->Print() ;  

    printf(">>>>>>>>>>>>>>>>>>>>>>chain of responsibility>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    Handle* h1 = new ConcreteHandle();
    Handle* h2 = new ConcreteHandle();
    h1->SetSuccessor(h2);
    h1->HandleRequest();

    printf(">>>>>>>>>>>>>>>>>>>>>>iterator>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    int tempofit = 0;
    NEOList<int> *nlist = new NEOList<int>(0,tempofit);
    GlobalList<int> *glist = new GlobalList<int>(0,tempofit);
    int tempofit1 = 1;
    NEOList<int> *nlist1 = new NEOList<int>(1,tempofit1);
    GlobalList<int> *glist1 = new GlobalList<int>(1,tempofit1);
    int tempofit8 = 8;
    NEOList<int> *nlist8 = new NEOList<int>(8,tempofit8);
    GlobalList<int> *glist8 = new GlobalList<int>(8,tempofit8);
    int tempofit3 = 3;
    NEOList<int> *nlist3 = new NEOList<int>(3,tempofit3);
    GlobalList<int> *glist3 = new GlobalList<int>(3,tempofit3);
    GlobalList<int>::iterator glt =  GlobalList<int>::begin();
    ++glt;
    printf("%d\r\n",*glt);
    nlist->Print();
    NEOList<int>::iterator lt = NEOList<int>::begin();
    printf("%d\r\n",*lt);
    ++lt;
    printf("%d\r\n",*lt);
    ++lt;
    printf("%d\r\n",*lt);
    ++lt;
    printf("%d\r\n",*lt);
    ++lt;
    
    printf(">>>>>>>>>>>>>>>>>>>>>>handler looper test>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    //Thread *t = new Thread();
   // t->start();

    /*ThreadWithLoop *tLoop = new ThreadWithLoop();
    tLoop->start();
    Looper * mainLooper= NULL;
    tLoop->getMyLooper(&mainLooper);
    //mainLooper = tLoop->getMyLooper();
    Handler *handler = new Handler(mainLooper);
    Message *msg2 = handler->obtainMessage(2);
    handler->sendMessage(*msg2);
    Message *msg = handler->obtainMessage(1);
    handler->sendMessage(*msg);*/
    printf(">>>>>>>>>>>>>>>>>>>>>>socket test server>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    NeoServer * server =new  NeoServer("127.0.0.1",8888);

    printf(">>>>>>>>>>>>>>>>>>>>>>socket test client>>>>>>>>>>>>>>>>>>>>>>>>>>\n");


    //NeoClient * client =new  NeoClient("127.0.0.1",8888);

#ifndef WIN32
	//linux 主线程退出之后，子线程也会退出
	while(1)
	{}
#endif

#ifdef WIN32
    system("pause");
#else
#endif
    return 0;
}