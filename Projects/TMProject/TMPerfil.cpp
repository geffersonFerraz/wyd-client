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



void TMFieldScene::Profile(int handle)
{
	switch (handle)
	{
	case 916:
	{
		auto paneltarget = (SPanel*)m_pControlContainer->FindControl(640);

		paneltarget->m_bVisible = false;


		if (m_pPGTOver && m_pPGTOver->m_dwID && m_pPGTOver->m_dwID < 1000)
		{
			pMsgSignal Signal{};
			Signal.Header.Type = 0x487;
			Signal.Header.ID = g_pObjectManager->m_dwCharID;
			Signal.Header.Size = sizeof Signal;

			Signal.Value = m_pPGTOver->m_dwID;

			SendOneMessage((char*)&Signal, sizeof(Signal));

		}
		//ProfileUpdate();
	}
	break;
	}

}

void TMFieldScene::ProfileUpdate(p487* request)
{
	auto Panel = (SPanel*)m_pControlContainer->FindControl(148700);
	Panel->SetVisible(TRUE);

	auto Label1 = (SText*)m_pControlContainer->FindControl(148719);
	auto Label2 = (SText*)m_pControlContainer->FindControl(148736);

	Label1->SetTextColor(Yellow);
	Label2->SetTextColor(Yellow);


	auto LabelName = (SText*)m_pControlContainer->FindControl(148710);
	auto LabelLevel = (SText*)m_pControlContainer->FindControl(148717);
	auto LabelGuild = (SText*)m_pControlContainer->FindControl(148712);    
	auto LabelCapa = (SPanel*)m_pControlContainer->FindControl(148718);
	auto Evolution = (SText*)m_pControlContainer->FindControl(148711);


	char text[20] = { 0, };
	sprintf(text, "%s", request->Name);
	LabelName->SetText(text, 0);

	sprintf(text, "%d", request->Level);
	LabelLevel->SetText(text, 0);

	sprintf(text, "%s", request->GuildName);
	LabelGuild->SetText(text, 0);
	  
	const char* classType[5] = { "   none", "   Mortal", "    Arch", " Celestial", " SubCelestial" };

	sprintf(text, "%s", classType[request->Evolution]);
	Evolution->SetText(text, 0);

	if (request->Mantle)
	{
		int capeId = g_pItemList[request->Mantle].nIndexTexture;

		LabelCapa->m_GCPanel.nTextureSetIndex = 65;
		LabelCapa->m_GCPanel.nTextureIndex = capeId;

		LabelCapa->m_bVisible = 1;
	}
	else
	{
		LabelCapa->m_bVisible = 0;

	}

	for (size_t i = 1; i <= 18; i++)
	{ 
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148720 + i);
		if (_slot)
		{
			_slot->Empty();
		} 
	} 


	if (request->Equip[1].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148720);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[1], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[2].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148721);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[2], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[3].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148722);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[3], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[4].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148723);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[4], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[5].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148724);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[5], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[6].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148725);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[6], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[7].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148726);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[7], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[12].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148727);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[12], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[14].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148728);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[14], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[18].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148729);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[18], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[8].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148730);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[8], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[10].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148731);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[10], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[9].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148733);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[9], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[11].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148734);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[11], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	}
	if (request->Equip[13].sIndex > 40)
	{
		auto _slot = (SGridControl*)m_pControlContainer->FindControl(148732);

		STRUCT_ITEM* item = new STRUCT_ITEM;
		memcpy(item, &request->Equip[13], sizeof(STRUCT_ITEM));
		_slot->AddItem(new SGridControlItem(nullptr, item, 0.0f, 0.0f), 0, 0);
		_slot->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	} 

}