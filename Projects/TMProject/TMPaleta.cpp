#include "pch.h"
#include "TMPaleta.h"
#include "TMFieldScene.h"
#include "TMGlobal.h"
#include "SControlContainer.h"

#include "SControl.h"
#include "TMUtil.h"
#include "PremiumStore.h"
#include "TMDroplistAndFilter.h"
 
int PatternOnPacketEvent(ESCENE_TYPE scene, MSG_STANDARD* header) // ok
{

	g_pApp->InitializeSceneResize(m_pConfig.Config[0]);
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);
	auto PacketId = header->Type;
	 
	if (scene == ESCENE_TYPE::ESCENE_SELECT_SERVER)
	{
	//	 printf("\n ESCENE_SELECT_SERVER::Paleta pacote 0x%X", PacketId);
	}
	else if (scene == ESCENE_TYPE::ESCENE_SELCHAR)
	{
	//	 printf("\n ESCENE_SELCHAR::Paleta pacote 0x%X", PacketId);
	}
	else if (scene == ESCENE_TYPE::ESCENE_FIELD)
	{
		if (PremiumStoreOnPacketEvent(header) == 1)
			return true;

		if (DropOnPacketEvent(header) == 1)
			return true;

		if (PacketId == 0xF30)
		{
			MSG_AccountLockInfo* packet = (MSG_AccountLockInfo*)header;

			Scene->m_pLockAcctPanel->SetVisible(0);
			 _HudControl.Lock.Status = false;

			for (int i = 0; i < 8; i++)
				_HudControl.Lock.Info[i] = packet->Conf[i];

			for (int i = 0; i < 8; i++)
				if (_HudControl.Lock.Info[i] == true)
				{
					_HudControl.Lock.Status = true;
					break;
				}

			for (int i = 0; i < 8; i++)
			{
				if (_HudControl.Lock.Info[i] == true)
				{
					Scene->m_pLockAcctBtn[i]->m_GCPanel.nTextureSetIndex = 806 ;
					Scene->m_pLockAcctDesc[i]->SetTextColor(0xFFCCBCAA);
				}
				else
				{
					Scene->m_pLockAcctBtn[i]->m_GCPanel.nTextureSetIndex = 807;
					Scene->m_pLockAcctDesc[i]->SetTextColor(0xFF964A35);
				}
			}


			if (_HudControl.Lock.Status == true)
				Scene->m_pLockAcctOpenBtn->m_GCPanel.nTextureSetIndex = 806;
			else
				Scene->m_pLockAcctOpenBtn->m_GCPanel.nTextureSetIndex = 807;

			Scene->m_pLockAcctPw->SetText((char*)"");
			std::cout << "\n _HudControl.Lock.Status " << _HudControl.Lock.Status;
			return true;
		}
		// printf("\n ESCENE_FIELD::Paleta pacote 0x%X", PacketId);
	}

	return 0;
}

