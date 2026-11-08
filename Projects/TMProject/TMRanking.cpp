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


void TMFieldScene::RankingUpdate(int scene = 0) {
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return;
    std::string sceneName[3] = { "Level", "Batalha Real", "PvP" };

    /*auto Label = (SText*)m_pControlContainer->FindControl(6005);

    char temps[127]{};
    sprintf(temps, "Ranking de %s", sceneName[scene].c_str());
    Label->SetText(temps, 0);*/

    auto Table = _HudControl.Ranking;

    m_pRankingListBox = (SListBox*)m_pControlContainer->FindControl(6015);
    auto pGuildList = m_pRankingListBox;

    // Deleta itens antigos da lista
    while (pGuildList->m_nNumItem > 0) {
        auto pGuildItem = (SListBoxRankingItem*)m_pRankingListBox->m_pItemList[0];
        m_pRankingListBox->DeleteItem(pGuildItem);
    }

    int rankCounter = 1;  // Contador de rank separado

    for (int i = 0; i < 15; i++) {
        if (Table.Rank[i].Level > 401)
            continue; // Pula jogadores com nível maior que 401

        char temp[4096];
        sprintf(temp, "%d_________%s", rankCounter, Table.Rank[i].Player);
        rankCounter++;  // Incrementa o contador de rank

        auto pGuildItem = new SListBoxRankingItem(temp, White, 0.0f, 2.0f, 114.0f, 15.0f, Table.Rank[i].Kingdom,
            Table.Rank[i].Level, Table.Rank[i].Class, Table.Rank[i].Evolution, Table.Rank[i].Guild, Table.Rank[i].Frags, Table.Rank[i].GuildName);

        pGuildList->AddItem(pGuildItem);
    }

    if (pGuildList->m_pScrollBar)
        pGuildList->m_pScrollBar->SetCurrentPos(0);
}