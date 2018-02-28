/*
==========================================================================
* �����CIOCPModel�Ǳ�����ĺ����࣬����˵��WinSock�������˱��ģ���е���ɶ˿�(IOCP)��ʹ�÷���

* ���е�IOContext���Ƿ�װ������ÿһ���ص������Ĳ���

* ����˵���˷������˽�����ɶ˿ڡ������������̡߳�Ͷ��Recv����Ͷ��Accept����ķ��������еĿͻ��������Socket����Ҫ�󶨵�IOCP�ϣ����дӿͻ��˷��������ݣ�������ûص�������

*�÷�������һ�����࣬���ػص�����

Author: TTGuoying

Date: 2018/02/07 16:22

==========================================================================
*/
#pragma once
#include "Common.h"


// IOCP����
class IOCPBase
{
public:
	IOCPBase();
	~IOCPBase();

	// ��ʼ����
	BOOL Start(int port = 10240, int maxConn = 2000, int maxIOContextInPool = 256, 
		int maxSocketContextInPool = 200);
	// ֹͣ����
	void Stop();
	// ��ָ���ͻ��˷�������
	BOOL SendData(RecvSendData *data);

	// ��ȡ������IP
	wstring GetLocalIP();

	// ��ȡ��ǰ������
	ULONG GetConnectCnt() { return connectCnt;  }

	// ��ȡ��ǰ������
	UINT GetPort() { return port; }

	// �¼�֪ͨ����(���������ش��庯��)
	// ������
	virtual void OnConnectionEstablished(ULONG connectID) = 0;
	// ���ӹر�
	virtual void OnConnectionClosed(ULONG connectID) = 0;
	// �����Ϸ�������
	virtual void OnConnectionError(ULONG connectID, int error) = 0;
	// ���������
	virtual void OnRecvCompleted(RecvSendData *data) = 0;
	// д�������
	virtual void OnSendCompleted(ULONG connectID, ULONG dataID) = 0;

private:
	HANDLE					stopEvent;				// ֪ͨ�߳��˳���ʱ��
	HANDLE					completionPort;			// ��ɶ˿�
	HANDLE					*workerThreads;			// �������̵߳ľ��ָ��
	int						workerThreadNum;		// �������̵߳�����
	wstring					IP;						// ����IP
	int						port;					// �����˿�
	SocketContext			*listenSockContext;		// ����socket��Context
	ULONG					connectCnt;				// ��ǰ����������
	ULONG					acceptPostCnt;			// ��ǰͶ�ݵĵ�Accept����

	vector<SocketContext *> vecSockContexts;			// socketContext�б�
	SRWLOCK					srwLockSC;				// socketContext�б��д��

	LPFN_ACCEPTEX			fnAcceptEx;				//AcceptEx����ָ��
	//GetAcceptExSockAddrs;����ָ��
	LPFN_GETACCEPTEXSOCKADDRS	fnGetAcceptExSockAddrs;

	static DWORD WINAPI WorkerThreadProc(LPVOID lpParam); // �����̺߳���

	// ��ʼ��IOCP
	BOOL InitializeIOCP();
	// ��ʼ��Socket
	BOOL InitializeListenSocket();
	// �ͷ���Դ
	void DeInitialize();
	// socket�Ƿ���
	BOOL IsSocketAlive(SOCKET sock);
	// ��ȡ����CPU������
	int GetNumOfProcessors();
	// �����(Socket)�󶨵���ɶ˿���
	BOOL AssociateWithIOCP(SocketContext *sockContext);
	// Ͷ��IO����
	BOOL PostAccept(SocketContext *sockContext, IOContext *ioContext);
	BOOL PostRecv(SocketContext *sockContext, IOContext *ioContext);
	BOOL PostSend(SocketContext *sockContext, IOContext *ioContext);

	// IO������
	BOOL DoAccpet(SocketContext *sockContext, IOContext *ioContext);
	BOOL DoRecv(SocketContext *sockContext, IOContext *ioContext);
	BOOL DoSend(SocketContext *sockContext, IOContext *ioContext);
	BOOL DoClose(SocketContext *sockContext);

	void AddSocketContext(SocketContext *sockContext);
	void RemoveSocketContext(SocketContext *sockContext);
	SocketContext *GetContextFromID(ULONG id);
};

