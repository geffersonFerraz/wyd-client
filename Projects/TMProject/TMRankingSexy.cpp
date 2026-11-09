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
#include "MacroMsg.h"

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
 
int PacketSend = 0x528;
int TMFieldScene::InitializeRankingSexy()
{

	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	  
	m_pRankSexy = new CapeRankingSexy("", 0, 0, 0, 0);
	if (m_pRankSexy)
	{
		m_pRankSexy->m_bSelectEnable = 0;
		m_pRankSexy->SetControlID(8500001); 
		m_pRankSexy->m_GCPanel.nTextureSetIndex = -999; 
		m_pRankSexy->SetVisible(0, 0);
		m_pControlContainer->AddItem(static_cast<SControl*>(m_pRankSexy));

	}
	  
	return 1; 
}



int TMFieldScene::OnPacketRankList(int scene)
{
	auto Table = _HudControl.Ranking;
	m_pRankSexy->currentpage = scene  ; // Atualiza para a página recebida
	m_pRankSexy->totalpage = Table.Rank[0].TotalPage; // Total de páginas enviado pelo servidor

	char strSexy[4096];

	// Reseta todos os painéis
	for (int i = 0; i < MAX_RANK; ++i) {
		m_pRankSexy->SetReset(i);
	}
	snprintf(strSexy, sizeof(strSexy), "%d", m_pRankSexy->currentpage);

	m_pRankSexy->m_pCurrentPage->SetText((char*)strSexy,0);
	
	int rankCounter = (scene - 1) * MAX_RANK + 1; // Calcula o contador de ranking baseado na página
	
	// Exibe os 10 participantes da página atual
	for (int i = 0; i < MAX_RANK; ++i) {
		if (strlen(Table.Rank[i].Player) < 4 || strlen(Table.Rank[i].Player) > 16) {
			m_pRankSexy->SetReset(i);
			m_pRankSexy->m_pEvolution[i]->SetTextureSetIndex(-2);
			m_pRankSexy->m_pCape[i]->SetTextureSetIndex(-2);
			m_pRankSexy->m_pClasse[i]->SetTextureSetIndex(-2);

			continue; // Pula jogadores com nomes inválidos
		}
		if (Table.Rank[i].Level <= 0) {
			m_pRankSexy->SetReset(i);
			m_pRankSexy->m_pEvolution[i]->SetTextureSetIndex(-2);
			m_pRankSexy->m_pCape[i]->SetTextureSetIndex(-2);
			m_pRankSexy->m_pClasse[i]->SetTextureSetIndex(-2);

			continue; // Pula jogadores com nomes inválidos
		}

		if (Table.Rank[i].Level >= 1 && Table.Rank[i].Level <= 400) {
			 

			snprintf(strSexy, sizeof(strSexy), "%dº", rankCounter);
			rankCounter++;
			m_pRankSexy->m_pRankPos[i]->SetText(strSexy, 0);

			snprintf(strSexy, sizeof(strSexy), "%s", Table.Rank[i].Player);
			m_pRankSexy->m_pRankName[i]->SetText(strSexy);

			snprintf(strSexy, sizeof(strSexy), "%d", Table.Rank[i].Level);
			m_pRankSexy->m_pLevel[i]->SetText(strSexy, 0);
			  
			snprintf(strSexy, sizeof(strSexy), "%d", Table.Rank[i].Frags);
			m_pRankSexy->m_pFrag[i]->SetText(strSexy, 0);
			snprintf(strSexy, sizeof(strSexy), "%d", Table.Rank[i].Power);
			m_pRankSexy->m_pPower[i]->SetText(strSexy, 0);





			 


			const char* evolution[5] = { "", "Mortal", "Arch", "Celestial", "SubCelestial" };
			char szEvolution[16]{};
			sprintf(szEvolution, "%s", evolution[Table.Rank[i].Evolution]);
			 

			m_pRankSexy->m_pEvolution[i]->m_GCPanel.nTextureSetIndex = 526;
			m_pRankSexy->m_pCape[i]->m_GCPanel.nTextureSetIndex = 526;
			m_pRankSexy->m_pClasse[i]->m_GCPanel.nTextureSetIndex = 200; 


			if (Table.Rank[i].Evolution == 1)//MORTAL
			{ 
				m_pRankSexy->m_pEvolution[i]->m_GCPanel.nTextureIndex = 404;
			}
			if (Table.Rank[i].Evolution == 2)//ARCH
			{
				m_pRankSexy->m_pEvolution[i]->m_GCPanel.nTextureIndex = 406;
			}
			if (Table.Rank[i].Evolution == 3)//CELESTIAL
			{
				m_pRankSexy->m_pEvolution[i]->m_GCPanel.nTextureIndex = 716;
			}
			if (Table.Rank[i].Evolution == 4)//SUB CELESTIAL
			{
				m_pRankSexy->m_pEvolution[i]->m_GCPanel.nTextureIndex = 716;
			}

			if (Table.Rank[i].Class == 0)//TK
			{
				m_pRankSexy->m_pClasse[i]->m_GCPanel.nTextureIndex = 3;
			}
			if(Table.Rank[i].Class == 1)//FM
			{ 
				m_pRankSexy->m_pClasse[i]->m_GCPanel.nTextureIndex = 35;
			}
			if (Table.Rank[i].Class == 2)//BM
			{ 
				m_pRankSexy->m_pClasse[i]->m_GCPanel.nTextureIndex = 71;
			}
			if (Table.Rank[i].Class == 3)//HT
			{
				m_pRankSexy->m_pClasse[i]->m_GCPanel.nTextureIndex = 86;
			}
			 

			 
			if (Table.Rank[i].Kingdom == 7)// 
			{
				m_pRankSexy->m_pSlotRank[i]->m_GCPanel.dwColor = 0x2C0000F0;
				m_pRankSexy->m_pCape[i]->m_GCPanel.nTextureIndex = 484;
			}
			else if (Table.Rank[i].Kingdom == 8)// 
			{
				m_pRankSexy->m_pSlotRank[i]->m_GCPanel.dwColor = 0x2CF00000;
				m_pRankSexy->m_pCape[i]->m_GCPanel.nTextureIndex =  485;
			}
			else
			{
				m_pRankSexy->m_pSlotRank[i]->m_GCPanel.dwColor = 0x1C00F000;
				m_pRankSexy->m_pCape[i]->m_GCPanel.nTextureIndex = 486;
			} 

			if (!Table.Rank[i].Evolution)
			{
				m_pRankSexy->m_pEvolution[i]->SetVisible(0);
			}
			else
			{
				m_pRankSexy->m_pEvolution[i]->SetVisible(1);
			}

			if (m_pRankSexy->tipo == 0)
			{
				m_pRankSexy->m_pPower[i]->SetText((char*)"", 0);
				m_pRankSexy->m_pFrag[i]->SetText((char*)"", 0);
				m_pRankSexy->m_pLevel[i]->SetText((char*)"", 0);
				snprintf(strSexy, sizeof strSexy, "%d", Table.Rank[i].Level);
				m_pRankSexy->m_pLevel[i]->SetText(strSexy, 0);
			}
			if (m_pRankSexy->tipo == 1)
			{
				m_pRankSexy->m_pPower[i]->SetText((char*)"", 0);
				m_pRankSexy->m_pFrag[i]->SetText((char*)"", 0);
				m_pRankSexy->m_pLevel[i]->SetText((char*)"", 0);
				snprintf(strSexy, sizeof strSexy, "%d", Table.Rank[i].Frags);
				m_pRankSexy->m_pFrag[i]->SetText(strSexy, 0); 
			}
			if (m_pRankSexy->tipo == 2)
			{
				m_pRankSexy->m_pPower[i]->SetText((char*)"", 0);
				m_pRankSexy->m_pFrag[i]->SetText((char*)"", 0);
				m_pRankSexy->m_pLevel[i]->SetText((char*)"", 0);
				snprintf(strSexy, sizeof strSexy, "%d", Table.Rank[i].Power);
				m_pRankSexy->m_pPower[i]->SetText(strSexy, 0); 
			}

			m_pRankSexy->SetVisibleForPanel(i, 1);
		}
		else
		{
			m_pRankSexy->SetReset(i);
			continue; // Pula jogadores com nível inválido
		}

	} 

	return 1;
}



 


