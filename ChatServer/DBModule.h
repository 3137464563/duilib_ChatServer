#pragma once
#include "stdafx.h"
#include "SQLite.h"

// ��¼��Ϣ
struct SignInParam
{
	LPCTSTR		account;
	LPCTSTR		password;
	ULONG		connectID;
};

// �û�
struct User
{
	ULONG		userID;
	int			headerImg;
	LPCTSTR		nickName;
	int			sexulity;
	LPCTSTR		account;
	LPCTSTR		password;
	LPCTSTR		signature;
	LPCTSTR		area;
	LPCTSTR		phone;
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
	LPCTSTR		remark;
	LPCTSTR		addTime;
	ULONG		connectID;
};

// ���������¼
struct SingleChatRecord
{
	ULONG		msgID;
	ULONG		userID;
	ULONG		friendID;
	LPCTSTR		msgTime;
	LPCTSTR		message;
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
	LPCTSTR		groupName;
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
	LPCTSTR		msgTime;
	LPCTSTR		message;
	ULONG		connectID;
};

// Ⱥ���������߼�¼
struct GroupUnSendMsg
{
	ULONG		msgID;
	ULONG		UserID;
	ULONG		connectID;
};

class DBModule
{
public:
	DBModule();
	~DBModule();

	BOOL Init();
	BOOL SignIn(SignInParam *param);
	BOOL SignUp(User *user);


	//���ݿ����
	SQLite sqliteQuery;
	SQLite sqliteModify;
	CRITICAL_SECTION csLock;
};

