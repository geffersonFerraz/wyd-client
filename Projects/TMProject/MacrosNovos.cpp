#include "pch.h"
#include "TMFieldScene.h"
#include "TMGlobal.h"
#include "TMLog.h"
#include "dsutil.h"
#include "MacroMsg.h"
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
#include <regex>
#include <windows.h>
#include <iostream>
#include <io.h>
#include <thread>
#include <chrono>

int PossX;
int PossY;


bool MacroUP = false;
bool AguaMacroN = false;
bool AguaMacroM = false;
bool AguaMacroA = false;

int needupdate = 0;

int countAguaN = 0;
int countAguaM = 0;
int countAguaA = 0;

void TMFieldScene::FiltroPosition() {
	PossX = (int)m_pMyHuman->m_vecPosition.x;
	PossY = (int)m_pMyHuman->m_vecPosition.y;
}

DWORD WINAPI ThreadMacro(LPVOID lpParameter) {

	while (MacroUP) {

		if (!MacroUP)
		{
			AguaMacroN = false;
			AguaMacroM = false;
			AguaMacroA = false;
			CloseHandle(lpParameter);
		}

		if (MacroUP)
		{
			WORD X = PossX / 128;
			WORD Y = PossY / 128;
			WORD Xm = PossX;
			WORD Ym = PossY;
			int slot = -1;
			auto mob = g_pObjectManager->m_stMobData;

			if (Xm > 1954 && Ym > 1760 && Xm < 1976 && Ym < 1776)
			{
				int havePerga = 0;
				for (int i = 0; i < MAX_CARRY; i++)
				{
					auto item = mob.Carry[i].sIndex;

					if (item == 0)
						continue;

					if (item == 3182)
					{
						havePerga++;
						break;
					}
					if (item == 777)
					{
						havePerga++;
						break;
					}
					if (item == 3173)
					{
						havePerga++;
						break;
					}
				}

				if (havePerga == FALSE)
				{
					MacroUP = false;
					AguaMacroN = false;
					AguaMacroM = false;
					AguaMacroA = false;
					CloseHandle(lpParameter);
				}
			}

			if (Xm <= 1975 && Xm >= 1956 && Ym <= 1775 && Ym >= 1766)
			{
				int Next[] = { 3182, 777,3173 };
				for (int i = 0; i < MAX_CARRY; i++)
				{
					auto item = mob.Carry[i].sIndex;
					for (int s = 0; s < 3; s++)
					{
						for (int p = 0; p < 8; p++)
						{
							if (item == Next[s] + p)
							{
								slot = i;
								break;
							}
						}
						if (slot != -1)
							break;
					}
				}
			}

			if ((X == 8 && Y == 27) || (X == 9 && Y == 28) || (X == 10 && Y == 27))
			{
				int Next[] = { 3183, 778, 3174 };
				for (int i = 0; i < MAX_CARRY; i++)
				{
					auto item = mob.Carry[i].sIndex;
					for (int s = 0; s < 3; s++)
					{
						for (int p = 0; p < 8; p++)
						{
							if (item == Next[s] + p)
							{
								slot = i;
								break;
							}
						}
						if (slot != -1)
							break;
					}
				}
			}

			if (Xm <= 1976 && Xm >= 1955 && Ym <= 1780 && Ym >= 1764)
			{
				int Next[] = { 3183, 778, 3174 };
				for (int i = 0; i < MAX_CARRY; i++)
				{
					auto item = mob.Carry[i].sIndex;
					for (int s = 0; s < 3; s++)
					{
						for (int p = 0; p < 8; p++)
						{
							if (item == Next[s] + p)
							{
								slot = i;
								break;
							}
						}
						if (slot != -1)
							break;
					}
				}

				for (int i = 0; i < MAX_CARRY; i++)
				{
					if (slot != -1)
						break;

					for (int p = 0; p < 10; p++)
					{
						auto item = mob.Carry[i].sIndex;
						if (item == 3182 || item == 777 || item == 3173)
						{
							slot = i;
							break;
						}
					}
				}
			}

			if (slot != -1)
			{
				MSG_UseItem p;
				memset(&p, 0, sizeof(MSG_UseItem));
				p.Header.Type = 0x373;
				p.Header.Size = sizeof p;
				p.Header.ID = g_pObjectManager->m_dwCharID;
				p.GridX = PossX;
				p.GridY = PossY;
				p.SourType = 1;
				p.SourPos = slot;
				SendOneMessage((char*)&p, sizeof(p));
			}
		}

		this_thread::sleep_for(chrono::milliseconds(3000));
	}

	return 0;
}

