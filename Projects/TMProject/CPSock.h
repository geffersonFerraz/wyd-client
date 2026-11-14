#pragma once

#include "SecureChannel.h"

#define RECV_BUFFER_SIZE 131072
#define SEND_BUFFER_SIZE 131072

#define MAX_KEYWORD_QUEUE 16

#define INIT_CODE 521270033

class CPSock
{
public:
	CPSock();
	~CPSock();

	bool WSAInitialize();
	UINT_PTR StartListen(HWND hWnd, int ip, int port, int WSA);
	UINT_PTR ConnectServer(char* HostAddr, int Port, int ip, int WSA);
	UINT_PTR SingleConnect(char* HostAddr, int Port, int ip, int WSA);
	int Receive();
	char* ReadMessage(int* ErrorCode, int* ErrorType);
	int CloseSocket();
	int AddMessage(char* pMsg, int Size);
	int AddMessage(char* pMsg, int Size, int FixedKeyWord);
	int MeasurePing(const char* HostAddr, int Port, int ip);
	bool SendMessageA();
	int SendOneMessage(char* Msg, int Size);
	int SendOneMessageKeyword(char* Msg, int Size, int Keyword);
	int AddMessage2(char* pMsg, int Size);
	char* ReadMessage2(int* ErrorCode, int* ErrorType);
	void RefreshRecvBuffer();
	void RefreshSendBuffer();

	// Troca de chaves com o servidor, logo depois do INIT_CODE (SecureChannel.h).
	bool SecureHandshake(UINT_PTR tSock);

	UINT_PTR Sock;
	char* pSendBuffer;
	char* pRecvBuffer;
	int nSendPosition;
	int nRecvPosition;
	int nProcPosition;
	int nSentPosition;
	int Init;

	// Chaves desta conexao e a mensagem de janela que avisa do socket.
	SecureChannel Secure;
	int nNotifyMsg;

	// Sequencia de KeyWord da cifra antiga. O transporte seguro nao a usa;
	// ficam porque a selecao de personagem ainda as preenche.
	char SendQueue[MAX_KEYWORD_QUEUE];
	char RecvQueue[MAX_KEYWORD_QUEUE];

	int SendCount;
	int RecvCount;
	int ErrCount;
};
