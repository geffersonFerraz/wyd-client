#include "pch.h"
#include "TMFieldScene.h"
#include "TMGlobal.h"
#include "TMLog.h"
#include "dsutil.h"
#include "DirShow.h"
#include "SControlContainer.h"
#include "SGrid.h"
#include "Mission.h"
#include "MrItemMix.h"
#include "TMGround.h"
#include "TMHuman.h"
#include "TMObjectContainer.h"
#include "TMCamera.h"
#include "TMSun.h"
#include "TMSky.h"
#include "TMSnow.h"
#include "TMRain.h"
#include "TMSkinMesh.h"
#include "TMEffectMesh.h"
#include "TMEffectBillBoard2.h"
#include "TMEffectBillBoard.h"
#include "TMUtil.h"
#include "TMHouse.h"
#include "TMEffectBillBoard4.h"
#include "TMSkillMagicArrow.h"
#include "TMEffectStart.h"
#include "TMSkillJudgement.h"
#include "TMSkillTownPortal.h"
#include "TMEffectLevelUp.h"
#include "TMEffectSkinMesh.h"
#include "SGrid.h"
#include "ItemEffect.h"
#include "TMUtil.h"
#include "TMEffectSWSwing.h"
#include "TMEffectSpark.h"
#include "TMSkillHolyTouch.h"
#include "TMEffectParticle.h"
#include "TMSkillMeteorStorm.h"
#include "TMSkillThunderBolt.h"
#include "TMSkillSlowSlash.h"
#include "TMSkillMagicShield.h"
#include "TMSkillFreezeBlade.h"
#include "TMArrow.h"
#include "TMShade.h"
#include "TMSkillPoison.h"
#include "TMFont3.h"
#include "TMSkillHeavenDust.h"
#include "TMSkillFlash.h"
#include "TMItem.h"
#include "TMCannon.h"
#include "TMEffectCharge.h"
#include "TMSkillExplosion2.h"
#include "TMEffectDust.h"
#include "TMGate.h"
#include "TMScene.h"
#include <wininet.h>
#include <string>  
#include <ctime>
#include <regex> //require c++11
#include "TMDroplistAndFilter.h"

#include <wininet.h>
#include <iostream>
#include <sstream>
#include <algorithm> // Incluir a biblioteca algorithm para usar a função transform 
#include <fstream>
#include <string> 
#include <set> 

unsigned int TMFieldScene::g_dwStartDropWinTime;
unsigned int TMFieldScene::g_dwStartEvocacaoMacro;

unsigned int TMFieldScene::pDropMode = 0;
unsigned int TMFieldScene::pFiltroMode = 0; 

int IndexFilterSelected;
int IndexFilterRemove;


int DropOnPacketEvent(MSG_STANDARD* header)
{
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);

	auto PacketId = header->Type;
	 
	if (PacketId == 0xCB5)
	{ 
		return true;
	}

	return false;
}