CapeRankingSexy::CapeRankingSexy(const char* istrMessage, float inX, float inY, float inWidth, float inHeight)
	: SPanel(-45, inX, inY, inWidth, inHeight, 0xFFFFFFFF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH) // Painel principal azul
{
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_MESSAGEPANEL;
	 
	m_dwControlID = 8500001;
	 

	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

	float posX = 0;
	float posY = 0;

	for (size_t i = 0; i < 7; i++)
	{
		m_pBtnMenu[i] = new SButton(-2, 0, 0, 60.0f, 30.0f, 0x77777777, 1, (char*)"");

		if (m_pBtnMenu[i] != nullptr)
		{
			if (pFScene->m_pControlContainer)
				m_pBtnMenu[i]->SetEventListener(this);
			else
				m_pBtnMenu[i]->SetEventListener(nullptr);

			m_pBgBtnMenu[i] = new SPanel(-1, 0, 0, 65.0f, 70.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);


			m_pBtnMenu[i]->SetControlID(i + 850501);
			AddChild(m_pBtnMenu[i]); 

			AddChild(m_pBgBtnMenu[i]);
		}
	}
	 


	for (size_t i = 0; i < MAX_RANK; i++)
	{

		posX = inX + (i % 1) * (m_panelSizeX + m_spacingX);
		posY = inY + (i / 1) * (m_panelSizeY + m_spacingX);

		m_pRankPos[i] = new SText(-2, istrMessage, 0xFFFFFFFF, 0, 0, 10.0f, 10.0f, 1, 0, 1, 1);
		 
		m_pLevel[i] = new SText(-2, "", 0xFFFFFFFF, 0, 0, 20.0f, 10.0f, 1, 0, 1, 1);
		m_pFrag[i] = new SText(-2, "", 0xFFFFFFFF, 0, 0, 20.0f, 10.0f, 1, 0, 1, 1);
		m_pPower[i] = new SText(-2, "", 0xFFFFFFFF, 0, 0, 20.0f, 10.0f, 1, 0, 1, 1);
		m_pEvolution[i] = new SPanel(-2, posX, posY, 18.0f, 18.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_pCape[i] = new SPanel(-2, posX, posY, 18.0f, 18.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		if (m_pCape[i] != nullptr)
			AddChild(m_pCape[i]);

		m_pClasse[i] = new SPanel(-2, posX, posY, 18.0f, 18.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

		m_pSlotRank[i] = new SPanel(-2, posX, posY, 85.0f, 110.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		 

		m_pRankName[i] = new SButton(-2, 0, 0, 20.0f, 20.0f, 0xFFFFFFFF, 1, (char*)istrMessage);
		
		m_pVermais[i] = new SButton(-2, 395.0f, 128.0f, 20.0f, 20.0f, 0x77777777, 1, (char*)"ver mais");

		if (m_pVermais[i] != nullptr)
		{
			if (pFScene->m_pControlContainer)
				m_pVermais[i]->SetEventListener(this);
			else
				m_pVermais[i]->SetEventListener(nullptr);

			m_pVermais[i]->SetControlID(i + 750501);
			AddChild(m_pVermais[i]);
		}
		
			if (m_pEvolution[i] != nullptr)
				AddChild(m_pEvolution[i]);
		if (m_pRankPos[i] != nullptr)
			AddChild(m_pRankPos[i]); 

		if (m_pRankName[i] != nullptr)
		{
			if (pFScene->m_pControlContainer)
				m_pRankName[i]->SetEventListener(this);
			else
				m_pRankName[i]->SetEventListener(nullptr);

			m_pRankName[i]->SetControlID(i + 950501);
			AddChild(m_pRankName[i]);
		}
		if (m_pLevel[i] != nullptr)
			AddChild(m_pLevel[i]);

		if (m_pFrag[i] != nullptr)
			AddChild(m_pFrag[i]);

		if (m_pPower[i] != nullptr)
			AddChild(m_pPower[i]);
		
		if (m_pClasse[i] != nullptr)
			AddChild(m_pClasse[i]);




		if (m_pSlotRank[i] != nullptr)
			AddChild(m_pSlotRank[i]);

	}

	m_pCurrentPage = new SText(-2, "0", 0xFFFFFFFF, ((float)g_pDevice->m_dwScreenWidth / 2.0f - (35.5f)) * RenderDevice::m_fWidthRatio, 215.0f * RenderDevice::m_fHeightRatio, 20.0f, 10.0f, 1, 0, 1, 1);

	if (m_pCurrentPage != nullptr)
		AddChild(m_pCurrentPage);

	m_pTitulo[0] = new SText(-2, "Pos", 0xFFFFFFFF, ((float)g_pDevice->m_dwScreenWidth / 2.0f - (35.5f)) * RenderDevice::m_fWidthRatio, 215.0f * RenderDevice::m_fHeightRatio, 20.0f, 10.0f, 1, 0, 1, 1);

	if (m_pTitulo[0] != nullptr)
		AddChild(m_pTitulo[0]);

	m_pTitulo[1] = new SText(-2, "Nick", 0xFFFFFFFF, ((float)g_pDevice->m_dwScreenWidth / 1.2f) * RenderDevice::m_fWidthRatio, 1.0f * RenderDevice::m_fHeightRatio, 20.0f, 10.0f, 1, 0, 1, 1);

	if (m_pTitulo[1] != nullptr)
		AddChild(m_pTitulo[1]);

	m_pTitulo[2] = new SText(-2, "PvP", 0xFFFFFFFF, ((float)g_pDevice->m_dwScreenWidth / 2.0f - (35.5f)) * RenderDevice::m_fWidthRatio, 235.0f * RenderDevice::m_fHeightRatio, 20.0f, 10.0f, 1, 0, 1, 1);

	if (m_pTitulo[2] != nullptr)
		AddChild(m_pTitulo[2]);


	m_pTitulo[3] = new SText(-2, "Evo", 0xFFFFFFFF, ((float)g_pDevice->m_dwScreenWidth / 2.0f - (35.5f)) * RenderDevice::m_fWidthRatio, 235.0f * RenderDevice::m_fHeightRatio, 20.0f, 10.0f, 1, 0, 1, 1);

	if (m_pTitulo[3] != nullptr)
		AddChild(m_pTitulo[3]);

	m_pTitulo[4] = new SText(-2, "Class", 0xFFFFFFFF, ((float)g_pDevice->m_dwScreenWidth / 2.0f - (35.5f)) * RenderDevice::m_fWidthRatio, 235.0f * RenderDevice::m_fHeightRatio, 20.0f, 10.0f, 1, 0, 1, 1);

	if (m_pTitulo[4] != nullptr)
		AddChild(m_pTitulo[4]);

	m_pTitulo[5] = new SText(-2, "Cape", 0xFFFFFFFF, ((float)g_pDevice->m_dwScreenWidth / 2.0f - (35.5f)) * RenderDevice::m_fWidthRatio, 235.0f * RenderDevice::m_fHeightRatio, 20.0f, 10.0f, 1, 0, 1, 1);

	if (m_pTitulo[5] != nullptr)
		AddChild(m_pTitulo[5]);

	m_pTitulo[6] = new SText(-2, "Ranking", 0xFFFFFFFF, ((float)g_pDevice->m_dwScreenWidth / 2.0f - (35.5f)) * RenderDevice::m_fWidthRatio, 235.0f * RenderDevice::m_fHeightRatio, 20.0f, 10.0f, 1, 0, 1, 1);

	if (m_pTitulo[6] != nullptr)
		AddChild(m_pTitulo[6]);

	m_pBgGrade = new SPanel(-776, 1.0f * RenderDevice::m_fWidthRatio, 1.0f * RenderDevice::m_fHeightRatio, 0 * RenderDevice::m_fWidthRatio, 0 * RenderDevice::m_fHeightRatio, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

	if (m_pBgGrade != nullptr)
		AddChild(m_pBgGrade);
	 

}

CapeRankingSexy::~CapeRankingSexy()
{
	
		SAFE_DELETE(m_pCurrentPage);
	for (size_t i = 0; i < 7; i++)
	{
		SAFE_DELETE(m_pTitulo[i]); 
	}
	for (size_t i = 0; i < MAX_RANK; i++)
	{
		 
		SAFE_DELETE(m_pPower[i]);
		SAFE_DELETE(m_pRankPos[i]); 
		SAFE_DELETE(m_pLevel[i]);
		SAFE_DELETE(m_pFrag[i]);
	}
} 

int CapeRankingSexy::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (m_bSelectEnable == 0)
		return 0;

	if (m_dwControlID == 65943 || m_dwControlID == 65947)
		return 0;

	int bInCaption = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, 24.0f);
	m_bOver = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);

	if (m_bOver == 0 && m_pDescPanel != nullptr)
		m_pDescPanel->SetVisible(0);
	 
	return 0;
} 

void CapeRankingSexy::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	unsigned int dwServerTime = g_pTimerManager->GetServerTime();
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

	SPanel::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);
	 
	m_pBgGrade->FrameMove(dwServerTime);
	 
	 
	for (size_t i = 0; i < MAX_RANK; i++)
	{
		m_pCape[i]->FrameMove(dwServerTime);
		m_pClasse[i]->FrameMove(dwServerTime);
		m_pSlotRank[i]->FrameMove(dwServerTime);
	}

	UpdateVisibility();

}

