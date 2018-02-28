#pragma once
#include "stdafx.h"
#include <vector>
#include <list>
#include <string>

using std::vector;
using std::list;
using std::wstring;

#define INIT_IOCONTEXT_NUM (100)				// IOContextPool�еĳ�ʼ����
#define BUFF_SIZE (1024*4)						// I/O ����Ļ�������С
#define WORKER_THREADS_PER_PROCESSOR (2)		// ÿ���������ϵ��߳���
#define MAX_POST_ACCEPT (10)					// ͬʱͶ�ݵ�Accept����
#define EXIT_CODE	(-1)						// ���ݸ�Worker�̵߳��˳��ź�
#define DEFAULT_IP	(L"127.0.0.1")				// Ĭ��IP��ַ
#define DEFAULT_PORT	(10240)					// Ĭ�϶˿�

// �ͷ�ָ��ĺ�
#define RELEASE(x)			{if(x != NULL) {delete x; x = NULL;}}
// �ͷž���ĺ�
#define RELEASE_HANDLE(x)	{if(x != NULL && x != INVALID_HANDLE_VALUE) { CloseHandle(x); x = INVALID_HANDLE_VALUE; }}
// �ͷ�Socket�ĺ�
#define RELEASE_SOCKET(x)	{if(x != INVALID_SOCKET) { closesocket(x); x = INVALID_SOCKET; }}

#ifndef TRACE
#include <atltrace.h>

#define TRACE							AtlTrace
#define TRACE0(f)						TRACE(f)
#define TRACE1(f, p1)					TRACE(f, p1)
#define TRACE2(f, p1, p2)				TRACE(f, p1, p2)
#define TRACE3(f, p1, p2, p3)			TRACE(f, p1, p2, p3)
#define TRACE4(f, p1, p2, p3, p4)		TRACE(f, p1, p2, p3, p4)
#define TRACE5(f, p1, p2, p3, p4, p5)	TRACE(f, p1, p2, p3, p4, p5)
#endif


const char* WcharToUtf8(const wchar_t *pwStr);
const wchar_t* Utf8ToWchar(const char *pStr);
CDuiString GetInstancePath();
CDuiString GetCurrentTimeString();

enum IO_OPERATION_TYPE
{
	NULL_POSTED,		// ���ڳ�ʼ����������
	ACCEPT_POSTED,		// Ͷ��Accept����
	SEND_POSTED,		// Ͷ��Send����
	RECV_POSTED,		// Ͷ��Recv����
};

class IOContext
{
public:
	WSAOVERLAPPED		overLapped;		// ÿ��socket��ÿһ��IO��������Ҫһ���ص��ṹ
	SOCKET				ioSocket;		// ��IO������Ӧ��socket
	WSABUF				wsaBuf;			// ���ݻ���
	IO_OPERATION_TYPE	ioType;			// IO��������
	DWORD				numOfBytes;		// �շ����ֽ���
	ULONG				sendDataID;		// �������ݰ���ID

	IOContext()
	{
		ZeroMemory(&overLapped, sizeof(overLapped));
		ioSocket = INVALID_SOCKET;
		wsaBuf.buf = (char *)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, BUFF_SIZE);
		wsaBuf.len = BUFF_SIZE;
		ioType = NULL_POSTED;
		numOfBytes = 0;
		sendDataID = 0;
	}

	~IOContext()
	{
		RELEASE_SOCKET(ioSocket);

		if (wsaBuf.buf != NULL)
			::HeapFree(::GetProcessHeap(), 0, wsaBuf.buf);
	}

	void Reset()
	{
		if (wsaBuf.buf != NULL)
			ZeroMemory(wsaBuf.buf, BUFF_SIZE);
		else
			wsaBuf.buf = (char *)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, BUFF_SIZE);
		ZeroMemory(&overLapped, sizeof(overLapped));
		ioType = NULL_POSTED;
		numOfBytes = 0;
		sendDataID = 0;
	}
};

// ���е�IOContext������(IOContext��)
class IOContextPool
{
private:
	list<IOContext *> contextList;
	CRITICAL_SECTION csLock;

public:
	IOContextPool()
	{
		InitializeCriticalSection(&csLock);
		contextList.clear();

		EnterCriticalSection(&csLock);
		for (size_t i = 0; i < INIT_IOCONTEXT_NUM; i++)
		{
			IOContext *context = new IOContext;
			contextList.push_back(context);
		}
		LeaveCriticalSection(&csLock);

	}

	~IOContextPool()
	{
		EnterCriticalSection(&csLock);
		for (list<IOContext *>::iterator it = contextList.begin(); it != contextList.end(); it++)
		{
			delete (*it);
		}
		contextList.clear();
		LeaveCriticalSection(&csLock);

		DeleteCriticalSection(&csLock);
	}

	// ����һ��IOContxt
	IOContext *AllocateIoContext()
	{
		IOContext *context = NULL;

		EnterCriticalSection(&csLock);
		if (contextList.size() > 0) //list��Ϊ�գ���list��ȡһ��
		{
			context = contextList.back();
			contextList.pop_back();
		}
		else	//listΪ�գ��½�һ��
		{
			context = new IOContext;
		}
		LeaveCriticalSection(&csLock);

		return context;
	}