int DropOnControlEvent(unsigned int idwControlID, unsigned int idwEvent)
{
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);
	 
	auto GridDrop = (SGridControl*)Scene->m_pControlContainer->FindControl(478473);
	auto TituloInfo = (SText*)Scene->m_pControlContainer->FindControl(5578483);

	auto BtnAtt = (SButton*)Scene->m_pControlContainer->FindControl(478480);
	unsigned int dwServerTime = g_pTimerManager->GetServerTime();
	 
	if (dwServerTime - Scene->g_dwStartDropWinTime < 300)
	{ 
		return 0;
	} 
	Scene->g_dwStartDropWinTime = dwServerTime;
	 

	if (idwControlID == 478486 || idwControlID == 9004) //fechardroplist
	{
		auto pnl = (SPanel*)Scene->m_pControlContainer->FindControl(478471);
		auto pnl2 = (SPanel*)Scene->m_pControlContainer->FindControl(9000);
		if (pnl)
			pnl->SetVisible(false);
		if (pnl2)
			pnl2->SetVisible(false);

		return 1;
	}

	if (idwControlID == 9003 || idwControlID == 9004 || idwControlID == 90006) //filtro drop
	{
		Scene->DropFilter(idwControlID);
		return 1;
	}

	if (idwControlID == 79003)//limpar filtro
	{ 
		MSG_pFilterDrop Pack;
		memset(&Pack, 0, sizeof(Pack)); 
		Pack.Header.Type = 20589;
		Pack.Header.ID = g_pObjectManager->m_dwCharID;
		Pack.Header.Size = sizeof(Pack);
		Pack.Code = 0;
		Pack.Type = Scene->pFiltroMode;
		SendOneMessage((char*)&Pack, sizeof(Pack)); 
		return 1;
	}   
	if (idwControlID == 478472 && Scene->pDropMode == 1)
	{
		if (TituloInfo)
		{
			TituloInfo->SetText((char*)"Itens encontrados", 0);
		}

		Scene->UpdateGridDropList(0);
		return 1;
	}
	if (idwControlID == 478472 && Scene->pDropMode == 2)
	{
		if (TituloInfo)
		{
			TituloInfo->SetText((char*)"Itens encontrados", 0);
		}
		 
		Scene->UpdateGridDropList(0);

		// Scene->UpdateGridDropList(0);
		return 1;
	}

	if (idwControlID >= 578481 && idwControlID <= 578491 || idwControlID == 478481 || idwControlID == 478477)
	{
		Scene->m_pSlotEffect->SetVisible(0);
	}
	if (idwControlID == 73729) //droplist
	{

		auto ListBox = (SListBox*)Scene->m_pControlContainer->FindControl(478472);

		if (ListBox)
		{
			ListBox->Empty();
			ListBox->m_bSelectEnable = 1;
			ListBox->m_bSelectEnable = 1;
		}

		Scene->pDropMode = 1; 

		Scene->UpdateFilterDrop();

		char strsexy[128] = {};
		// Array de textos para cada botão
		const char* buttonTexts[11] = {
			"ARMIA",             // Botão 1
			"AZRAN",             // Botão 2
			"ERION",             // Botão 3
			"KARDEN",            // Botão 4
			"DESERTO",           // Botão 5
			"SUBMUNDO",          // Botão 6
			"DUNGEON",           // Botão 7
			"KEFRA",             // Botão 8
			"LAN",               // Botão 9
			"CARTA_DUELO",   // Botão 10
			"PISTA_RUNAS"   // Botão 11
		};

		// IDs dos botões
		int buttonIDs[11] = {
			578481, 578482, 578483, 578484, 578485,
			578486, 578487, 578488, 578489, 578490, 578491
		};

		// Loop para processar cada botão e aplicar o texto correspondente
		for (int i = 0; i < 11; i++)
		{
			auto pBtn = (SButton*)Scene->m_pControlContainer->FindControl(buttonIDs[i]);
			if (pBtn)
			{
				snprintf(strsexy, sizeof(strsexy), "%s", buttonTexts[i]);
				pBtn->SetText(strsexy);


			}
		}

		if (TituloInfo)
		{
			TituloInfo->SetText((char*)"Itens encontrados", 0);
		}

		 
		GridDrop->Empty();

		Scene->m_pControlContainer->SetFocusedControl(0);
		auto pSearchEdit = (SEditableText*)Scene->m_pControlContainer->FindControl(478491);
		pSearchEdit->SetText((char*)"");
		auto pnl = (SPanel*)Scene->m_pControlContainer->FindControl(478471);
		if (pnl)
		{
			pnl->SetVisible(1);  
		}


		MSG_STANDARDPARM2 stParm{};
		stParm.Header.ID = Scene->m_pMyHuman->m_dwID;
		stParm.Header.Type = 0xA08;
		stParm.Parm1 = 13;
		stParm.Parm2 = 13;
		g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));
		return 1;
	}
	if (idwControlID == 478484 || idwControlID == 478485)
	{
		auto button = (SButton*)Scene->m_pControlContainer->FindControl(idwControlID);
		auto button2 = (SButton*)Scene->m_pControlContainer->FindControl(idwControlID == 478484 ? 478485 : 478484);

		button->SetSelected(true);
		button2->SetSelected(false);

		Scene->UpdateGridDropList(idwControlID - 478484);

		return 1;
	}
	
	if (idwControlID == 478481)
	{
		auto pnl = (SPanel*)Scene->m_pControlContainer->FindControl(9000);

		Scene->m_pDropList->GetGeomControl()->nTextureSetIndex = -563;
		Scene->pDropMode = 1;

		Scene->UpdateFilterDrop();

		pnl->SetPos(395.0f * RenderDevice::m_fHeightRatio, 353.0f * RenderDevice::m_fWidthRatio); 
		pnl->SetSize(245.0f * RenderDevice::m_fHeightRatio, 130.0f * RenderDevice::m_fWidthRatio);
		

		auto GridFiltro = (SGridControl*)Scene->m_pControlContainer->FindControl(9002);
		GridFiltro->SetPos(16.0f * RenderDevice::m_fHeightRatio, 31.0f * RenderDevice::m_fWidthRatio);
		GridFiltro->m_nWidth = 215.0f * RenderDevice::m_fHeightRatio;



		if(GridDrop->m_nNumItem)
		{
			GridDrop->Empty();
		}
		char strsexy[128] = {};
		// Array de textos para cada botão
		const char* buttonTexts[11] = {
			"ARMIA",             // Botão 1
			"AZRAN",             // Botão 2
			"ERION",             // Botão 3
			"KARDEN",            // Botão 4
			"DESERTO",           // Botão 5
			"SUBMUNDO",          // Botão 6
			"DUNGEON",           // Botão 7
			"KEFRA",             // Botão 8
			"LAN",               // Botão 9
			"CARTA_DUELO",   // Botão 10
			"PIST_DE_RUNAS"   // Botão 11
		};

		// IDs dos botões
		int buttonIDs[11] = {
			578481, 578482, 578483, 578484, 578485,
			578486, 578487, 578488, 578489, 578490, 578491
		};

		// Loop para processar cada botão e aplicar o texto correspondente
		for (int i = 0; i < 11; i++)
		{
			auto pBtn = (SButton*)Scene->m_pControlContainer->FindControl(buttonIDs[i]);
			if (pBtn)
			{
				snprintf(strsexy, sizeof(strsexy), "%s", buttonTexts[i]);
				pBtn->SetText(strsexy);   
			}
		}
		GridDrop->m_eGridType = TMEGRIDTYPE::GRID_DROPLIST;// TMEGRIDTYPE::GRID_SKILLB; 
		Scene->m_pControlContainer->SetFocusedControl(0);
		auto pSearchEdit = (SEditableText*)Scene->m_pControlContainer->FindControl(478491);
		pSearchEdit->SetText((char*)"");
		if (BtnAtt)
		{
			if (Scene->pDropMode == 2)
			{
				BtnAtt->SetText((char*)"Salvar");
			}
			if (Scene->pDropMode == 1)
			{
				BtnAtt->SetText((char*)"Atualizar");
			}
		}
		auto TituloInfo2 = static_cast<SText*>(Scene->m_pControlContainer->FindControl(5578482));
		if (TituloInfo2)
		{
			TituloInfo2->SetText((char*)"     Nomes", 0);  // Usa a string formatada para definir o título
		}

		if (TituloInfo)
		{
			TituloInfo->SetText((char*)"Itens encontrados", 0);
		}

		return true;
	}	 

	if (idwControlID == 478477)
	{
		auto ListBox = (SListBox*)Scene->m_pControlContainer->FindControl(478472);

		auto pnl = (SPanel*)Scene->m_pControlContainer->FindControl(9000);

		Scene->m_pDropList->GetGeomControl()->nTextureSetIndex = -779;
	 
		 
		if (ListBox)
		{
			ListBox->Empty();
		}
		if (GridDrop->m_nNumItem)
		{
			GridDrop->Empty();
		}
		char strsexy[128] = {};
		// Array de textos para cada botão
		const char* buttonTexts[11] = {
			"Filtro 1",  // Botão 1
			"Filtro 2",  // Botão 2
			"Filtro 3",  // Botão 3
			"Filtro 4",  // Botão 4
			"Filtro 5",  // Botão 5
			"Filtro 6",  // Botão 6
			"Filtro 7",  // Botão 7
			"Filtro 8",  // Botão 8
			"Filtro 9",  // Botão 9
			"Filtro 10",  // Botão 10
			"Filtro 11"  // Botão 10
		};

		// IDs dos botões
		int buttonIDs[11] = {
			578481, 578482, 578483, 578484, 578485,
			578486, 578487, 578488, 578489, 578490, 578491
		};

		// Loop para processar cada botão e aplicar a lógica de pFiltroSave
		for (int i = 0; i < 11; i++)
		{
			auto pBtn = (SButton*)Scene->m_pControlContainer->FindControl(buttonIDs[i]);
			if (pBtn)
			{
				snprintf(strsexy, sizeof(strsexy), "%s", buttonTexts[i]);
				pBtn->SetText(strsexy);
			}
		}

		GridDrop->m_eGridType = TMEGRIDTYPE::GRID_DROPLIST;
		Scene->pDropMode = 2;

		Scene->UpdateFilterDrop();


		Scene->m_pControlContainer->SetFocusedControl(0);
		auto pSearchEdit = (SEditableText*)Scene->m_pControlContainer->FindControl(478491);
		pSearchEdit->SetText((char*)"");
		if (BtnAtt)
		{
			if (Scene->pDropMode == 2)
				BtnAtt->SetText((char*)"Salvar");
			if (Scene->pDropMode == 1)
				BtnAtt->SetText((char*)"Atualizar");
		}
		auto TituloInfo2 = static_cast<SText*>(Scene->m_pControlContainer->FindControl(5578482));
		if (TituloInfo2)
		{
			TituloInfo2->SetText((char*)"", 0);  // Usa a string formatada para definir o título
		}

		if (TituloInfo)
		{
			TituloInfo->SetText((char*)"", 0);
		} 
		 

		return true;
	}

	if(Scene->pDropMode == 1)
	{

		if (idwControlID == 478480) // atualiza
		{ 
			MSG_STANDARDPARM2 stParm{};
			stParm.Header.ID = Scene->m_pMyHuman->m_dwID;
			stParm.Header.Type = 0xA08;
			stParm.Parm1 = 16;
			stParm.Parm2 = 16;
			g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));

			return 1;
		} 
		if (idwControlID >= 578481 && idwControlID <= 578491)
		{
			auto TituloInfo2 = static_cast<SText*>(Scene->m_pControlContainer->FindControl(5578482));
			 
			const char* mobTitles[] = {
				"Armia Mobs",       // 1
				"Azran Mobs",       // 2
				"Erion Mobs",       // 3
				"Karden Mobs",      // 4
				"Deserto Mobs",     // 5
				"Submundo Mobs",    // 6
				"Dungeon Mobs",     // 7
				"Kefra Mobs",       // 8
				"Lanhouse's Mobs",  // 9
				"Carta duelo Mobs",  // 10
				"Pista Runas Mobs"  // 10
			};

			int DropTipe = idwControlID - 578481 + 1;
			 
			if (DropTipe >= 1 && DropTipe <= 11)
			{
				TituloInfo2->m_dwAlignType = 3;
				TituloInfo2->SetText((char*)mobTitles[DropTipe - 1], 0);
			}
			 
			ReqDropListZone(DropTipe, 0);

			return true;
		} 
	}
	else
	{
		if (idwControlID == 478480)  
		{
			Scene->m_pMessagePanel->SetMessage((char*)"Filtro salvo!", 4000);
			Scene->m_pMessagePanel->SetVisible(1, 1);
			return 1;
		} 
		    
		if (idwControlID >= 578481 && idwControlID <= 578491) {
			

			Scene->m_pMessagePanel->SetMessage((char*)"ALTERAR PERFIL.", 4000);
		}
		  
	}

	    
	return false;
}