void CapeRankingSexy::UpdateVisibility()
{
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

	auto res = RenderDevice::m_fResBase;

	for (size_t i = 0; i < MAX_RANK; i++)
	{ 
		auto BgRank1 = (SPanel*)pFScene->m_pControlContainer->FindControl(12880);
		auto BgRank = (SPanel*)pFScene->m_pControlContainer->FindControl(12882);
		auto Profile = (SPanel*)pFScene->m_pControlContainer->FindControl(148700);

		if (res == 1)
		{
			SetPos(230.0f, 140.5f);
			if (i == 1)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(210.0f * RenderDevice::m_fWidthRatio, 290.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 2)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Kill");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(49.0f * RenderDevice::m_fWidthRatio, -55.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 3)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Power");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(159.0f * RenderDevice::m_fWidthRatio, -55.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 4)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Level");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(277.0f * RenderDevice::m_fWidthRatio, -55.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 5)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(5.0f * RenderDevice::m_fWidthRatio, 290.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 6)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(48.0f * RenderDevice::m_fWidthRatio, 290.0f * RenderDevice::m_fHeightRatio);
			}

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 20.0f * RenderDevice::m_fHeightRatio);

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 28.0f * RenderDevice::m_fHeightRatio);

			m_pRankPos[0]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pRankPos[1]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pRankPos[2]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pRankPos[3]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pRankPos[4]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pRankPos[5]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pRankPos[6]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pRankPos[7]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pRankPos[8]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pRankPos[9]->SetPos(20.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pRankName[0]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 8.0f);
			m_pRankName[1]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 38.0f);
			m_pRankName[2]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 66.0f);
			m_pRankName[3]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 94.0f);
			m_pRankName[4]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 122.0f);
			m_pRankName[5]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 150.0f);
			m_pRankName[6]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 178.0f);
			m_pRankName[7]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 206.0f);
			m_pRankName[8]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 234.0f);
			m_pRankName[9]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 262.0f);

			m_pLevel[0]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pLevel[1]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pLevel[2]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pLevel[3]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pLevel[4]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pLevel[5]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pLevel[6]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pLevel[7]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pLevel[8]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pLevel[9]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pFrag[0]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pFrag[1]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pFrag[2]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pFrag[3]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pFrag[4]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pFrag[5]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pFrag[6]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pFrag[7]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pFrag[8]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pFrag[9]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pPower[0]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pPower[1]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pPower[2]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pPower[3]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pPower[4]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pPower[5]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pPower[6]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pPower[7]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pPower[8]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pPower[9]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pEvolution[0]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pEvolution[1]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pEvolution[2]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pEvolution[3]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pEvolution[4]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pEvolution[5]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pEvolution[6]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pEvolution[7]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pEvolution[8]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pEvolution[9]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 266.0f);


			m_pClasse[0]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pClasse[1]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pClasse[2]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pClasse[3]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pClasse[4]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pClasse[5]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pClasse[6]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pClasse[7]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pClasse[8]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pClasse[9]->SetPos(350.0f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pCape[0]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pCape[1]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pCape[2]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pCape[3]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pCape[4]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pCape[5]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pCape[6]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pCape[7]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pCape[8]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pCape[9]->SetPos(395.0f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pSlotRank[i]->SetSize(85.0f, 145.0f);

			Profile->SetPos(2.0f * RenderDevice::m_fWidthRatio, 55.0f * RenderDevice::m_fHeightRatio);

			BgRank1->SetPos(230.0f * RenderDevice::m_fWidthRatio, 55.0f * RenderDevice::m_fHeightRatio);
			BgRank->SetSize(430.0f, 400.0f);
			pFScene->m_pEditRank->SetPos(120.0f * RenderDevice::m_fWidthRatio, 375.0f * RenderDevice::m_fHeightRatio);
			m_pTitulo[0]->SetPos(15.0f * RenderDevice::m_fWidthRatio, -20.0f);
			m_pTitulo[1]->SetPos(95.0f * RenderDevice::m_fWidthRatio, -20.0f);
			m_pTitulo[2]->SetPos(255.0f * RenderDevice::m_fWidthRatio, -20.0f);
			m_pTitulo[3]->SetPos(310.0f * RenderDevice::m_fWidthRatio, -20.0f);
			m_pTitulo[4]->SetPos(350.0f * RenderDevice::m_fWidthRatio, -20.0f);
			m_pTitulo[5]->SetPos(395.0f * RenderDevice::m_fWidthRatio, -20.0f);
			m_pTitulo[6]->SetPos(200.0f * RenderDevice::m_fWidthRatio, -78.0f);
			m_pCurrentPage->SetPos(27.0f * RenderDevice::m_fWidthRatio, 295.0f);

		}
		if (res == 2)
		{
			SetPos(230.0f, 140.5f);
			if (i == 1)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(213.0f * RenderDevice::m_fWidthRatio, 257.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 2)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Kill");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetPos(60.0f * RenderDevice::m_fWidthRatio, -41.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 3)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Power");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(155.0f * RenderDevice::m_fWidthRatio, -41.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 4)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Level");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(255.0f * RenderDevice::m_fWidthRatio, -41.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 5)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(32.0f * RenderDevice::m_fWidthRatio, 257.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 6)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(75.0f * RenderDevice::m_fWidthRatio, 257.0f * RenderDevice::m_fHeightRatio);
			}

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 20.0f * RenderDevice::m_fHeightRatio);

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 28.0f * RenderDevice::m_fHeightRatio);

			m_pRankPos[0]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pRankPos[1]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pRankPos[2]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pRankPos[3]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pRankPos[4]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pRankPos[5]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pRankPos[6]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pRankPos[7]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pRankPos[8]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pRankPos[9]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pRankName[0]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 8.0f);
			m_pRankName[1]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 38.0f);
			m_pRankName[2]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 66.0f);
			m_pRankName[3]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 94.0f);
			m_pRankName[4]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 122.0f);
			m_pRankName[5]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 150.0f);
			m_pRankName[6]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 178.0f);
			m_pRankName[7]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 206.0f);
			m_pRankName[8]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 234.0f);
			m_pRankName[9]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 262.0f);

			m_pLevel[0]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pLevel[1]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pLevel[2]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pLevel[3]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pLevel[4]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pLevel[5]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pLevel[6]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pLevel[7]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pLevel[8]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pLevel[9]->SetPos(150.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pFrag[0]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pFrag[1]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pFrag[2]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pFrag[3]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pFrag[4]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pFrag[5]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pFrag[6]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pFrag[7]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pFrag[8]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pFrag[9]->SetPos(160.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pPower[0]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pPower[1]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pPower[2]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pPower[3]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pPower[4]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pPower[5]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pPower[6]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pPower[7]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pPower[8]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pPower[9]->SetPos(152.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pEvolution[0]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pEvolution[1]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pEvolution[2]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pEvolution[3]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pEvolution[4]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pEvolution[5]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pEvolution[6]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pEvolution[7]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pEvolution[8]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pEvolution[9]->SetPos(298.0f * RenderDevice::m_fWidthRatio, 266.0f);


			m_pClasse[0]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pClasse[1]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pClasse[2]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pClasse[3]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pClasse[4]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pClasse[5]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pClasse[6]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pClasse[7]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pClasse[8]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pClasse[9]->SetPos(336.0f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pCape[0]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pCape[1]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pCape[2]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pCape[3]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pCape[4]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pCape[5]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pCape[6]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pCape[7]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pCape[8]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pCape[9]->SetPos(372.0f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pSlotRank[i]->SetSize(85.0f, 145.0f);

			Profile->SetPos(2.0f * RenderDevice::m_fWidthRatio, 52.0f * RenderDevice::m_fHeightRatio);

			BgRank1->SetPos(230.0f * RenderDevice::m_fWidthRatio, 55.0f * RenderDevice::m_fHeightRatio);
			BgRank->SetSize(430.0f, 400.0f);
			pFScene->m_pEditRank->SetPos(101.0f * RenderDevice::m_fWidthRatio, 325.0f * RenderDevice::m_fHeightRatio);
			m_pTitulo[0]->SetPos(40.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[1]->SetPos(135.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[2]->SetPos(250.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[3]->SetPos(298.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[4]->SetPos(335.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[5]->SetPos(370.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[6]->SetPos(200.0f * RenderDevice::m_fWidthRatio, -68.0f);
			m_pCurrentPage->SetPos(53.0f * RenderDevice::m_fWidthRatio, 302.0f);
		}

		if (res == 3)
		{
			SetPos(265.0f, 150.5f);
			if (i == 1)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(188.0f * RenderDevice::m_fWidthRatio, 220.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 2)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Kill");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetPos(50.0f * RenderDevice::m_fWidthRatio, -40.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 3)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Power");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(133.0f * RenderDevice::m_fWidthRatio, -40.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 4)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Level");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(219.0f * RenderDevice::m_fWidthRatio, -40.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 5)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(35.0f * RenderDevice::m_fWidthRatio, 220.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 6)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(65.0f * RenderDevice::m_fWidthRatio, 220.0f * RenderDevice::m_fHeightRatio);
			}

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 20.0f * RenderDevice::m_fHeightRatio);

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 28.0f * RenderDevice::m_fHeightRatio);

			m_pRankPos[0]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pRankPos[1]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pRankPos[2]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pRankPos[3]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pRankPos[4]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pRankPos[5]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pRankPos[6]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pRankPos[7]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pRankPos[8]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pRankPos[9]->SetPos(45.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pRankName[0]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 8.0f);
			m_pRankName[1]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 38.0f);
			m_pRankName[2]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 66.0f);
			m_pRankName[3]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 94.0f);
			m_pRankName[4]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 122.0f);
			m_pRankName[5]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 150.0f);
			m_pRankName[6]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 178.0f);
			m_pRankName[7]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 206.0f);
			m_pRankName[8]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 234.0f);
			m_pRankName[9]->SetPos(110.0f * RenderDevice::m_fWidthRatio, 262.0f);

			m_pLevel[0]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pLevel[1]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pLevel[2]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pLevel[3]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pLevel[4]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pLevel[5]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pLevel[6]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pLevel[7]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pLevel[8]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pLevel[9]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pFrag[0]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pFrag[1]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pFrag[2]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pFrag[3]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pFrag[4]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pFrag[5]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pFrag[6]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pFrag[7]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pFrag[8]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pFrag[9]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pPower[0]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pPower[1]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pPower[2]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pPower[3]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pPower[4]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pPower[5]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pPower[6]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pPower[7]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pPower[8]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pPower[9]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pEvolution[0]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pEvolution[1]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pEvolution[2]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pEvolution[3]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pEvolution[4]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pEvolution[5]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pEvolution[6]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pEvolution[7]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pEvolution[8]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pEvolution[9]->SetPos(261.0f * RenderDevice::m_fWidthRatio, 266.0f);


			m_pClasse[0]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pClasse[1]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pClasse[2]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pClasse[3]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pClasse[4]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pClasse[5]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pClasse[6]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pClasse[7]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pClasse[8]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pClasse[9]->SetPos(292.5f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pCape[0]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pCape[1]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pCape[2]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pCape[3]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pCape[4]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pCape[5]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pCape[6]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pCape[7]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pCape[8]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pCape[9]->SetPos(324.5f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pSlotRank[i]->SetSize(85.0f, 145.0f);

			Profile->SetPos(2.0f * RenderDevice::m_fWidthRatio, 50.0f * RenderDevice::m_fHeightRatio);

			BgRank1->SetPos(230.0f * RenderDevice::m_fWidthRatio, 55.0f * RenderDevice::m_fHeightRatio);
			BgRank->SetSize(430.0f, 400.0f);
			pFScene->m_pEditRank->SetPos(60.0f * RenderDevice::m_fWidthRatio, 278.0f * RenderDevice::m_fHeightRatio);
			m_pTitulo[0]->SetPos(40.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[1]->SetPos(115.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[2]->SetPos(220.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[3]->SetPos(261.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[4]->SetPos(293.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[5]->SetPos(325.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[6]->SetPos(185.0f * RenderDevice::m_fWidthRatio, -70.0f);
			m_pCurrentPage->SetPos(50.0f * RenderDevice::m_fWidthRatio, 300.0f);
		}

		if (res == 4)
		{
			SetPos(260.0f, 150.5f);
			if (i == 1)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(188.0f * RenderDevice::m_fWidthRatio, 230.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 2)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Kill");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetPos(47.0f * RenderDevice::m_fWidthRatio, -43.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 3)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Power");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(133.0f * RenderDevice::m_fWidthRatio, -43.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 4)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Level");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(222.0f * RenderDevice::m_fWidthRatio, -43.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 5)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(30.0f * RenderDevice::m_fWidthRatio, 230.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 6)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(60.0f * RenderDevice::m_fWidthRatio, 230.0f * RenderDevice::m_fHeightRatio);
			}

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 20.0f * RenderDevice::m_fHeightRatio);

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 28.0f * RenderDevice::m_fHeightRatio);

			m_pRankPos[0]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pRankPos[1]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pRankPos[2]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pRankPos[3]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pRankPos[4]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pRankPos[5]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pRankPos[6]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pRankPos[7]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pRankPos[8]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pRankPos[9]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pRankName[0]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 8.0f);
			m_pRankName[1]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 38.0f);
			m_pRankName[2]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 66.0f);
			m_pRankName[3]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 94.0f);
			m_pRankName[4]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 122.0f);
			m_pRankName[5]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 150.0f);
			m_pRankName[6]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 178.0f);
			m_pRankName[7]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 206.0f);
			m_pRankName[8]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 234.0f);
			m_pRankName[9]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 262.0f);

			m_pLevel[0]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pLevel[1]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pLevel[2]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pLevel[3]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pLevel[4]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pLevel[5]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pLevel[6]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pLevel[7]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pLevel[8]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pLevel[9]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pFrag[0]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pFrag[1]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pFrag[2]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pFrag[3]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pFrag[4]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pFrag[5]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pFrag[6]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pFrag[7]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pFrag[8]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pFrag[9]->SetPos(140.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pPower[0]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pPower[1]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pPower[2]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pPower[3]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pPower[4]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pPower[5]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pPower[6]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pPower[7]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pPower[8]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pPower[9]->SetPos(135.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pEvolution[0]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pEvolution[1]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pEvolution[2]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pEvolution[3]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pEvolution[4]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pEvolution[5]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pEvolution[6]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pEvolution[7]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pEvolution[8]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pEvolution[9]->SetPos(267.0f * RenderDevice::m_fWidthRatio, 266.0f);


			m_pClasse[0]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pClasse[1]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pClasse[2]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pClasse[3]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pClasse[4]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pClasse[5]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pClasse[6]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pClasse[7]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pClasse[8]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pClasse[9]->SetPos(301.5f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pCape[0]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pCape[1]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pCape[2]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pCape[3]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pCape[4]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pCape[5]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pCape[6]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pCape[7]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pCape[8]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pCape[9]->SetPos(333.5f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pSlotRank[i]->SetSize(85.0f, 145.0f);

			Profile->SetPos(2.0f * RenderDevice::m_fWidthRatio, 50.0f * RenderDevice::m_fHeightRatio);

			BgRank1->SetPos(230.0f * RenderDevice::m_fWidthRatio, 55.0f * RenderDevice::m_fHeightRatio);
			BgRank->SetSize(430.0f, 400.0f);
			pFScene->m_pEditRank->SetPos(60.0f * RenderDevice::m_fWidthRatio, 294.0f * RenderDevice::m_fHeightRatio);
			m_pTitulo[0]->SetPos(40.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[1]->SetPos(115.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[2]->SetPos(220.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[3]->SetPos(266.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[4]->SetPos(301.0f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[5]->SetPos(333.5f * RenderDevice::m_fWidthRatio, -13.0f);
			m_pTitulo[6]->SetPos(185.0f * RenderDevice::m_fWidthRatio, -70.0f);
			m_pCurrentPage->SetPos(45.0f * RenderDevice::m_fWidthRatio, 300.0f);
		}

		if (res == 5)
		{
			SetPos(280.0f, 150.5f);
			if (i == 1)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(180.0f * RenderDevice::m_fWidthRatio, 215.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 2)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Kill");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetPos(45.0f * RenderDevice::m_fWidthRatio, -38.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 3)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Power");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(123.0f * RenderDevice::m_fWidthRatio, -38.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 4)
			{
				m_pBtnMenu[i]->SetText((char*)"Top Level");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetPos(210.0f * RenderDevice::m_fWidthRatio, -38.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 5)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(30.0f * RenderDevice::m_fWidthRatio, 218.0f * RenderDevice::m_fHeightRatio);
			}
			if (i == 6)
			{
				m_pBtnMenu[i]->SetText((char*)"");
				m_pBtnMenu[i]->SetVisible(1);
				m_pBtnMenu[i]->SetTextureSetIndex(-2);
				m_pBtnMenu[i]->SetSize(20.0f * RenderDevice::m_fWidthRatio, 20.0f * RenderDevice::m_fHeightRatio);
				m_pBtnMenu[i]->SetPos(60.0f * RenderDevice::m_fWidthRatio, 218.0f * RenderDevice::m_fHeightRatio);
			}

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 20.0f * RenderDevice::m_fHeightRatio);

			m_pSlotRank[i]->SetPos(25.0f * RenderDevice::m_fWidthRatio, (i / 1) * 28.0f * RenderDevice::m_fHeightRatio);

			m_pRankPos[0]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pRankPos[1]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pRankPos[2]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pRankPos[3]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pRankPos[4]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pRankPos[5]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pRankPos[6]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pRankPos[7]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pRankPos[8]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pRankPos[9]->SetPos(40.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pRankName[0]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 8.0f);
			m_pRankName[1]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 38.0f);
			m_pRankName[2]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 66.0f);
			m_pRankName[3]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 94.0f);
			m_pRankName[4]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 122.0f);
			m_pRankName[5]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 150.0f);
			m_pRankName[6]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 178.0f);
			m_pRankName[7]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 206.0f);
			m_pRankName[8]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 234.0f);
			m_pRankName[9]->SetPos(100.0f * RenderDevice::m_fWidthRatio, 262.0f);

			m_pLevel[0]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pLevel[1]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pLevel[2]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pLevel[3]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pLevel[4]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pLevel[5]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pLevel[6]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pLevel[7]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pLevel[8]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pLevel[9]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pFrag[0]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pFrag[1]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pFrag[2]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pFrag[3]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pFrag[4]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pFrag[5]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pFrag[6]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pFrag[7]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pFrag[8]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pFrag[9]->SetPos(130.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pPower[0]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pPower[1]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pPower[2]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pPower[3]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pPower[4]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pPower[5]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pPower[6]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pPower[7]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pPower[8]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pPower[9]->SetPos(125.0f * RenderDevice::m_fWidthRatio, 266.0f);

			m_pEvolution[0]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pEvolution[1]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pEvolution[2]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pEvolution[3]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pEvolution[4]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pEvolution[5]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pEvolution[6]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pEvolution[7]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pEvolution[8]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pEvolution[9]->SetPos(250.0f * RenderDevice::m_fWidthRatio, 266.0f);


			m_pClasse[0]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pClasse[1]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pClasse[2]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pClasse[3]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pClasse[4]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pClasse[5]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pClasse[6]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pClasse[7]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pClasse[8]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pClasse[9]->SetPos(280.0f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pCape[0]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 14.0f);
			m_pCape[1]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 42.0f);
			m_pCape[2]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 70.0f);
			m_pCape[3]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 98.0f);
			m_pCape[4]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 126.0f);
			m_pCape[5]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 154.0f);
			m_pCape[6]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 182.0f);
			m_pCape[7]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 210.0f);
			m_pCape[8]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 238.0f);
			m_pCape[9]->SetPos(310.0f * RenderDevice::m_fWidthRatio, 266.0f);



			m_pSlotRank[i]->SetSize(85.0f, 145.0f);

			Profile->SetPos(-1.0f * RenderDevice::m_fWidthRatio, 50.0f * RenderDevice::m_fHeightRatio);

			BgRank1->SetPos(230.0f * RenderDevice::m_fWidthRatio, 55.0f * RenderDevice::m_fHeightRatio);
			BgRank->SetSize(430.0f, 400.0f);
			pFScene->m_pEditRank->SetPos(58.0f * RenderDevice::m_fWidthRatio, 270.0f * RenderDevice::m_fHeightRatio);
			m_pTitulo[0]->SetPos(35.0f * RenderDevice::m_fWidthRatio, -10.0f);
			m_pTitulo[1]->SetPos(100.0f * RenderDevice::m_fWidthRatio, -10.0f);
			m_pTitulo[2]->SetPos(210.0f * RenderDevice::m_fWidthRatio, -10.0f);
			m_pTitulo[3]->SetPos(249.0f * RenderDevice::m_fWidthRatio, -10.0f);
			m_pTitulo[4]->SetPos(280.0f * RenderDevice::m_fWidthRatio, -10.0f);
			m_pTitulo[5]->SetPos(310.5f * RenderDevice::m_fWidthRatio, -10.0f);
			m_pTitulo[6]->SetPos(170.0f * RenderDevice::m_fWidthRatio, -69.0f);
			m_pCurrentPage->SetPos(45.0f * RenderDevice::m_fWidthRatio, 300.0f);
		}
		m_pRankPos[i]->m_bVisible = 1;
		m_pRankName[i]->m_bVisible = 1;
		m_pCape[i]->m_bVisible = 1;
		m_pEvolution[i]->m_bVisible = 1;
		m_pLevel[i]->m_bVisible = 1;
		m_pClasse[i]->m_bVisible = 1;


		m_pSlotRank[i]->m_bVisible = 0;
		m_pVermais[i]->m_bVisible = 0;


		// Set alignment
		m_pRankPos[i]->m_dwAlignType = 1;


		m_pLevel[i]->m_dwAlignType = 4;
		m_pFrag[i]->m_dwAlignType = 4;
		m_pPower[i]->m_dwAlignType = 4;
	}
	if (pFScene->m_pAffectDesc)
	{
		for (int i = 0; i < 32; ++i)
		{
			pFScene->m_pAffectDesc->SetVisible(m_bVisible ? 0 : 1);
			pFScene->m_pAffectIcon[i]->SetVisible(m_bVisible ? 0 : 1);
			pFScene->m_pAffect[i]->SetVisible(m_bVisible ? 0 : 1);
		}
	}
}
   
