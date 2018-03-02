#pragma once
#include "IOCPBase.h"

class Server;
class NetwordModule : public IOCPBase
{
public:
	NetwordModule(Server *server);
	~NetwordModule();

public:
	// ������
	virtual void OnConnectionEstablished(ULONG connectID);
	// ���ӹر�
	virtual void OnConnectionClosed(ULONG connectID);
	// �����Ϸ�������
	virtual void OnConnectionError(ULONG connectID, int error);
	// ���������
	virtual void OnRecvCompleted(RecvSendData *data);
	// д�������
	virtual void OnSendCompleted(ULONG connectID, ULONG dataID);

	Server *server;
};