int DropOnClickGrid(int InvSlot, TMEGRIDTYPE GridType, int PanelId)
{
	auto Scene = static_cast<TMFieldScene*>(g_pCurrentScene);
	auto Grid = Scene->m_pDropGrids;

	if (Scene->pDropMode == 1 && PanelId == 478473)//drop
	{ 
		return 0;
	}
	 

	if (GridType == TMEGRIDTYPE::GRID_DROPLIST)
	{  
	}
	return 1;
}

void DropSetCurrentPosSize(float x, float y, float w, float h) 
{
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);
	Scene->m_pSlotEffect->m_pSlot->SetSize(w, h);
	Scene->m_pSlotEffect->m_pSlot->SetPos(x, y);
} 


void ReqDropListZone(int indicex, int indicey)
{
	auto pScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	auto pSearchEdit = (SEditableText*)pScene->m_pControlContainer->FindControl(478491);
	pScene->m_pControlContainer->SetFocusedControl(0);
	pSearchEdit->SetText((char*)"");
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);  

	MSG_STANDARDPARM2 stParm{};
	stParm.Header.ID = Scene->m_pMyHuman->m_dwID;
	stParm.Header.Type = 0xA08;
	stParm.Parm1 = indicex;
	stParm.Parm2 = indicey;
	g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));
}

