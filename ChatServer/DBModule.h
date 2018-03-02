#pragma once
#include "stdafx.h"
#include "SQLite.h"

// ��¼��Ϣ
struct SignInParam
{
	CString		account;
	CString		password;
	ULONG		connectID;
};

// �û�
struct User
{
	ULONG		userID;
	int			headerImg;
	CString		nickName;
	int			sexulity;
	CString		account;
	CString		password;
	CString		signature;
	CString		area;
	CString		phone;
	ULONG		connectID;
};

// ��¼״̬
struct SignInStatus
{
	ULONG		userID;
	BOOL		online;
	ULONG		connectID;
};

// ����
struct Friend
{
	ULONG		userID;
	ULONG		friendID;
	CString		remark;
	CString		addTime;
	ULONG		connectID;
};

// ���������¼
struct SingleChatRecord
{
	ULONG		msgID;
	ULONG		userID;
	ULONG		friendID;
	CString		msgTime;
	CString		message;
	ULONG		connectID;
};

// �����������߼�¼
struct SingleUnSendMsg
{
	ULONG		msgID;
	ULONG		connectID;
};

// Ⱥ��
struct Group
{
	ULONG		groupID;
	CString		groupName;
	ULONG		ownerID;
	int			numOfMember;
	ULONG		connectID;
};

// Ⱥ���Ա
struct GroupMember
{
	ULONG		groupID;
	ULONG		UserID;
	ULONG		connectID;
};

// Ⱥ�������¼
struct GroupChatRecord
{
	ULONG		msgID;
	ULONG		groupID;
	ULONG		sendUserID;
	CString		msgTime;
	CString		message;
	ULONG		connectID;
};

// Ⱥ���������߼�¼
struct GroupUnSendMsg
{
	ULONG		msgID;
	ULONG		UserID;
	ULONG		connectID;
};

class Server;
class DBModule
{
public:
	DBModule(Server *server);
	~DBModule();

	BOOL Init();
	BOOL SignIn(SignInParam *param);
	void SetSignInStatus(ULONG userID, BOOL online);
	BOOL GetSignInStatus(ULONG userID);
	BOOL SignUp(User *user);
	BOOL GetFriends(ULONG userID, ULONG connectID);
	User *GetUser(ULONG userID);
	User *GetUser(CString account);


	//���ݿ����
	SQLite sqliteQuery;
	SQLite sqliteModify;
	CRITICAL_SECTION csLock;
	Server *server;
};