int PatternOnControlEvent(ESCENE_TYPE scene, unsigned int idwControlID, unsigned int idwEvent) // ok
{
	TMFieldScene* pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	TMScene* pCurrentNode = g_pCurrentScene; 
	  
	if (scene == ESCENE_TYPE::ESCENE_SELECT_SERVER)
	{ 
		//printf("\n ESCENE_SELECT_SERVER::Paleta BTN 0x%d", idwControlID);

	}
	else if (scene == ESCENE_TYPE::ESCENE_SELCHAR)
	{
		// printf("\n ESCENE_SELCHAR::Paleta BTN 0x%d", idwControlID);
	}
	else if (scene == ESCENE_TYPE::ESCENE_FIELD)
	{
		//printf("\n ESCENE_FIELD::Paleta BTN 0x%d", idwControlID);

		if (PremiumStoreOnControlEvent(idwControlID, idwEvent) == 1)
			return true;

		if (DropOnControlEvent(idwControlID, idwEvent) == 1)
			return true;

		if (pFScene->m_RouletteSystem.OnRouletteControlEvent(idwControlID, idwEvent) == 1)
			return true;

		if (idwControlID >= 78083 && idwControlID <= 78090)
		{
			if (_HudControl.Lock.Status == 1)
			{
				pFScene->m_pMessagePanel->SetMessage("Digite a senha configurada para desbloquear", 3000);
				pFScene->m_pMessagePanel->SetVisible(1, 1);
				return true;
			}
			else
			{

				if (pFScene->m_pLockAcctBtn[idwControlID - 78083]->m_GCPanel.nTextureSetIndex == 806)
				{
					pFScene->m_pLockAcctBtn[idwControlID - 78083]->m_GCPanel.nTextureSetIndex = 807;
					pFScene->m_pLockAcctDesc[idwControlID - 78083]->SetTextColor(0xFFCCBCAA);
				}
				else
				{
					pFScene->m_pLockAcctBtn[idwControlID - 78083]->m_GCPanel.nTextureSetIndex = 806;
					pFScene->m_pLockAcctDesc[idwControlID - 78083]->SetTextColor(0xFF964A35);
				}

				return true;
			}
		}
		if (idwControlID == 78091)
		{
			if (pFScene->m_pLockAcctPw->m_strText[0] == 0)
				return true;

			auto length = strlen(pFScene->m_pLockAcctPw->m_strText);

			std::cout << "\n pFScene->m_pLockAcctPw->m_strText " << pFScene->m_pLockAcctPw->m_strText;

			if (length != 8)
			{
				pFScene->m_pMessagePanel->SetMessage("Seu código deve conter 8 caracteres", 3000);
				pFScene->m_pMessagePanel->SetVisible(1, 1);
				return true;
			}
			else
			{
				if (_HudControl.Lock.Status == 1)
				{
					MSG_UnlockAccount packet;
					memset(&packet, 0, sizeof packet);
					packet.Header.Size = sizeof packet;
					packet.Header.Type = 0xF30;
					packet.Header.ID = pFScene->m_dwID;
					strncpy(packet.Password, pFScene->m_pLockAcctPw->m_strText, 8); 
					SendOneMessage((char*)&packet, sizeof packet);
				}
				else
				{
					bool CanLock = false;
					for (int i = 0; i < 8; i++)
					{
						if (pFScene->m_pLockAcctBtn[i]->m_GCPanel.nTextureSetIndex == 806)
						{
							CanLock = true;
							break;
						}
					}

					if (CanLock == false)
					{
						pFScene->m_pMessagePanel->SetMessage("Selecione ao menos uma função a ser bloqueada", 3000);
						pFScene->m_pMessagePanel->SetVisible(1, 1);
						return true;
					}
					else
					{
						MSG_LockAccount packet;
						memset(&packet, 0, sizeof packet);
						packet.Header.Size = sizeof packet;
						packet.Header.Type = 0xF31;
						packet.Header.ID = pFScene->m_dwID;
						strncpy(packet.Password, pFScene->m_pLockAcctPw->m_strText, 8);
						packet.Password[8] = 0;
						for (int i = 0; i < 8; i++)
						{
							if (pFScene->m_pLockAcctBtn[i]->m_GCPanel.nTextureSetIndex == 806)
								packet.Conf[i] = 1;
						}
						SendOneMessage((char*)&packet, sizeof packet);
					}
					pFScene->m_pControlContainer->SetFocusedControl(0);
				}

				return true;
			}
		}
		if (idwControlID == 44001)
		{
			if (idwEvent == 0)
				pFScene->m_pLockAcctPanel->SetVisible(1);

			pFScene->m_pLockAcctPw->SetText((char*)"");

			if (_HudControl.Lock.Status == 1) // Está bloqueado
			{
				pFScene->m_pLockAcctTitle->SetText(g_UIString[526], 0);
				pFScene->m_pLockAcctText1->SetText(g_UIString[527], 0);
				pFScene->m_pLockAcctText2->SetText(g_UIString[528], 0);
			}
			else
			{
				pFScene->m_pLockAcctTitle->SetText(g_UIString[281], 0);
				pFScene->m_pLockAcctText1->SetText(g_UIString[524], 0);
				pFScene->m_pLockAcctText2->SetText(g_UIString[525], 0);
			}

			for (int i = 0; i < 8; i++)
			{
				if (_HudControl.Lock.Info[i] == true)
				{
					pFScene->m_pLockAcctBtn[i]->m_GCPanel.nTextureSetIndex = 806;
					pFScene->m_pLockAcctDesc[i]->SetTextColor(0xFFCCBCAA);
				}
				else
				{
					pFScene->m_pLockAcctBtn[i]->m_GCPanel.nTextureSetIndex = 807;
					pFScene->m_pLockAcctDesc[i]->SetTextColor(0xFF964A35);
				}
			}

			if (_HudControl.Lock.Status == true)
				pFScene->m_pLockAcctOpenBtn->m_GCPanel.nTextureSetIndex = 806;
			else
				pFScene->m_pLockAcctOpenBtn->m_GCPanel.nTextureSetIndex = 807;
			return true;
		}
	}

	return 0;
}

int PatternOnClickGrid(ESCENE_TYPE scene, int PanelId, int CellX, int CellY, TMEGRIDTYPE GridType, int ControlId)
{
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);
	auto pMobData = &g_pObjectManager->m_stMobData;
	 

	auto InvSlot = CellX + (CellY * 5);
	InvSlot += (PanelId - 67072) * 15;

	if (scene == ESCENE_TYPE::ESCENE_SELECT_SERVER)
	{ 
	//	printf("\n ESCENE_SELECT_SERVER::Paleta Grid %d ControlId %d", PanelId, ControlId);
	}
	else if (scene == ESCENE_TYPE::ESCENE_SELCHAR)
	{ 
	//	printf("\n ESCENE_SELCHAR::Paleta Grid %d ControlId %d", PanelId, ControlId);
	}
	else if (scene == ESCENE_TYPE::ESCENE_FIELD)
	{
		if (PremiumStoreOnClickGrid(InvSlot, GridType, ControlId) == 1)
			return true;   

		//printf("\n ESCENE_FIELD::Paleta Grid %d ControlId %d GridType %d", PanelId, ControlId, (int)GridType);

	 
	}

	return 0;
}

int PatternOnCharEvent(ESCENE_TYPE scene, char iCharCode, int lParam) // ok
{
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);

	if (scene == ESCENE_TYPE::ESCENE_SELECT_SERVER)
	{
	//	 printf("\n ESCENE_FIELD::Paleta Char %d ControlId %d", iCharCode, lParam);
		return 1;
	}
	else if (scene == ESCENE_TYPE::ESCENE_SELCHAR)
	{
	//	 printf("\n ESCENE_FIELD::Paleta Panel %d ControlId %d", iCharCode, lParam);
		return 1;
	}
	else if (scene == ESCENE_TYPE::ESCENE_FIELD)
	{
	 //	 printf("\n ESCENE_FIELD::CharEvent Panel %d ControlId %d", iCharCode, lParam);
		 return 1;
	}

	return 0;
}