void ReqDropList()
{
	struct TMP_ReqDrop
	{
		MSG_STANDARD Header;
		short Aba;
		short Page;
	};
	TMP_ReqDrop packet;
	memset(&packet, 0, sizeof(TMP_ReqDrop));

	packet.Header.Size = sizeof(TMP_ReqDrop);
	packet.Header.Type = 0xCB5;
	packet.Header.ID = g_pCurrentScene->m_dwID;
	SendOneMessage((char*)&packet, sizeof(TMP_ReqDrop));
}

#pragma region DROPLIST  

int TMFieldScene::InitializeDroplistSexy()
{  

	auto LabelTituloPanel = (SText*)m_pControlContainer->FindControl(9003);
	LabelTituloPanel->SetText((char*)"Clique sobre o item para retira-lo do filtro", 0);

	m_pDropList = (SPanel*)m_pControlContainer->FindControl(478471);
	if(m_pDropList)
	m_pDropList->m_bVisible = false;
	 
	pDropMode = 1;

	auto TituloInfo = (SText*)m_pControlContainer->FindControl(5578483);
	auto TituloInfo2 = static_cast<SText*>(m_pControlContainer->FindControl(5578482));
	if (TituloInfo2)
	{
		TituloInfo2->SetText((char*)"     Nomes", 0);  // Usa a string formatada para definir o título
	}

	if (TituloInfo)
	{
		TituloInfo->SetText((char*)"Itens encontrados", 0);
	}
	UpdateFilterDrop();

	m_pDropGrids = (SGridControl*)m_pControlContainer->FindControl(478473);
	if (m_pDropGrids)
	{
		m_pDropGrids->m_eGridType =  TMEGRIDTYPE::GRID_DROPLIST;
		m_pDropGrids->SetVisible(0);
	} 

	return 1;
}
 