void UseItem_packet(int slot)
{
	MSG_UseItem p;
	memset(&p, 0, sizeof(MSG_UseItem));
	p.Header.Type = 0x373;
	p.Header.Size = sizeof p;
	p.Header.ID = g_pObjectManager->m_dwCharID;
	p.GridX = PossX;
	p.GridY = PossY;
	p.SourType = 1;
	p.SourPos = slot;
	SendOneMessage((char*)&p, sizeof(p));
}

void TMFieldScene::updateaguaMacro()
{
	needupdate = 0;
	if (m_pMacroAguaPanel->IsVisible() == 1)
	{
		auto mob = g_pObjectManager->m_stMobData;

		countAguaN = 0;
		int AguaN = 3173;
		for (int i = 0; i < MAX_CARRY - 4; i++)
		{
			auto item = mob.Carry[i].sIndex;

			if (item != AguaN)
				continue;

			if (mob.Carry[i].stEffect[0].cEffect == 61)
			{
				countAguaN += mob.Carry[i].stEffect[0].cValue;
			}

			if (mob.Carry[i].stEffect[1].cEffect == 61)
			{
				countAguaN += mob.Carry[i].stEffect[1].cValue;
			}

			if (mob.Carry[i].stEffect[2].cEffect == 61)
			{
				countAguaN += mob.Carry[i].stEffect[2].cValue;
			}
		}

		countAguaM = 0;
		int AguaM = 777;
		for (int i = 0; i < MAX_CARRY - 4; i++)
		{
			auto item = mob.Carry[i].sIndex;

			if (item != AguaM)
				continue;

			if (mob.Carry[i].stEffect[0].cEffect == 61)
			{
				countAguaM += mob.Carry[i].stEffect[0].cValue;
			}

			if (mob.Carry[i].stEffect[1].cEffect == 61)
			{
				countAguaM += mob.Carry[i].stEffect[1].cValue;
			}

			if (mob.Carry[i].stEffect[2].cEffect == 61)
			{
				countAguaM += mob.Carry[i].stEffect[2].cValue;
			}
		}

		countAguaA = 0;
		int AguaA = 3182;
		for (int i = 0; i < MAX_CARRY - 4; i++)
		{
			auto item = mob.Carry[i].sIndex;

			if (item != AguaA)
				continue;

			if (mob.Carry[i].stEffect[0].cEffect == 61)
			{
				countAguaA += mob.Carry[i].stEffect[0].cValue;
			}

			if (mob.Carry[i].stEffect[1].cEffect == 61)
			{
				countAguaA += mob.Carry[i].stEffect[1].cValue;
			}

			if (mob.Carry[i].stEffect[2].cEffect == 61)
			{
				countAguaA += mob.Carry[i].stEffect[2].cValue;
			}
		}

		auto QuantidadeAguaN = (SText*)m_pControlContainer->FindControl(255706);
		QuantidadeAguaN->SetText(strFmt("%d", countAguaN), 0);


		auto QuantidadeAguaM = (SText*)m_pControlContainer->FindControl(255707);
		QuantidadeAguaM->SetText(strFmt("%d", countAguaM), 0);


		auto QuantidadeAguaA = (SText*)m_pControlContainer->FindControl(255708);
		QuantidadeAguaA->SetText(strFmt("%d", countAguaA), 0);


		if (AguaMacroN)
		{
			auto StatusAguaN = (SText*)m_pControlContainer->FindControl(255712);
			StatusAguaN->SetText(strFmt("ON", 0), 0);
		}
		else {
			auto StatusAguaN = (SText*)m_pControlContainer->FindControl(255712);
			StatusAguaN->SetText(strFmt("OFF", 0), 0);
		}

		if (AguaMacroM)
		{
			auto StatusAguaM = (SText*)m_pControlContainer->FindControl(255713);
			StatusAguaM->SetText(strFmt("ON", 0), 0);
		}
		else {
			auto StatusAguaM = (SText*)m_pControlContainer->FindControl(255713);
			StatusAguaM->SetText(strFmt("OFF", 0), 0);
		}

		if (AguaMacroA)
		{
			auto StatusAguaA = (SText*)m_pControlContainer->FindControl(255714);
			StatusAguaA->SetText(strFmt("ON", 0), 0);
		}
		else {
			auto StatusAguaA = (SText*)m_pControlContainer->FindControl(255714);
			StatusAguaA->SetText(strFmt("OFF", 0), 0);
		}

		auto Painelinfo = (SText*)m_pControlContainer->FindControl(255715);
		Painelinfo->SetText(strFmt("Selecione um pergaminho", 0), 0);

		auto Paineltitulo = (SText*)m_pControlContainer->FindControl(255716);
		Paineltitulo->SetText(strFmt("Macro de Pergaminho Elemental", 0), 0);

	}
}

