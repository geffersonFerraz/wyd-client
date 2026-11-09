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
#include <wininet.h>
#include <string>  
#include <ctime>
#include <regex> //require c++11
#include "PremiumStore.h"

unsigned int g_dwStartBuyWinTime;
stPremiumData g_PremiumData;

int PremiumStoreOnPacketEvent(MSG_STANDARD* header)
{
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);

	auto PacketId = header->Type;
	 
	auto labelcash1 = (SText*)Scene->m_pControlContainer->FindControl(1048982);
	auto labelcash = (SText*)Scene->m_pControlContainer->FindControl(6000069);
	auto Price = -1;
	if (PacketId == 0x3B1) // Att Cash
	{
		MSG_STANDARDPARM* packet = (MSG_STANDARDPARM*)header;
		char szRMoen[128]{};
		_HudControl.Personagem.Cash = (int)packet->Parm;
		snprintf(szRMoen, sizeof szRMoen, "%d", _HudControl.Personagem.Cash);

		Scene->m_pPremiumStoreCash->SetText(szRMoen, 0);
		labelcash->SetText(szRMoen, 0);
		snprintf(szRMoen, sizeof szRMoen, "Cash:%d", _HudControl.Personagem.Cash);
		labelcash1->SetText(szRMoen, 0);
		
		return true;
	}

	else if (PacketId == CashShopInfoPacket)
	{
		MSG_CASHSHOPINFO* packet = (MSG_CASHSHOPINFO*)header;

		Scene->m_pPremiumStoreBuyWin->SetVisible(0);


		/*for (int i = 0; i < 7; i++)
		{
			Scene->m_pPremiumStoreAbaBtn[i]->m_pAltText->SetText((char*)"", 0);
			if (i != packet->Aba)
				Scene->m_pPremiumStoreAbaBtn[i]->m_GCPanel.nTextureSetIndex = 631;
			else
				Scene->m_pPremiumStoreAbaBtn[i]->m_GCPanel.nTextureSetIndex = 632;
		}*/

		 Scene->m_pPremiumStoreCurPage->SetText(strfmt("%d", packet->Page + 1), 0);



		for (size_t i = 0; i < 65; i++)
		{
			memset(&g_PremiumData.FreeReward[i], 0x0, sizeof g_PremiumData.FreeReward[i]);
			g_PremiumData.price[i] = packet->price[i];
			Price = packet->price[i];
			g_PremiumData.amount[i] = packet->amount[i];

		}

		for (size_t i = 0; i < 32; i++)
		{
			Scene->m_pPremiumStoreSlot[i]->Empty();
			Scene->m_pPremiumStoreSlot[i]->m_eGridType = TMEGRIDTYPE::GRID_PREMIUMSTORE;
			memcpy(&g_PremiumData.FreeReward[i], &packet->FreeReward[i], sizeof(STRUCT_ITEM));

			if (g_PremiumData.FreeReward[i].sIndex != 0)
				Scene->m_pPremiumStoreSlot[i]->SetItemOnGrid(&g_PremiumData.FreeReward[i], 0, 0);
			Scene->m_pPremiumStorePrice[i]->SetText(strfmt("$:%d", g_PremiumData.price[i]), 0);
			Scene->m_pPremiumStorePrice[i]->SetTextColor(0xFFFFFFFF);
			 
			Scene->m_pPremiumStorePrice[i]->SetVisible(1);

		}
		//Scene->m_pPremiumStorePanel->SetVisible(1);
		return true;
	}

	return false;
}