int TMFieldScene::OnKeySelectMobSexy(char iCharCode, int lParam)
{
	auto ListBox = (SListBox*)m_pControlContainer->FindControl(478472); 

	if (((int)iCharCode != 40) && ((int)iCharCode != 38)) 
		return 0;

	int selected = lParam;
	int total = ListBox->m_nNumItem;

	if ((int)iCharCode == 40)
		selected++;

	if ((int)iCharCode == 38)
		selected--; 

	if (selected < 0 || selected >= total || !total)
	{
		selected = 0;
	} 

	UpdateMobKeyUP(selected);

	return 1;
}

void TMFieldScene::UpdateMobKeyUP(int parm)
{
	auto ListBox = (SListBox*)m_pControlContainer->FindControl(478472);
	auto GridDrop = (SGridControl*)m_pControlContainer->FindControl(478473);
	int page = 0;

	GridDrop->Empty();

	ListBox->m_nSelectedItem = parm;
	int selected = ListBox->m_nSelectedItem;
	int total = ListBox->m_nNumItem;

	if (selected < 0 || selected >= total || !total) return;

	auto item = ListBox->m_pItemList[selected];

	if (!item) return;

	auto lstName = item->GetText();

	auto it = std::find_if(_HudControl.DropListEvent.Drop.begin(), _HudControl.DropListEvent.Drop.end(), [&lstName](stDropList const& c)
		{
			return !strcmp(c.name, lstName);
		});

	if (it == _HudControl.DropListEvent.Drop.end())
		return;

	if (page == 0)
	{
		int lb[] = { 478487, 478489, 478490 };
		char str[80];

		for (int i = 0; i < 3; i++)
		{
			memset(str, 0, sizeof(str));

			if (lb[i] == 478487)
				sprintf(str, "%s", lstName);
			else if (lb[i] == 478489)
				sprintf(str, " %lld", it->gold);
			else if (lb[i] == 478490)
				sprintf(str, " %lld", it->exp);

			auto _lb = (SText*)m_pControlContainer->FindControl(lb[i]);
			if (_lb)
				_lb->SetText(str, 0);
		}
	}

	if (strcmp(_HudControl.DropListEvent.DropSelected.Name, lstName))
	{
		strcpy(_HudControl.DropListEvent.DropSelected.Name, lstName);
		_HudControl.DropListEvent.DropSelected.LastSlot = 0;
		memset(&_HudControl.DropListEvent.DropSelected.Slot, 0, sizeof(_HudControl.DropListEvent.DropSelected.Slot));
	}

	int start = page == 0 ? 0 : _HudControl.DropListEvent.DropSelected.LastSlot + 1;
	_HudControl.DropListEvent.DropPage = page;

	int column = 0;
	int row = 0;
	int slot = 0;

	std::set<int> addedItems;

	for (int i = start; i < 64; i++)
	{
		if (slot == 60)
			return;

		if (it->carry[i].sIndex == 0)
		{
			continue;
		}

		if (addedItems.find(it->carry[i].sIndex) != addedItems.end())
			continue;


		auto ItemDrop = new SGridControlItem(0, &it->carry[i], (float)column, (float)row);

		GridDrop->AddItem(ItemDrop, column, row);
		GridDrop->m_bVisible = 1;
		ItemDrop->m_nHeight = 30.5f * RenderDevice::m_fWidthRatio;
		ItemDrop->m_nWidth = 30.5f * RenderDevice::m_fHeightRatio;

		_HudControl.DropListEvent.DropSelected.LastSlot = i;
		_HudControl.DropListEvent.DropSelected.Slot[slot] = i;

		addedItems.insert(it->carry[i].sIndex);

		slot++;
		column++;

		if (column == 8)
		{
			column = 0;
			row++;
		}
	}

}

