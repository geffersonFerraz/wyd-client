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
#include <WinInet.h>
#include "TMDroplistAndFilter.h"
  
TMSlotMove::TMSlotMove(float inX, float inY, float inWidth, float inHeight, unsigned int MaxSlot)
    : SPanel(-45, inX, inY, inWidth, inHeight, 0xAA000000, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH),
    nMaxSlot(MaxSlot),
    m_spacingX(10.0f),
    m_spacingY(5.0f)
{    

    m_pSlot = new SPanel(428, 10.0f, 0.0f, inWidth, inHeight, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
    if (m_pSlot)
        AddChild(m_pSlot);
}
 
void TMSlotMove::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
    unsigned int dwServerTime = g_pTimerManager->GetServerTime();
    auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

    m_pSlot->FrameMove(dwServerTime);
    if (pFScene->m_pDropList && pFScene->m_pDropList->m_bVisible == 0)
        m_pSlot->SetVisible(0);
	 
    SPanel::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);
}


TMSlotMove::~TMSlotMove()
{ 
}  

void TMSlotMove::SetVisible(int bVisible)
{
     

    SPanel::SetVisible(bVisible);
}

void TMSlotMove::SlotOnMouseEvent(unsigned int idwControlID, unsigned int slotOver, unsigned int indice)
{
	TMFieldScene* Scene = static_cast<TMFieldScene*>(g_pCurrentScene);

	auto pGrid = (SGridControl*)Scene->m_pControlContainer->FindControl(idwControlID);
	if (idwControlID == 478473)
	{
		Scene->m_pSlotEffect->m_pSlot->SetTextureSetIndex(428);
		Scene->m_pSlotEffect->m_pSlot->m_GCPanel.nTextureIndex = 1;
		auto gridcurrent = pGrid->m_pItemList[indice];
		if (gridcurrent)
		{
			if (gridcurrent->m_bOver)
			{
				Scene->m_pSlotEffect->nIndice = gridcurrent->m_pItem->sIndex;

				// Calcula a largura e altura da célula no grid
				float nCellVWidth = (float)(pGrid->m_nWidth / (float)pGrid->m_nColumnGridCount);
				float nCellVHeight = (float)(pGrid->m_nHeight / (float)pGrid->m_nRowGridCount);

				auto nResIndex = m_pConfig.Config[0];
				switch (nResIndex)
				{

				case 1:
				{
					SetPos(150.0f * RenderDevice::m_fWidthRatio, 10.0f * RenderDevice::m_fHeightRatio);

					// Ajuste o tamanho das células com base na relação de aspecto do dispositivo
					nCellVWidth = 31.0f * RenderDevice::m_fWidthRatio;
					nCellVHeight = 31.0f * RenderDevice::m_fHeightRatio;

					// Espaçamento entre células (ajuste conforme necessário)
					float espaco = 7.5333f * RenderDevice::m_fWidthRatio;

					// Calcula a posição da célula no grid (X e Y)
					float nCellX = (float)(indice % pGrid->m_nColumnGridCount);
					float nCellY = (float)(indice / pGrid->m_nColumnGridCount);

					// Atualiza o tamanho e a posição do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->SetSize(nCellVWidth, nCellVHeight);
					Scene->m_pSlotEffect->m_pSlot->SetPos(
						pGrid->m_nPosX + nCellX * (nCellVWidth + espaco),
						pGrid->m_nPosY + nCellY * (nCellVHeight + espaco)
					);

					// Atualiza a visibilidade e o estado do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->m_bOver = gridcurrent->m_bOver;
					Scene->m_pSlotEffect->m_pSlot->SetVisible(slotOver);
					Scene->m_pSlotEffect->SetVisible(slotOver);
					break;
				}


				case 2:
				{
					SetPos(150.0f * RenderDevice::m_fWidthRatio, 10.0f * RenderDevice::m_fHeightRatio);

					// Ajuste o tamanho das células com base na relação de aspecto do dispositivo
					nCellVWidth = 31.0f * RenderDevice::m_fWidthRatio;
					nCellVHeight = 31.0f * RenderDevice::m_fHeightRatio;

					// Espaçamento entre células (ajuste conforme necessário)
					float espaco = 7.5333f * RenderDevice::m_fWidthRatio;

					// Calcula a posição da célula no grid (X e Y)
					float nCellX = (float)(indice % pGrid->m_nColumnGridCount);
					float nCellY = (float)(indice / pGrid->m_nColumnGridCount);

					// Atualiza o tamanho e a posição do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->SetSize(nCellVWidth, nCellVHeight);
					Scene->m_pSlotEffect->m_pSlot->SetPos(
						pGrid->m_nPosX + nCellX * (nCellVWidth + espaco),
						pGrid->m_nPosY + nCellY * (nCellVHeight + espaco)
					);

					// Atualiza a visibilidade e o estado do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->m_bOver = gridcurrent->m_bOver;
					Scene->m_pSlotEffect->m_pSlot->SetVisible(slotOver);
					Scene->m_pSlotEffect->SetVisible(slotOver);
					break;
				}

				case 3:
				{
					SetPos(150.0f * RenderDevice::m_fWidthRatio, 10.0f * RenderDevice::m_fHeightRatio);

					// Ajuste o tamanho das células com base na relação de aspecto do dispositivo
					nCellVWidth = 31.0f * RenderDevice::m_fWidthRatio;
					nCellVHeight = 31.0f * RenderDevice::m_fHeightRatio;

					// Espaçamento entre células (ajuste conforme necessário)
					float espaco = 7.5333f * RenderDevice::m_fWidthRatio;

					// Calcula a posição da célula no grid (X e Y)
					float nCellX = (float)(indice % pGrid->m_nColumnGridCount);
					float nCellY = (float)(indice / pGrid->m_nColumnGridCount);

					// Atualiza o tamanho e a posição do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->SetSize(nCellVWidth, nCellVHeight);
					Scene->m_pSlotEffect->m_pSlot->SetPos(
						pGrid->m_nPosX + nCellX * (nCellVWidth + espaco),
						pGrid->m_nPosY + nCellY * (nCellVHeight + espaco)
					);

					// Atualiza a visibilidade e o estado do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->m_bOver = gridcurrent->m_bOver;
					Scene->m_pSlotEffect->m_pSlot->SetVisible(slotOver);
					Scene->m_pSlotEffect->SetVisible(slotOver);
					break;
				}

				case 4:
				{
					SetPos(150.0f * RenderDevice::m_fWidthRatio, 10.0f * RenderDevice::m_fHeightRatio);

					// Ajuste o tamanho das células com base na relação de aspecto do dispositivo
					nCellVWidth = 31.0f * RenderDevice::m_fWidthRatio;
					nCellVHeight = 31.0f * RenderDevice::m_fHeightRatio;

					// Espaçamento entre células (ajuste conforme necessário)
					float espaco = 7.5333f * RenderDevice::m_fWidthRatio;

					// Calcula a posição da célula no grid (X e Y)
					float nCellX = (float)(indice % pGrid->m_nColumnGridCount);
					float nCellY = (float)(indice / pGrid->m_nColumnGridCount);

					// Atualiza o tamanho e a posição do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->SetSize(nCellVWidth, nCellVHeight);
					Scene->m_pSlotEffect->m_pSlot->SetPos(
						pGrid->m_nPosX + nCellX * (nCellVWidth + espaco),
						pGrid->m_nPosY + nCellY * (nCellVHeight + espaco)
					);

					// Atualiza a visibilidade e o estado do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->m_bOver = gridcurrent->m_bOver;
					Scene->m_pSlotEffect->m_pSlot->SetVisible(slotOver);
					Scene->m_pSlotEffect->SetVisible(slotOver);
					break;
				}
				case 5:
				{
					SetPos(153.0f * RenderDevice::m_fWidthRatio, 10.0f * RenderDevice::m_fHeightRatio);

					// Ajuste o tamanho das células com base na relação de aspecto do dispositivo
					nCellVWidth = 31.0f * RenderDevice::m_fWidthRatio;
					nCellVHeight = 31.0f * RenderDevice::m_fHeightRatio;

					// Espaçamento entre células (ajuste conforme necessário)
					float espaco = 8.466f * RenderDevice::m_fWidthRatio;

					// Calcula a posição da célula no grid (X e Y)
					float nCellX = (float)(indice % pGrid->m_nColumnGridCount);
					float nCellY = (float)(indice / pGrid->m_nColumnGridCount);

					// Atualiza o tamanho e a posição do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->SetSize(nCellVWidth, nCellVHeight);
					Scene->m_pSlotEffect->m_pSlot->SetPos(
						pGrid->m_nPosX + nCellX * (nCellVWidth + espaco),
						pGrid->m_nPosY + nCellY * (nCellVHeight + espaco)
					);

					// Atualiza a visibilidade e o estado do efeito de slot
					Scene->m_pSlotEffect->m_pSlot->m_bOver = gridcurrent->m_bOver;
					Scene->m_pSlotEffect->m_pSlot->SetVisible(slotOver);
					Scene->m_pSlotEffect->SetVisible(slotOver);
					break;
				}

				};
			}
		}

	}
}