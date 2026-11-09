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


int TMFieldScene::OnPacketRecaptcha(MSG_pRecaptcha* pStd)
{
	strcpy(_HudControl.Object_Human.reCaptcha.Pass, pStd->Pass);
	_HudControl.Object_Human.reCaptcha.Code = pStd->Code;
	_HudControl.Object_Human.reCaptcha.Parm = pStd->Parm;

	UpdateRecaptcha();

	return 1;
}

void TMFieldScene::UpdateRecaptcha()
{
	auto Captcha = _HudControl.Object_Human.reCaptcha;

	m_PanelreCaptcha->m_bVisible = true;

	auto recaptcha = (SPanel*)m_pControlContainer->FindControl(15761);
	auto recaptcha2 = (SPanel*)m_pControlContainer->FindControl(15762);
	auto recaptcha3 = (SPanel*)m_pControlContainer->FindControl(15763);
	auto recaptcha4 = (SPanel*)m_pControlContainer->FindControl(15764);



	int reCATPCHA = atoi(_HudControl.Object_Human.reCaptcha.Pass);
	int digits[4];

	digits[0] = reCATPCHA / 1000;
	digits[1] = (reCATPCHA % 1000) / 100;
	digits[2] = (reCATPCHA % 100) / 10;
	digits[3] = reCATPCHA % 10;

	int textureIndices[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	for (int i = 0; i < 4; i++) {
		int digit = digits[i];
		switch (i) {
		case 0:
			recaptcha->m_GCPanel.nTextureSetIndex = 139;
			recaptcha->m_GCPanel.nTextureIndex = textureIndices[digit];
			recaptcha->m_bVisible = true;
			break;
		case 1:
			recaptcha2->m_GCPanel.nTextureSetIndex = 139;
			recaptcha2->m_GCPanel.nTextureIndex = textureIndices[digit];
			recaptcha2->m_bVisible = true;
			break;
		case 2:
			recaptcha3->m_GCPanel.nTextureSetIndex = 139;
			recaptcha3->m_GCPanel.nTextureIndex = textureIndices[digit];
			recaptcha3->m_bVisible = true;
			break;
		case 3:
			recaptcha4->m_GCPanel.nTextureSetIndex = 139;
			recaptcha4->m_GCPanel.nTextureIndex = textureIndices[digit];
			recaptcha4->m_bVisible = true;
			break;
		}
	}
	auto Editsenha = (SEditableText*)m_pControlContainer->FindControl(15765);
	Editsenha->SetText((char*)"");
	m_pControlContainer->SetFocusedControl(0);
	Editsenha->SetTextColor(Yellow);

	m_pInvenPanel->m_bVisible = false;

	if (Captcha.Code == 2)
	{
		m_PanelreCaptcha->m_bVisible = false;

	}


}

void TMFieldScene::reCaptchaConfirm(int handle)
{
	switch (handle)
	{
	case 15766: //Confirmar
	{
		auto txsenha = (SEditableText*)m_pControlContainer->FindControl(15765);

		MSG_pRecaptcha Pack;
		memset(&Pack, 0, sizeof(Pack));

		Pack.Header.Type = 5269;
		Pack.Header.ID = g_pObjectManager->m_dwCharID;
		Pack.Header.Size = sizeof(Pack);
		Pack.Code = 1;
		strcpy(Pack.Pass, txsenha->GetText());

		SendOneMessage((char*)&Pack, sizeof(Pack));

		/*	m_PanelreCaptcha->m_bVisible = false;*/


	} break;



	}
}