void TMFieldScene::DropListUpdate()
{
	auto ListBox = (SListBox*)m_pControlContainer->FindControl(478472);

	if (ListBox)
	{
		ListBox->Empty();
		ListBox->m_bSelectEnable = 1;
		ListBox->m_bSelectEnable = 1;
		 

		for (auto& i : _HudControl.DropListEvent.Drop)
		{ 
			auto labelname = (new SListBoxItem(i.name, 0xFF00FF00, 0.0f, 0.0f, 100.0f, 13.0f * RenderDevice::m_fHeightRatio, 0, 0x22FFFFFF, 1, 0));

			ListBox->m_nHeight = 380.0f * RenderDevice::m_fHeightRatio;
			labelname->m_dwAlignType = 3;
			ListBox->AddItem(labelname);
		}

		if (ListBox->m_nNumItem > 0)
		{
			ListBox->m_nSelectedItem = 0;
			UpdateGridDropList(0);

		}
	}
} 

void TMFieldScene::UpdateGridDropList(int page)
{
	auto ListBox = (SListBox*)m_pControlContainer->FindControl(478472);
	auto GridDrop = (SGridControl*)m_pControlContainer->FindControl(478473);
	GridDrop->Empty();
	int selected = ListBox->m_nSelectedItem;
	int total = ListBox->m_nNumItem;

	if (selected < 0 || selected >= total || !total) return;

	auto item = ListBox->m_pItemList[selected];

	if (!item) return;

	auto lstName = item->GetText();

	auto it = std::find_if(_HudControl.DropListEvent.Drop.begin(), _HudControl.DropListEvent.Drop.end(), [&lstName](stDropList const& c)
		{
			return !strcmp(c.name, lstName);
		});

	if (it == _HudControl.DropListEvent.Drop.end())
		return;

	if (page == 0)
	{
		int lb[] = { 478487, 478489, 478490 };
		char str[80];

		for (int i = 0; i < 3; i++)
		{
			memset(str, 0, sizeof(str));

			if (lb[i] == 478487)
				sprintf(str, "%s", lstName);
			else if (lb[i] == 478489)
				sprintf(str, " %lld", it->gold);
			else if (lb[i] == 478490)
				sprintf(str, " %lld", it->exp);

			auto _lb = (SText*)m_pControlContainer->FindControl(lb[i]);
			if (_lb)
				_lb->SetText(str, 0);
		}
	}

	if (strcmp(_HudControl.DropListEvent.DropSelected.Name, lstName))
	{
		strcpy(_HudControl.DropListEvent.DropSelected.Name, lstName);
		_HudControl.DropListEvent.DropSelected.LastSlot = 0;
		memset(&_HudControl.DropListEvent.DropSelected.Slot, 0, sizeof(_HudControl.DropListEvent.DropSelected.Slot));
	}

	int start = page == 0 ? 0 : _HudControl.DropListEvent.DropSelected.LastSlot + 1;
	_HudControl.DropListEvent.DropPage = page;

	int column = 0;
	int row = 0;
	int slot = 0;
	  
	std::set<int> addedItems;
	 
	for (int i = start; i < 64; i++)
	{
		if (slot == 60)
			return;

		if (it->carry[i].sIndex == 0)
		{ 
			continue;
		}

		if (addedItems.find(it->carry[i].sIndex) != addedItems.end())
			continue;


		auto ItemDrop = new SGridControlItem(0, &it->carry[i], (float)column, (float)row);

		GridDrop->AddItem(ItemDrop, column, row);
		GridDrop->m_bVisible = 1;
		ItemDrop->m_nHeight = 30.5f * RenderDevice::m_fWidthRatio;
		ItemDrop->m_nWidth = 30.5f * RenderDevice::m_fHeightRatio;

		_HudControl.DropListEvent.DropSelected.LastSlot = i;
		_HudControl.DropListEvent.DropSelected.Slot[slot] = i;

		addedItems.insert(it->carry[i].sIndex);

		slot++;
		column++;

		if (column == 8)
		{
			column = 0;
			row++;
		}
	} 
}