char* strFmt(const char* str, ...)
{
	static char buffer[512] = { 0, };
	va_list va;
	va_start(va, str);
	vsprintf_s(buffer, str, va);
	va_end(va);
	return buffer;
}

void TMFieldScene::PainelMacroPerga() {

	if (m_pMacroAguaPanel->IsVisible() == 0)
	{
		m_pMacroAguaPanel->SetVisible(1);

		auto mob = g_pObjectManager->m_stMobData;

		countAguaN = 0;
		int AguaN = 3173;
		for (int i = 0; i < MAX_CARRY - 4; i++)
		{
			auto item = mob.Carry[i].sIndex;

			if (item != AguaN)
				continue;

			if (mob.Carry[i].stEffect[0].cEffect == 61)
			{
				countAguaN += mob.Carry[i].stEffect[0].cValue;
			}

			if (mob.Carry[i].stEffect[1].cEffect == 61)
			{
				countAguaN += mob.Carry[i].stEffect[1].cValue;
			}

			if (mob.Carry[i].stEffect[2].cEffect == 61)
			{
				countAguaN += mob.Carry[i].stEffect[2].cValue;
			}
		}

		countAguaM = 0;
		int AguaM = 777;
		for (int i = 0; i < MAX_CARRY - 4; i++)
		{
			auto item = mob.Carry[i].sIndex;

			if (item != AguaM)
				continue;

			if (mob.Carry[i].stEffect[0].cEffect == 61)
			{
				countAguaM += mob.Carry[i].stEffect[0].cValue;
			}

			if (mob.Carry[i].stEffect[1].cEffect == 61)
			{
				countAguaM += mob.Carry[i].stEffect[1].cValue;
			}

			if (mob.Carry[i].stEffect[2].cEffect == 61)
			{
				countAguaM += mob.Carry[i].stEffect[2].cValue;
			}
		}

		countAguaA = 0;
		int AguaA = 3182;
		for (int i = 0; i < MAX_CARRY - 4; i++)
		{
			auto item = mob.Carry[i].sIndex;

			if (item != AguaA)
				continue;

			if (mob.Carry[i].stEffect[0].cEffect == 61)
			{
				countAguaA += mob.Carry[i].stEffect[0].cValue;
			}

			if (mob.Carry[i].stEffect[1].cEffect == 61)
			{
				countAguaA += mob.Carry[i].stEffect[1].cValue;
			}

			if (mob.Carry[i].stEffect[2].cEffect == 61)
			{
				countAguaA += mob.Carry[i].stEffect[2].cValue;
			}
		}

		auto QuantidadeAguaN = (SText*)m_pControlContainer->FindControl(255706);
		QuantidadeAguaN->SetText(strFmt("%d", countAguaN), 0);


		auto QuantidadeAguaM = (SText*)m_pControlContainer->FindControl(255707);
		QuantidadeAguaM->SetText(strFmt("%d", countAguaM), 0);


		auto QuantidadeAguaA = (SText*)m_pControlContainer->FindControl(255708);
		QuantidadeAguaA->SetText(strFmt("%d", countAguaA), 0);


		if (AguaMacroN)
		{
			auto StatusAguaN = (SText*)m_pControlContainer->FindControl(255712);
			StatusAguaN->SetText(strFmt("ON", 0), 0);
		}
		else {
			auto StatusAguaN = (SText*)m_pControlContainer->FindControl(255712);
			StatusAguaN->SetText(strFmt("OFF", 0), 0);
		}

		if (AguaMacroM)
		{
			auto StatusAguaM = (SText*)m_pControlContainer->FindControl(255713);
			StatusAguaM->SetText(strFmt("ON", 0), 0);
		}
		else {
			auto StatusAguaM = (SText*)m_pControlContainer->FindControl(255713);
			StatusAguaM->SetText(strFmt("OFF", 0), 0);
		}

		if (AguaMacroA)
		{
			auto StatusAguaA = (SText*)m_pControlContainer->FindControl(255714);
			StatusAguaA->SetText(strFmt("ON", 0), 0);
		}
		else {
			auto StatusAguaA = (SText*)m_pControlContainer->FindControl(255714);
			StatusAguaA->SetText(strFmt("OFF", 0), 0);
		}

		auto Painelinfo = (SText*)m_pControlContainer->FindControl(255715);
		Painelinfo->SetText(strFmt("Selecione um pergaminho", 0), 0);

		auto Paineltitulo = (SText*)m_pControlContainer->FindControl(255716);
		Paineltitulo->SetText(strFmt("Macro de Pergaminho Elemental", 0), 0);
	}

	else if (m_pMacroAguaPanel->IsVisible() == 1)
		m_pMacroAguaPanel->SetVisible(0);
}