int PremiumStoreOnControlEvent(unsigned int idwControlID, unsigned int idwEvent)
{
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);


	if (idwControlID == 657888)
	{
		Scene->m_pPremiumStorePanel->SetVisible(1);
		Scene->m_pControlContainer->SetFocusedControl(Scene->m_pPremiumStorePanel);
		//char szRMoeny2[64]{};
		//sprintf(szRMoeny2, "%d", _HudControl.Personagem.Cash);// 
		//Scene->m_pPremiumStoreCash->SetText(szRMoeny2,0);
		ReqPremiumStore();
		return true;
	}
	else if (idwControlID == 78187) // Fechar
	{
		ReqPremiumStore();
		Scene->m_pPremiumStorePanel->SetVisible(0);
		Scene->m_pControlContainer->SetFocusedControl(0);
		return true;
	}
	else if (idwControlID >= 78042 && idwControlID <= 78049)
	{
		g_PremiumData.Aba = idwControlID - 78042;
		g_PremiumData.Page = 0;
		ReqPremiumStore();

		return true;
	}
	else if (idwControlID == 78052)
	{
		if (g_PremiumData.Page > 0)
		{
			g_PremiumData.Page--;

			if (g_PremiumData.Page > 3)
				g_PremiumData.Page = 0;

			if (g_PremiumData.Page < 0)
				g_PremiumData.Page = 0;
			ReqPremiumStore();
		}

		return true;
	}
	else if (idwControlID == 78053)
	{
		if (g_PremiumData.Page <= 3)
		{
			g_PremiumData.Page++;

			if (g_PremiumData.Page > 3)
				g_PremiumData.Page = 0;

			if (g_PremiumData.Page < 0)
				g_PremiumData.Page = 0;

			ReqPremiumStore();
		}

		return true;
	}
	else if (idwControlID == 82358)//aqui compra item
	{
		auto Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(82358));
		auto pFocused = Button->IsFocused();

		unsigned int dwServerTime = g_pTimerManager->GetServerTime();

		if (dwServerTime < g_dwStartBuyWinTime + 1000)
		{
			/*Scene->m_pMessagePanel->SetMessage("Clicou muito rapido...tente novamente!", 2000);
			Scene->m_pMessagePanel->SetVisible(1, 1);*/
			return false;
		}
		MSG_BuyPremiumStore packet;
		memset(&packet, 0, sizeof(MSG_BuyPremiumStore));
		packet.Header.Size = sizeof(MSG_BuyPremiumStore);
		packet.Header.Type = 0xF03;
		packet.Header.ID = Scene->m_dwID;
		packet.Aba = g_PremiumData.Aba;
		packet.Page = g_PremiumData.Page;
		packet.SlotItem = g_PremiumData.FreeReward[g_PremiumData.SelectedSlot];
		packet.Slot = g_PremiumData.SelectedSlot;
		packet.Price = g_PremiumData.price[g_PremiumData.SelectedSlot];
		packet.Quant = g_PremiumData.BuyQuant;

		SendOneMessage((char*)&packet, sizeof(MSG_BuyPremiumStore));
		PremiumStoreOnControlEvent(82359, 0);
		return true;
	}
	else if (idwControlID == 82359)
	{
		auto Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(82358));
		auto pFocused = Button->IsFocused();

		unsigned int dwServerTime = g_pTimerManager->GetServerTime();

		if (pFocused == 1)
			Button->m_bFocused = 0;

		if (dwServerTime < g_dwStartBuyWinTime + 1000 && pFocused == 0)
		{  //Evitar de voltar para a tela de itens, pois o grid do item esta na frente do botao de cancelar, 
			//entao se for mais rapido que 1sg vai anular a ação. :) 
			return false;
		}

		Scene->m_pPremiumStoreBuyWin->SetVisible(0);
		Scene->m_pPremiumStorePanel->SetVisible(1);
		return true;
	}
	else if (idwControlID == 82360)
	{

		if (g_PremiumData.BuyQuant < 10)
		{
			g_PremiumData.BuyQuant++;

			Scene->m_pPremiumStoreBuyQuant->SetText(strfmt("%d", g_PremiumData.BuyQuant), 0);

			auto Price = g_PremiumData.price[g_PremiumData.SelectedSlot];

			Scene->m_pPremiumStoreBuyPrice->SetText(strfmt("$:%d", Price * g_PremiumData.BuyQuant), 0);
			Scene->m_pPremiumStoreBuyPrice->SetTextColor(0XFFFFFFFF);
		}

		return true;
	}
	else if (idwControlID == 82361)
	{
		if (g_PremiumData.BuyQuant > 1)
		{
			g_PremiumData.BuyQuant--;

			Scene->m_pPremiumStoreBuyQuant->SetText(strfmt("%d", g_PremiumData.BuyQuant), 0);

			auto Price = g_PremiumData.price[g_PremiumData.SelectedSlot];

			Scene->m_pPremiumStoreBuyPrice->SetText(strfmt("$:%d", Price * g_PremiumData.BuyQuant), 0);
			Scene->m_pPremiumStoreBuyPrice->SetTextColor(0XFFFFFFFF);

			/*if (g_PremiumData.Slots[g_PremiumData.SelectedSlot].PriceType == 1)
			{
				Scene->m_pPremiumStoreBuyPrice->SetText(strfmt("%d VC", Price * g_PremiumData.BuyQuant), 0);
				Scene->m_pPremiumStoreBuyPrice->SetTextColor(Neve);
			}
			else if (g_PremiumData.Slots[g_PremiumData.SelectedSlot].PriceType == 2)
			{
				Scene->m_pPremiumStoreBuyPrice->SetText(strfmt("%d [C]", Price * g_PremiumData.BuyQuant), 0);
				Scene->m_pPremiumStoreBuyPrice->SetTextColor(Amarelo);
			}*/
		}

		return true;
	}

	return false;
}