int TMFieldScene::OnPacketDropList(MSG_DropList* pStd)
{
	_HudControl.DropListEvent.Drop.clear();
	auto GridDrop = (SGridControl*)m_pControlContainer->FindControl(478473);
	GridDrop->Empty();

	for (int i = 0; i < pStd->amount; i++)
	{
		stDropList Temp;
		memcpy(&Temp, &pStd->Drop[i], sizeof(MSG_DropList::_Drop));
		_HudControl.DropListEvent.Drop.push_back(Temp);
	}

	DropListUpdate();
	return 1;
}
  
int TMFieldScene::SendSearchDropList(int itemType, char* ItemName)
{ 
	pMsgSignalChar req{};
	req.Header.ID = g_pObjectManager->m_dwCharID;
	req.Header.Type = 0xA09; 
	if (strlen(ItemName) < sizeof(req.Nick)) {
		strcpy(req.Nick, ItemName);
	}
	else {
		// Trate o caso de erro aqui, por exemplo, truncando ou retornando uma mensagem de erro.
		strncpy(req.Nick, ItemName, sizeof(req.Nick) - 1);
		req.Nick[sizeof(req.Nick) - 1] = '\0'; // Garante que o último caractere é nulo.
	}

	SendOneMessage((char*)&req, sizeof(req));
	return 1;
}

void TMFieldScene::SearchDropListUpdate()
{
	auto ListBox = (SListBox*)m_pControlContainer->FindControl(478472);

	if (ListBox)
	{
		ListBox->Empty();
		ListBox->m_bSelectEnable = 1;
		ListBox->m_bSelectEnable = 1;


		for (auto& i : _HudControl.DropSearchListEvent.Drop)
		{
			auto labelname = (new SListBoxItem(i.name, 0xFFFFFFFF, 0.0f, 0.0f, 100.0f * RenderDevice::m_fHeightRatio, 12.0f * RenderDevice::m_fWidthRatio, 1, 0x77777777, 0, 1));
			 
			labelname->m_dwAlignType = 0;

			ListBox->AddItem(labelname);
		}

		if (ListBox->m_nNumItem > 0)
		{
			ListBox->m_nSelectedItem = 0;
			 

		}
	}
}
  
#pragma endregion


#pragma region FILTRO


int TMFieldScene::TryDropFilter(MSG_pFilterDrop* pStd)
{
	memcpy(&_HudControl.Object_Human.DropFilterEvent.Items, pStd->Items, sizeof(STRUCT_ITEM) * 24);
	_HudControl.Object_Human.DropFilterEvent.Code = pStd->Code;
	_HudControl.Object_Human.DropFilterEvent.Type = pStd->Type;
	UpdateFilterDrop();

	return 1;
}

void TMFieldScene::DropFilter(int handle)
{
	switch (handle)
	{

	case 9003: //Limpar
	{
		auto Grid = (SGridControl*)m_pControlContainer->FindControl(9002);

		Grid->m_eGridType = TMEGRIDTYPE::GRID_DEFAULT;

		Grid->Empty();


		MSG_pFilterDrop Pack;
		memset(&Pack, 0, sizeof(Pack));

		Pack.Header.Type = 20589;
		Pack.Header.ID = g_pObjectManager->m_dwCharID;
		Pack.Header.Size = sizeof(Pack);
		Pack.Code = 0;

		SendOneMessage((char*)&Pack, sizeof(Pack));

	} break;

	case 9004:
	{
		auto pnl = (SPanel*)m_pControlContainer->FindControl(9000);

		if (pnl)
			pnl->m_bVisible = false;

	}break;



	}
}