int CapeRankingSexy::OnControlEvent(DWORD idwControlID, DWORD idwEvent) {
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	if (idwControlID >= 750501 && idwControlID <= 750548) {


		auto bb = (SButton*)pFScene->m_pControlContainer->FindControl(idwControlID); 
		if (bb)
		{
			struct pMsgSignalChar
			{
				MSG_STANDARD Header;

				char Nick[16];
			};


			pMsgSignalChar Signal{};
			Signal.Header.Type = 0x912A;
			Signal.Header.ID = g_pObjectManager->m_dwCharID;
			Signal.Header.Size = sizeof(Signal);

			snprintf(Signal.Nick, sizeof(Signal.Nick), "%s", bb->m_pAltText->GetText());

			SendOneMessage((char*)&Signal, sizeof(Signal)); 
		}

	}
	else if (idwControlID >= 850501 && idwControlID <= 850507) {

		auto Table = _HudControl.Ranking;
		char strSexy[56] = {};
		
		for (int i = 0; i < MAX_RANK; ++i)
		{
			if (idwControlID == 850502)
				continue;


			pFScene->m_pRankSexy->SetReset(i);
			pFScene->m_pRankSexy->m_pEvolution[i]->SetTextureSetIndex(-2);
			pFScene->m_pRankSexy->m_pCape[i]->SetTextureSetIndex(-2);
			pFScene->m_pRankSexy->m_pClasse[i]->SetTextureSetIndex(-2);

		}


		if (idwControlID == 850503)
		{
			m_pTitulo[2]->SetText((char*)"PvP", 0);
			for (int i = 0; i < MAX_RANK; ++i)
			{
				if (strlen(Table.Rank[i].Player) < 4 || strlen(Table.Rank[i].Player) > 16) {
					pFScene->m_pRankSexy->SetReset(i);
					pFScene->m_pRankSexy->m_pEvolution[i]->SetTextureSetIndex(-2);
					pFScene->m_pRankSexy->m_pCape[i]->SetTextureSetIndex(-2);
					pFScene->m_pRankSexy->m_pClasse[i]->SetTextureSetIndex(-2);

					continue; // Pula jogadores com nomes inválidos
				}
				pFScene->m_pRankSexy->m_pFrag[i]->SetText((char*)"", 0);
				pFScene->m_pRankSexy->m_pPower[i]->SetText((char*)"", 0);
				pFScene->m_pRankSexy->m_pLevel[i]->SetText((char*)"", 0);
				snprintf(strSexy, sizeof(strSexy), "%d", Table.Rank[i].Frags);
				pFScene->m_pRankSexy->m_pFrag[i]->SetText(strSexy, 0);
			}
		}
		if (idwControlID == 850504)
		{
			m_pTitulo[2]->SetText((char*)"Power", 0);
			for (int i = 0; i < MAX_RANK; ++i)
			{
				if (strlen(Table.Rank[i].Player) < 4 || strlen(Table.Rank[i].Player) > 16) {
					pFScene->m_pRankSexy->SetReset(i);
					pFScene->m_pRankSexy->m_pEvolution[i]->SetTextureSetIndex(-2);
					pFScene->m_pRankSexy->m_pCape[i]->SetTextureSetIndex(-2);
					pFScene->m_pRankSexy->m_pClasse[i]->SetTextureSetIndex(-2);

					continue; // Pula jogadores com nomes inválidos
				}
				pFScene->m_pRankSexy->m_pFrag[i]->SetText((char*)"", 0);
				pFScene->m_pRankSexy->m_pLevel[i]->SetText((char*)"", 0);
				snprintf(strSexy, sizeof(strSexy), "%d", Table.Rank[i].Power);
				pFScene->m_pRankSexy->m_pPower[i]->SetText(strSexy, 0);
			}
		}
		if (idwControlID == 850505)
		{
			m_pTitulo[2]->SetText((char*)"Level", 0);
			for (int i = 0; i < MAX_RANK; ++i)
			{
				if (strlen(Table.Rank[i].Player) < 4 || strlen(Table.Rank[i].Player) > 16) {
					pFScene->m_pRankSexy->SetReset(i);
					pFScene->m_pRankSexy->m_pEvolution[i]->SetTextureSetIndex(-2);
					pFScene->m_pRankSexy->m_pCape[i]->SetTextureSetIndex(-2);
					pFScene->m_pRankSexy->m_pClasse[i]->SetTextureSetIndex(-2);

					continue; // Pula jogadores com nomes inválidos
				}

				pFScene->m_pRankSexy->m_pFrag[i]->SetText((char*)"", 0);
				pFScene->m_pRankSexy->m_pPower[i]->SetText((char*)"", 0);
				pFScene->m_pRankSexy->m_pLevel[i]->SetText((char*)"", 0);
				snprintf(strSexy, sizeof(strSexy), "%d", Table.Rank[i].Level);
				pFScene->m_pRankSexy->m_pLevel[i]->SetText(strSexy, 0);
			}
		}


		// Estrutura de mensagem para solicitação de ranking
		struct MSG_ReqRANK {
			MSG_STANDARD Header;
			int CurrentPage; // Campo para a página atual
		};


		MSG_ReqRANK stParm{};
		stParm.Header.ID = pFScene->m_pMyHuman->m_dwID;
		 
		if (idwControlID == 850502) { // Busca o oponente do ranking
			pFScene->OnControlEvent(148335, 1);
			return 1;
		}
		if (idwControlID == 850505) { // Solicitar ranking level
			tipo = 0;
			PacketSend = 0x528;
			stParm.CurrentPage = 1; 
		}
		if (idwControlID == 850503) { // Solicitar ranking pvp
			tipo = 1;
			PacketSend = 0x529;
			stParm.CurrentPage = 1; 
		}
		if (idwControlID == 850504) { // Solicitar ranking power
			tipo = 2;
			PacketSend = 0x530;
			stParm.CurrentPage = 1; 
		}
		if (idwControlID == 850506) { // Página anterior
			 
			if (pFScene->m_pRankSexy->currentpage <= 1)
				pFScene->m_pRankSexy->currentpage = 1;
			else
				pFScene->m_pRankSexy->currentpage--;

			stParm.CurrentPage = pFScene->m_pRankSexy->currentpage;
		}
		if (idwControlID == 850507) { // Próxima página
			 
			pFScene->m_pRankSexy->currentpage++;
			stParm.CurrentPage = pFScene->m_pRankSexy->currentpage;
		}
		stParm.Header.Type = PacketSend;
		g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));
	}
	else if (idwControlID >= 950501 && idwControlID <= 950510) {

		auto Names = (SButton*)pFScene->m_pControlContainer->FindControl(idwControlID);

		pFScene->m_pEditRank->SetText((char*)Names->m_pAltText->GetText());
	}
	// Lida com o evento e chama o ouvinte de eventos, se necessário
	if (idwEvent == 0 && m_pEventListener != nullptr) {
		m_pEventListener->OnControlEvent(m_dwControlID, 1);
		return 1;
	}
	return 1;
}