	// ����һ��IOContxt
	void RecycleIOContext(IOContext *pContext)
	{
		pContext->Reset();
		EnterCriticalSection(&csLock);
		contextList.push_front(pContext);
		LeaveCriticalSection(&csLock);
	}
};

// ���е�connectIDPool������(connectID��)
class ULongIDPool
{
private:
	list<ULONG> IDList;
	CRITICAL_SECTION csLock;
	ULONG idCnt;

public:
	ULongIDPool()
	{
		idCnt = 0;
		InitializeCriticalSection(&csLock);
		IDList.clear();

		EnterCriticalSection(&csLock);
		for (size_t i = 0; i < INIT_IOCONTEXT_NUM; i++)
		{
			IDList.push_back(i+1);
			idCnt++;
		}
		LeaveCriticalSection(&csLock);

	}

	~ULongIDPool()
	{
		EnterCriticalSection(&csLock);
		IDList.clear();
		LeaveCriticalSection(&csLock);

		DeleteCriticalSection(&csLock);
	}

	// ����һ��ID
	ULONG AllocateID()
	{
		ULONG id = 0;

		EnterCriticalSection(&csLock);
		if (IDList.size() > 0) //list��Ϊ�գ���list��ȡһ��
		{
			id = IDList.front();
			IDList.pop_front();
		}
		else	//listΪ�գ��½�һ��
		{
			idCnt++;
			id = idCnt;
		}
		LeaveCriticalSection(&csLock);

		return id;
	}

	// ����һ��ID
	void RecycleID(ULONG id)
	{
		EnterCriticalSection(&csLock);
		IDList.push_back(id);
		LeaveCriticalSection(&csLock);
	}
};


class SocketContext
{
public:
	SOCKET connSocket;						// ���ӵ�socket
	SOCKADDR_IN clientAddr;					// ���ӵ�Զ�̵�ַ
	ULONG		connectID;					// ���ӵ�ID

private:
	//vector<IOContext*> arrIoContext;		// ͬһ��socket�ϵĶ��IO����
	static IOContextPool ioContextPool;		// ���е�IOContext��
	static ULongIDPool IDPool;				// ���е�IOContext��
	//CRITICAL_SECTION csLock;

public:
	SocketContext()
	{
		//InitializeCriticalSection(&csLock);
		//arrIoContext.clear();
		connSocket = INVALID_SOCKET;
		ZeroMemory(&clientAddr, sizeof(clientAddr));
		connectID = IDPool.AllocateID();
	}

	~SocketContext()
	{
		IDPool.RecycleID(connectID);
		RELEASE_SOCKET(connSocket);

		// �������е�IOContext
		/*for (vector<IOContext*>::iterator it = arrIoContext.begin(); it != arrIoContext.end(); it++)
		{
			ioContextPool.RecycleIOContext(*it);
		}

		EnterCriticalSection(&csLock);
		arrIoContext.clear();
		LeaveCriticalSection(&csLock);

		DeleteCriticalSection(&csLock);*/
	}

	// ��ȡһ���µ�IoContext
	IOContext *AllocateIoContext()
	{
		IOContext *context = ioContextPool.AllocateIoContext();
		/*if (context != NULL)
		{
			EnterCriticalSection(&csLock);
			arrIoContext.push_back(context);
			LeaveCriticalSection(&csLock);
		}*/
		context->ioSocket = this->connSocket;
		return context;
	}

	// ���������Ƴ�һ��ָ����IoContext
	void RecycleIOContext(IOContext* pContext)
	{
		ioContextPool.RecycleIOContext(pContext);
		/*for (vector<IOContext*>::iterator it = arrIoContext.begin(); it != arrIoContext.end(); it++)
		{
			if (pContext == *it)
			{

				EnterCriticalSection(&csLock);
				arrIoContext.erase(it);
				LeaveCriticalSection(&csLock);

				break;
			}
		}*/
	} 
};

// �����շ�������
class RecvSendData
{
public:
	RecvSendData(BYTE *data, size_t bytes, ULONG connectID, ULONG sendDataID = 0)
	{
		buff = data;
		this->bytes = bytes;
		this->sendDataID = sendDataID;
		this->connectID = connectID;
	}

	RecvSendData(IOContext *ioContext, ULONG connectID, ULONG sendDataID = 0)
	{
		buff = (BYTE*)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ioContext->numOfBytes);
		bytes = ioContext->numOfBytes;
		memcpy_s(buff, bytes, ioContext->wsaBuf.buf, bytes);
		this->sendDataID = sendDataID;
		this->connectID = connectID;
	}

	~RecvSendData()
	{
		if (buff)
			::HeapFree(::GetProcessHeap(), 0, buff);
		bytes = 0;
	}

	BYTE		*buff;
	size_t		bytes;
	ULONG		sendDataID;
	ULONG		connectID;
};

class Server;
struct ProcessRecvDataParam
{
	Server *server;
	RecvSendData *data;
};