void TMFieldScene::UpdateFilterDrop()
{
	auto Drop = _HudControl.Object_Human.DropFilterEvent;
	auto pnl = (SPanel*)m_pControlContainer->FindControl(9000);
	auto Grid = (SGridControl*)m_pControlContainer->FindControl(9002);
	auto pnlDropMonster = (SPanel*)m_pControlContainer->FindControl(478471);
	int currentColumn = 0;
	int currentRow = 0;

	if (pnl)
	{
		auto res = RenderDevice::m_fResBase;
		
		pnl->m_bVisible = true;
		if (pDropMode == 1)
		{
			if (res <= 2)
			{ 
				pnl->SetPos(395.0f * RenderDevice::m_fHeightRatio, 353.0f * RenderDevice::m_fWidthRatio);
				pnl->SetSize(245.0f * RenderDevice::m_fHeightRatio, 130.0f * RenderDevice::m_fWidthRatio);
			}
			if (res == 3)
			{
				pnl->SetPos(395.0f * RenderDevice::m_fHeightRatio, 383.0f * RenderDevice::m_fWidthRatio);
				pnl->SetSize(245.0f * RenderDevice::m_fHeightRatio, 130.0f * RenderDevice::m_fWidthRatio);
			}
			if (res == 4)
			{
				pnl->SetPos(395.0f * RenderDevice::m_fHeightRatio, 355.0f * RenderDevice::m_fWidthRatio);
				pnl->SetSize(245.0f * RenderDevice::m_fHeightRatio, 130.0f * RenderDevice::m_fWidthRatio);
			}
			if (res == 5)
			{
				pnl->SetPos(395.0f * RenderDevice::m_fHeightRatio, 360.0f * RenderDevice::m_fWidthRatio);
				pnl->SetSize(245.0f * RenderDevice::m_fHeightRatio, 130.0f * RenderDevice::m_fWidthRatio);
			}

			auto GridFiltro = (SGridControl*)m_pControlContainer->FindControl(9002);
			GridFiltro->SetPos(16.0f * RenderDevice::m_fHeightRatio, 31.0f * RenderDevice::m_fWidthRatio);
			GridFiltro->SetSize(295.0f * RenderDevice::m_fHeightRatio, 75.0f * RenderDevice::m_fWidthRatio);
			GridFiltro->m_nWidth = 215.0f * RenderDevice::m_fHeightRatio;

		}
		if (pDropMode == 2)
		{ 

			pnl->SetPos(295.0f * RenderDevice::m_fHeightRatio, 120.0f * RenderDevice::m_fWidthRatio);//alterado
			pnl->SetSize(335.0f * RenderDevice::m_fHeightRatio, 190.0f * RenderDevice::m_fWidthRatio);

			auto GridFiltro = (SGridControl*)m_pControlContainer->FindControl(9002);
			GridFiltro->SetPos(24.0f * RenderDevice::m_fHeightRatio, 50.0f * RenderDevice::m_fWidthRatio);
			GridFiltro->SetSize(295.0f * RenderDevice::m_fHeightRatio, 111.0f * RenderDevice::m_fWidthRatio);

		}
		Grid->Empty();
		Grid->m_eGridType = TMEGRIDTYPE::GRID_DROPLIST;

		for (int y = 0; y < 24; y++)
		{
			if (Drop.Items[y].sIndex == 0) continue;

			auto pREQItem = new STRUCT_ITEM;
			memset(pREQItem, 0, sizeof(STRUCT_ITEM));
			pREQItem->sIndex = _HudControl.Object_Human.DropFilterEvent.Items[y].sIndex;

			auto pItem = new SGridControlItem(0, pREQItem, 0.0f, 0.0f);
			if (pDropMode == 1)
			{
				pItem->m_nHeight = 23.0f * RenderDevice::m_fHeightRatio;
				pItem->m_nWidth = 23.0f * RenderDevice::m_fWidthRatio;
				 

			}
			if (pDropMode == 2)
			{
				pItem->m_nHeight = 29.0f * RenderDevice::m_fHeightRatio;
				pItem->m_nWidth = 29.0f * RenderDevice::m_fWidthRatio;
				 
				 
			}

			Grid->AddItem(pItem, currentColumn, currentRow);
			

			currentColumn++;

			if (currentColumn == 8)
			{
				currentColumn = 0;
				currentRow++;
			}
		}
	}
}


#pragma endregion
     
 


 