void CapeRankingSexy::SetReset(int panelIndex)
{
	if (panelIndex >= 0 && panelIndex < MAX_RANK)
	{
		if (m_pCape[panelIndex])
		{
			
			//m_pEvolution[panelIndex]->SetText((char*)"", 0); 
			m_pRankPos[panelIndex]->SetText((char*)"", 0);
			m_pRankName[panelIndex]->SetText((char*)"");
			m_pPower[panelIndex]->SetText((char*)"",0);
			m_pLevel[panelIndex]->SetText((char*)"", 0);
			m_pFrag[panelIndex]->SetText((char*)"", 0);
			SetVisibleForPanel(panelIndex, 0);
			  
		}
	}
}  
void CapeRankingSexy::SetVisibleForPanel(int panelIndex, int indice)
{
	if (panelIndex >= 0 && panelIndex < MAX_RANK)
	{
		m_pRankPos[panelIndex]->SetVisible(indice);
		m_pRankName[panelIndex]->SetVisible(indice);
		m_pLevel[panelIndex]->SetVisible(indice); m_pFrag[panelIndex]->SetVisible(indice); m_pPower[panelIndex]->SetVisible(indice);
		m_pEvolution[panelIndex]->SetVisible(indice);
		m_pClasse[panelIndex]->SetVisible(indice);
		m_pCape[panelIndex]->SetVisible(indice);
		m_pSlotRank[panelIndex]->SetVisible(indice);
	}
} 
 