int PremiumStoreOnClickGrid(int InvSlot, TMEGRIDTYPE GridType, int PanelId)
{

	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);

	if (GridType == TMEGRIDTYPE::GRID_PREMIUMSTORE)
	{
		if (Scene->m_pPremiumStorePanel->IsVisible() == 1)
		{
			auto SelectedSlot = PanelId - 78001;


			Scene->m_pDescPanel->SetVisible(0);
			Scene->m_pPremiumStoreBuyWin->SetVisible(1);
			Scene->m_pPremiumStorePanel->SetVisible(0);

			g_dwStartBuyWinTime = g_pTimerManager->GetServerTime();
			g_PremiumData.SelectedSlot = SelectedSlot;
			g_PremiumData.BuyQuant = 1;

			Scene->m_pPremiumStoreBuyItem->SetItemOnGrid(&g_PremiumData.FreeReward[g_PremiumData.SelectedSlot], 0, 0);

			Scene->m_pPremiumStoreBuyItemName->SetText(g_pItemList[g_PremiumData.FreeReward[g_PremiumData.SelectedSlot].sIndex].Name, 0);

			Scene->m_pPremiumStoreBuyPrice->SetText(Scene->m_pPremiumStorePrice[SelectedSlot]->GetText(), 0);

			Scene->m_pPremiumStoreBuyQuant->SetText((char*)"1", 0);
			g_PremiumData.BuyItem = g_PremiumData.FreeReward[g_PremiumData.SelectedSlot].sIndex;
		}
		else
		{
			for (int i = 0; i < 4; i++)
				Scene->m_pGridInvList[i]->m_eGridType = TMEGRIDTYPE::GRID_DEFAULT;

			Scene->SetEquipGridState(1);
		}

	}

	return false;
}

void ReqPremiumStore()
{
	struct TMP_ReqPremiumStore
	{
		MSG_STANDARD Header;
		short Aba;
		short Page;
	};
	TMP_ReqPremiumStore packet;
	memset(&packet, 0, sizeof(TMP_ReqPremiumStore));

	packet.Header.Size = sizeof(TMP_ReqPremiumStore);
	packet.Header.Type = reqCashShop;
	packet.Header.ID = g_pCurrentScene->m_dwID;
	packet.Aba = g_PremiumData.Aba;
	packet.Page = g_PremiumData.Page;
	SendOneMessage((char*)&packet, sizeof(TMP_ReqPremiumStore));
} 


void SGridControl::SetStoreQuantities(SGridControlItem* grid)
{
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

	unsigned int controlId = grid->m_dwControlID;
	if (controlId >= 78001 && controlId <= 78020)
	{
		int quantity = 0;// g_PremiumData.Quant;
		/*int quantity = g_PremiumData.Slots[controlId - 78001].Quant;

		if (quantity == -1)
		{
			pFScene->m_pDescQuantityLine->SetText((char*)"Unidades ilimitadas", 0);
			pFScene->m_pDescQuantityLine->SetTextColor(0xFFFFB21C);
		}
		else if (quantity == 0)
		{
			pFScene->m_pDescQuantityLine->SetText((char*)"Unidades esgotadas", 0);
			pFScene->m_pDescQuantityLine->SetTextColor(0xFFFF6347);
		}
		else
		{
			pFScene->m_pDescQuantityLine->SetText(strfmt("Unidades disponíveis: %d", quantity), 0);
			pFScene->m_pDescQuantityLine->SetTextColor(0xFFFFFF00);
		}*/

		pFScene->m_pDescQuantityLine->SetText(strfmt("Unidades disponíveis: %d", quantity), 0);
		pFScene->m_pDescQuantityLine->SetTextColor(0xFFFFFF00);
		pFScene->m_pDescQuantityLine->SetVisible(1);
	}
	 
	else
	{
		pFScene->m_pDescQuantityLine->SetVisible(0);
	}
}