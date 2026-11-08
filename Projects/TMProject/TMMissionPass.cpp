#include "pch.h"
#include "TMPaleta.h"
#include "TMFieldScene.h"
#include "TMGlobal.h"
#include "SControlContainer.h"

#include "SControl.h"
#include "TMUtil.h"
#include "PremiumStore.h"
#include "TMDroplistAndFilter.h"

int TMFieldScene::OnPacketUpdateMissionPass(pMsgSignalChar2* pStd)
{
	snprintf(_HudControl.LastMission,sizeof(_HudControl.LastMission), pStd->LastMission);
	snprintf(_HudControl.CurrentMission, sizeof(_HudControl.CurrentMission), pStd->CurrentMission); 
	return 1;
}

//aqui pacote de missioes
int TMFieldScene::OnPacketMissionInfo(MSG_STANDARD* pStd)
{

	// Casting o pacote para o tipo MSG_MISSIONINFO 


	auto pPacket = (MSG_MISSIONINFO*)pStd;

	memcpy(&_HudControl.gMissionPass, pPacket, sizeof MSG_MISSIONINFO);

	// Log básico sobre a missão
	std::cout << "=== Missão Recebida ===" << std::endl;
	std::cout << "Nome da Missão: " << pPacket->Name << std::endl;
	std::cout << "Missão Aceita: " << (pPacket->IsAccepted ? "Sim" : "Não") << std::endl;
	//pPacket->Name
	auto missionName = (SText*)m_pControlContainer->FindControl(177553);

	auto ListBox = (SListBox*)m_pControlContainer->FindControl(177564);
	auto ListBox2 = (SListBox*)m_pControlContainer->FindControl(177565);
	
	if (ListBox)
	{
		ListBox->Empty();
		ListBox->m_bSelectEnable = 0;
		char strSexy[128] = {};
		char strSexy2[1028] = {};

		snprintf(strSexy2, sizeof(strSexy2), "Missão aceita: %s | Missão encerrar: %s", _HudControl.LastMission, _HudControl.CurrentMission);

		// Adiciona a mensagem informativa no topo do ListBox
		auto infoLabel = (new SListBoxItem("", 0xFFFFFFFF, 0.0f, 0.0f, 100.0f * RenderDevice::m_fHeightRatio, 12.0f * RenderDevice::m_fWidthRatio, 1, 0x77777777, 0, 1));
		infoLabel->m_dwAlignType = 0;
		infoLabel->SetText((char*)strSexy2, 0);
		infoLabel->SetTextColor(0xFFAAFFAA);  // Um vermelho menos intenso


		ListBox->AddItem(infoLabel);  // Adiciona a mensagem na primeira linha

		// Informações de mobs da missão
		std::cout << "--- Mobs da Missão ---" << std::endl;
		for (int i = 0; i < 5; i++)
		{
			// Verifica se o mob tem informações válidas
			if (pPacket->Mob[i].Killed == 0 && pPacket->Mob[i].Total == 0 && strlen(pPacket->Mob[i].Name) == 0)
			{
				continue; // Pula a criação do item se o mob for inválido
			}

			auto labelname = (new SListBoxItem("", 0xFFFFFFFF, 0.0f, 0.0f, 100.0f * RenderDevice::m_fHeightRatio, 12.0f * RenderDevice::m_fWidthRatio, 1, 0x77777777, 0, 1));
			labelname->m_dwAlignType = 0;
			ListBox->AddItem(labelname);

			// Ajusta o formato para 2 dígitos nos números e remove espaços extras
			snprintf(strSexy, sizeof(strSexy), "%02d/%02d %s", pPacket->Mob[i].Killed, pPacket->Mob[i].Total, pPacket->Mob[i].Name);
			labelname->SetText((char*)strSexy, 0);
			labelname->SetTextColor(0xFFFFFFFF);
		}

		if (ListBox->m_nNumItem > 0)
		{
			ListBox->m_nSelectedItem = 0;
		}
	}

	if (ListBox2)
	{
		ListBox2->Empty();
		ListBox2->m_bSelectEnable = 1;

		char strSexy[128] = {};
		char strSexy2[1028] = {};

		snprintf(strSexy2, sizeof(strSexy2), "Missão aceita: %s | Missão encerrar: %s", _HudControl.LastMission, _HudControl.CurrentMission);

		// Adiciona a mensagem informativa no topo do ListBox
		auto infoLabel = (new SListBoxItem("", 0xFFFFFFFF, 0.0f, 0.0f, 100.0f * RenderDevice::m_fHeightRatio, 12.0f * RenderDevice::m_fWidthRatio, 1, 0, 0, 1));
		infoLabel->m_dwAlignType = 0;
		infoLabel->SetText((char*)strSexy2, 0);
		infoLabel->SetTextColor(0xFFAAFFAA);  // Um vermelho menos intenso


		ListBox2->AddItem(infoLabel);  // Adiciona a mensagem na primeira linha

		// Informações de mobs da missão
		std::cout << "--- Mobs da Missão ---" << std::endl;
		for (int i = 0; i < 5; i++)
		{
			// Verifica se o mob tem informações válidas
			if (pPacket->Mob[i].Killed == 0 && pPacket->Mob[i].Total == 0 && strlen(pPacket->Mob[i].Name) == 0)
			{
				continue; // Pula a criação do item se o mob for inválido
			}

			auto labelname = (new SListBoxItem("", 0xFFFFFFFF, 0.0f, 0.0f, 100.0f * RenderDevice::m_fHeightRatio, 12.0f * RenderDevice::m_fWidthRatio, 1, 0x77777777, 0, 1));
			labelname->m_dwAlignType = 0;
			ListBox2->AddItem(labelname);

			// Ajusta o formato para 2 dígitos nos números e remove espaços extras
			snprintf(strSexy, sizeof(strSexy), "%02d/%02d %s", pPacket->Mob[i].Killed, pPacket->Mob[i].Total, pPacket->Mob[i].Name);
			labelname->SetText((char*)strSexy, 0);
			labelname->SetTextColor(0xFFFFFFFF);
		}

		if (ListBox2->m_nNumItem > 0)
		{
			ListBox2->m_nSelectedItem = 0;
		}
	}

	if (missionName)
	{
		missionName->SetText((char*)pPacket->Name, 0);
	}

	auto SlotPassFree = (SGridControl*)m_pControlContainer->FindControl(177558);


	SlotPassFree->Empty();

	for (int i = 0; i < 6; ++i)
	{
		auto pItemList = new STRUCT_ITEM;
		memcpy(pItemList, &pPacket->FreeReward[i], sizeof(STRUCT_ITEM));

		if (pPacket->FreeReward[i].sIndex <= 0)
		{
			delete pItemList;
			continue;
		}

		auto pItem = new SGridControlItem(0, pItemList, 0.0f, 0.0f);

		SlotPassFree->AddItem(pItem, i % 2, i / 2);
		int nAmount = BASE_GetItemAmount(pItemList);
		if (nAmount > 0)
		{
			sprintf(pItem->m_GCText.strString, "%2d", nAmount);
			pItem->m_GCText.pFont->SetText(pItem->m_GCText.strString, pItem->m_GCText.dwColor, 0);
		}
		SlotPassFree->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}


	auto SlotPassPw2 = (SGridControl*)m_pControlContainer->FindControl(177559);


	SlotPassPw2->Empty();

	for (int i = 0; i < 6; ++i)
	{
		auto pItemList = new STRUCT_ITEM;
		memcpy(pItemList, &pPacket->BattlePassReward[i], sizeof(STRUCT_ITEM));

		if (pPacket->BattlePassReward[i].sIndex <= 0)
		{
			delete pItemList;
			continue;
		}

		auto pItem = new SGridControlItem(0, pItemList, 0.0f, 0.0f);

		SlotPassPw2->AddItem(pItem, i % 2, i / 2);
		int nAmount = BASE_GetItemAmount(pItemList);
		if (nAmount > 0)
		{
			sprintf(pItem->m_GCText.strString, "%2d", nAmount);
			pItem->m_GCText.pFont->SetText(pItem->m_GCText.strString, pItem->m_GCText.dwColor, 0);
		}
		SlotPassPw2->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	// Para Exp
	std::stringstream ssExp;
	ssExp.imbue(std::locale(""));  // Configura para usar separador de milhares
	ssExp << std::fixed << pPacket->Exp;  // Adiciona o valor de Exp formatado
	std::string formattedExp = ssExp.str();  // Converte o stream formatado para string

	// Para Gold
	std::stringstream ssGold;
	ssGold.imbue(std::locale(""));  // Configura para usar separador de milhares
	ssGold << std::fixed << pPacket->Gold;  // Adiciona o valor de Gold formatado
	std::string formattedGold = ssGold.str();  // Converte o stream formatado para string

	// Definir o texto com o valor formatado
	m_pMissionExp->SetText((char*)formattedExp.c_str(), 0);
	m_pMissionGold->SetText((char*)formattedGold.c_str(), 0);

	 
	return 0;
}

