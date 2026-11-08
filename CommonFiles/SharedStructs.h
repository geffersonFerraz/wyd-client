 #pragma once
/*
Este header, ira compartilhar as structs entre servidor e client a medida que o mesmo for sendo atualizado
outros headers também poder´ser criado como de constantes por exemplo.
*/

#ifndef _SHAREDSTRUCTS_ // Last updated 27/01/2013

#define _SHAREDSTRUCTS_

#define _MSG      	short		  Size;			\
					char		  KeyWord;		\
					char		  CheckSum;		\
					short		  Type;			\
					short		  ID;			\
					unsigned int  ClientTick;

struct MSG_HSTANDARD
{
	short Size;
	char KeyWord;
	char CheckSum;
	short Type;
	short ID;
	int Tick;
};

#define _PCKHEADER MSG_STANDARD Header

#ifdef _WYDCLIENT
struct		 MSG_STANDARD
{
	WORD Size;
	BYTE KeyWord;
	BYTE CheckSum;
	WORD Type;
	WORD ID;
	DWORD Tick;
};
#endif // _WYDCLIENT
struct		  MSG_AccountLogin
{
	MSG_STANDARD Header;

	char Password[12];
	char Login[16];

	char Unknow[52];
	UINT32 CliVer;
	UINT32 Unknow_84;
	unsigned char Mac[8];
	char Keys[12];

};
#pragma pack(push, 1)

#pragma pack(pop)

#endif