void CapeRankingSexy::SetVisible(int bVisible, int bSound)
{
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	auto MainRank = (SPanel*)pFScene->m_pControlContainer->FindControl(12880);


	if(MainRank)
	{
		MainRank->m_bVisible = bVisible;
	}
	if (bVisible)
	{
		if (pFScene->m_pChatPanel)
		{
			pFScene->m_pChatPanel->m_bVisible = 0;
			pFScene->m_pChatList->m_bVisible = 0;
			pFScene->m_pChatListnotice->m_bVisible = 0;
			pFScene->m_pChatBack->m_bVisible = 0;
			pFScene->m_pChatGeneral->m_bVisible = 0;
			pFScene->m_pChatParty->m_bVisible = 0;
			pFScene->m_pChatWhisper->m_bVisible = 0;
			pFScene->m_pChatGuild->m_bVisible = 0;
			pFScene->m_pChatGeneral_C->m_bVisible = 0;
			pFScene->m_pChatParty_C->m_bVisible = 0;
			pFScene->m_pChatWhisper_C->m_bVisible = 0;
			pFScene->m_pChatGuild_C->m_bVisible = 0;
			pFScene->m_pKingdomText->m_bVisible = 0;
			pFScene->m_pKingDomGuild->m_bVisible = 0;
			pFScene->m_pKingDomGuild_C->m_bVisible = 0;
		}
	}
	else
	{
		if(pFScene->m_pChatPanel)
		{
			pFScene->m_pChatPanel->m_bVisible = 1;
			pFScene->m_pChatList->m_bVisible = 1;
			pFScene->m_pChatListnotice->m_bVisible = 1;
			pFScene->m_pChatBack->m_bVisible = 1;
			pFScene->m_pChatGeneral->m_bVisible = 1;
			pFScene->m_pChatParty->m_bVisible = 1;
			pFScene->m_pChatWhisper->m_bVisible = 1;
			pFScene->m_pChatGuild->m_bVisible = 1;
			pFScene->m_pChatGeneral_C->m_bVisible = 1;
			pFScene->m_pChatParty_C->m_bVisible = 1;
			pFScene->m_pChatWhisper_C->m_bVisible = 1;
			pFScene->m_pChatGuild_C->m_bVisible = 1;
			pFScene->m_pKingdomText->m_bVisible = 1;
			pFScene->m_pKingDomGuild->m_bVisible = 1;
			pFScene->m_pKingDomGuild_C->m_bVisible = 1;
		}

	}
	 
	SPanel::SetVisible(bVisible);

	auto soundManager = g_pSoundManager;
	if (bVisible == 1 && bSound == 1 && soundManager != nullptr)
	{
		auto soundData = soundManager->GetSoundData(51);
		if (soundData)
			soundData->Play();
	}
}