#pragma once

#include <fstream>
#include <iterator>
#include <algorithm>

#include <functional>
#include <map>
#include <vector>
#include <tuple>
#include <array>
#include <excpt.h>

#include "NewApp.h"
#include "CPSock.h"
#include "ObjectManager.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "TimerManager.h"
#include "TMScene.h"
#include "RenderDevice.h"
#include "EventTranslator.h"
#include "dsutil.h"
#include "SControl.h"
#include "TMPaths.h"
#include "ResourceControl.h"
#include "Basedef.h"
#include "HudController.h" 

class HudElement
{


public:
	struct stObject_Human
	{
		int GroupType;
		MSG_pRecaptcha reCaptcha;
		MSG_pFilterDrop DropFilterEvent;
	} Object_Human;


	struct stDropListEvent
	{
		std::vector<stDropList> Drop;

		int DropPage;

		st_DropListMobSelected DropSelected;

	}DropListEvent;

	struct stSearchDropListEvent
	{
		std::vector<stDropList> Drop;

		int DropPage;

		st_DropListMobSelected DropSelected;

	}DropSearchListEvent;

	struct stPersonagem
	{
		int Cash;
		char SenhaGrupo[12];
		int UsersRegistres;
		int Honrar;


	}Personagem;

	struct stObject_World
	{

		int* inventarioGrid[4];
		int* LabelExpBonus;
		int* LabelDropBonus;
		int* PanelFiltroDrop;

		SPanel* PanelCharacter3;

	}Object_World;
	
	struct stArenaReal
	{

		MSG_ArenaReal Packet;

	}ArenaReal;

	struct
	{
		char Status;
		bool Info[8];
	} Lock;

	p528 Ranking;
	MSG_MISSIONINFO gMissionPass;

	char LastMission[256];    // Armazena a data da última missão
	char CurrentMission[256]; // Armazena a data da próxima missão
	MSG_ListaGrupo gListgrup;
};

class HudController : public HudElement
{

public:
	static void Init();

public:
	static void DiaryMission(int handle);
};

extern HudController _HudControl;
