#include "pch.h"
#include "SControlContainer.h"
#include "SControl.h"
#include "SGrid.h"
#include "UIBinary.h"
#include "TMGround.h"
#include "TMSky.h"
#include "TMSun.h"
#include "TMLight.h"
#include "TMHuman.h"
#include "TMObject.h"
#include "TMCamera.h"
#include "TMObjectContainer.h"
#include "TMItem.h"
#include "TMGlobal.h"
#include "TMLog.h"
#include "TMScene.h"
#include "TMFieldScene.h"
#include "TMEffectFirework.h"
#include "TMSnow.h"
#include "TMRain.h"

TMScene::TMScene() : TreeNode(0)
{
	m_eSceneType = ESCENE_TYPE::ESCENE_NONE;
	m_pControlContainer = nullptr;
	m_pExtraContainer = nullptr;
	m_pGround = nullptr;
	m_pMouseOverHuman = nullptr;
	m_pMouseOverItem = nullptr;
	m_sPlayDemo = -1;
	m_nCurrentGroundIndex = 0;
	m_dwStartCamTime = 0;
	g_ClipFar = 70.0f;
	SControl::m_dwStaticID = 0;
	m_nAdjustTime = 0;
	m_dwInitTime = 0;
	n_bPrtScreen = 0;
	m_pMyHuman = nullptr;
	m_pSky = nullptr;
	m_pSun = nullptr;
	m_pHumanContainer = nullptr;
	m_pItemContainer = nullptr;
	m_pAlphaNative = nullptr;
	m_pDescPanel = nullptr;
	m_pTextBillMsg = nullptr;
	m_bShowNameLabel = 0;
	m_bCriticalError = 0;
	m_bAutoRun = 0;
	m_bReverse = 0;
	TMLight::m_dwBaseLightIndex = 2;
	m_pTextIMEDesc = 0;

	for (int nImeCount = 0; nImeCount < 10; ++nImeCount)
		m_pTextCandidate[nImeCount] = 0;

	m_pTextCompose = nullptr;
	m_pTextComposeB = nullptr;
	m_pTextReadCompose = nullptr;
	m_pTextReadComposeB = nullptr;
	m_dwDelayDisconnectTime = 0;
	m_bMsgRemoveServer = 0;

	for (int i = 0; i < 2; ++i)
	{
		m_pGroundList[i] = 0;
		m_pObjectContainerList[i] = 0;
	}

	m_pControlContainer = new SControlContainer(this);

	m_pExtraContainer = new TreeNode(0);

	AddChild(m_pExtraContainer);

	m_pEffectContainer = new TreeNode(0);

	AddChild(m_pEffectContainer);

	m_pShadeContainer = new TreeNode(0);

	AddChild(m_pShadeContainer);

	m_pGroundObjectContainer = new TreeNode(0);

	AddChild(m_pGroundObjectContainer);

	m_pHumanContainer = new TreeNode(0);

	AddChild(m_pHumanContainer);

	m_pTextBillMsg = new SText(-2, "ºô", 0xFFFFFFFF, 120.0f, 70.0f, 540.0f, 20.0f, 1, 0xAAFF0000, 1, 1);
	m_pTextBillMsg->m_bSelectEnable = 0;
	m_pTextBillMsg->SetVisible(0);

	m_pMessagePanel = new SMessagePanel("Message Panel", 160.0f, 80.0f, 480.0f, 24.0f, 2000);

	m_pMessagePanel->SetPos((((float)g_pDevice->m_dwScreenWidth - m_pMessagePanel->m_nWidth) * 0.5f),
		((float)g_pDevice->m_dwScreenHeight - m_pMessagePanel->m_nHeight) * 0.105f);



	m_pReconnectPanel = new SReconnectPanel("", 250.0f, 80.0f, g_pDevice->m_dwScreenWidth * RenderDevice::m_fWidthRatio, g_pDevice->m_dwScreenHeight * RenderDevice::m_fHeightRatio);


	if (m_pReconnectPanel)
	{

		m_pReconnectPanel->SetPos((((float)g_pDevice->m_dwScreenWidth - m_pReconnectPanel->m_nWidth) * 0.5f),
			((float)g_pDevice->m_dwScreenHeight - m_pReconnectPanel->m_nHeight) * 0.1f);

		m_pReconnectPanel->SetVisible(0);
	}






	m_pMessageBox = new SMessageBox(
		"Message Box",
		0,
		(((float)g_pDevice->m_dwScreenWidth / RenderDevice::m_fWidthRatio) - 256.0f) / 2.0f,
		(((float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fHeightRatio) - 152.0f) / 2.0f);

	m_pMessageBox2 = new SMessageBox(
		"Message Box",
		4,
		(((float)g_pDevice->m_dwScreenWidth / RenderDevice::m_fWidthRatio) - 256.0f) / 2.0f,
		(((float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fHeightRatio) - 152.0f) / 2.0f);

	for (int nImeCounta = 0; nImeCounta < 10; ++nImeCounta)
	{
		m_pTextCandidate[nImeCounta] = new SText(
			-2,
			"",
			0xFFFFFFFF,
			((float)g_pDevice->m_dwScreenWidth / RenderDevice::m_fWidthRatio) - 180.0f,
			(((float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fHeightRatio) - 161.0f) + (float)(16 * nImeCounta),
			179.0f,
			16.0f,
			1,
			0x9955AA55,
			1,
			0);
	}

	m_pTextCompose = new SText(
		-2,
		"",
		0xFFFFFFFF,
		0.0f,
		((float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fHeightRatio) - 36.0f,
		200.0f,
		16.0f,
		1,
		0xAA55AA55,
		1,
		0);

	m_pTextReadCompose = new SText(
		-2,
		"",
		0xFFFFFFFF,
		0.0f,
		((float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fHeightRatio) - 36.0f,
		200.0f,
		16.0f,
		1,
		0xAA55AA55,
		1,
		0);

	m_pTextComposeB = new SText(
		-2,
		"",
		0xFFFFFF00,
		0.0f,
		((float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fHeightRatio) - 36.0f,
		200.0f,
		16.0f,
		0,
		0xAA55AA55,
		1,
		0);

	m_pTextReadComposeB = new SText(
		-2,
		"",
		0xFFFFFF00,
		0.0f,
		((float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fHeightRatio) - 36.0f,
		200.0f,
		16.0f,
		0,
		0xAA55AA55,
		1,
		0);

	m_pTextCompose->SetVisible(0);
	m_pTextReadCompose->SetVisible(0);
	m_pTextComposeB->SetVisible(0);
	m_pTextReadComposeB->SetVisible(0);
	m_pMessagePanel->SetVisible(0, 1);
	m_pReconnectPanel->SetVisible(0);
	m_pMessageBox->SetVisible(0);
	m_pMessageBox2->SetVisible(0);

	if (m_pControlContainer)
		m_pMessageBox->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
	else
		m_pMessageBox->SetEventListener(nullptr);

	if (m_pControlContainer)
		m_pMessageBox2->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
	else
		m_pMessageBox2->SetEventListener(nullptr);

	m_pMessageBox->m_bModal = 1;
	m_pMessageBox2->m_bModal = 1;
	m_pMessagePanel->m_bSelectEnable = 0;
	m_pReconnectPanel->m_bSelectEnable = 0;

	m_pControlContainer->m_pModalControl[0] = dynamic_cast<SControl*>(m_pMessageBox);
	m_pControlContainer->m_pModalControl[4] = dynamic_cast<SControl*>(m_pMessageBox2);

	m_pControlContainer->AddItem(static_cast<SControl*>(m_pMessagePanel));
	m_pControlContainer->AddItem(static_cast<SControl*>(m_pMessageBox));
	m_pControlContainer->AddItem(static_cast<SControl*>(m_pMessageBox2));
	m_pControlContainer->AddItem(static_cast<SControl*>(m_pTextBillMsg));
	m_pControlContainer->AddItem(static_cast<SControl*>(m_pReconnectPanel));

	for (int nImeCountb = 0; nImeCountb < 10; ++nImeCountb)
	{
		m_pTextCandidate[nImeCountb]->SetVisible(0);

		m_pControlContainer->AddItem(static_cast<SControl*>(m_pTextCandidate[nImeCountb]));
	}

	m_pControlContainer->AddItem(static_cast<SControl*>(m_pTextComposeB));
	m_pControlContainer->AddItem(static_cast<SControl*>(m_pTextReadComposeB));
	m_pControlContainer->AddItem(static_cast<SControl*>(m_pTextCompose));
	m_pControlContainer->AddItem(static_cast<SControl*>(m_pTextReadCompose));

	float fHeight = (float)g_pDevice->m_dwScreenHeight - 20.0f;

	m_pAlphaNative = new SText(-2, "Ch", 0xFFFFFFDD, 0.0f, fHeight, 28.0f, 16.0f, 1, 0xAA000077, 1, 0);
	m_pAlphaNative->SetVisible(0);

	m_pControlContainer->AddItem(static_cast<SControl*>(m_pAlphaNative));

	m_pTextIMEDesc = new SText(-2, "IME Desc", 0xFFFFFFDD, 30.0f, fHeight, 180.0f, 16.0f, 1, 0xAA000077, 1, 0);
	m_pTextIMEDesc->SetVisible(0);

	m_pControlContainer->AddItem(static_cast<SControl*>(m_pTextIMEDesc));

	if (g_pEventTranslator)
	{
		if (g_pEventTranslator->IsNative())
		{
			char tmp[] = "Ch";
			m_pAlphaNative->SetText(tmp, 0);

			char szDesc[256]{};

			ImmGetDescriptionA(GetKeyboardLayout(0), szDesc, sizeof(szDesc));

			m_pTextIMEDesc->SetText(szDesc, 0);
			m_pTextIMEDesc->SetSize((float)(8 * strlen(szDesc)) + 16.0f, 16.0f);
		}
		else
		{
			char tmp[] = "En";
			m_pAlphaNative->SetText(tmp, 0);
		}
	}

	memset(m_HeightMapData, 0, sizeof(m_HeightMapData));
	memset(m_GateMapData, 0, sizeof(m_GateMapData));

	BASE_ApplyAttribute((char*)m_HeightMapData, 256);

	for (int i = 0; i < 32; ++i)
		m_TargetAffect[i] = 0;
}

TMScene::~TMScene()
{
	g_pTimerManager->m_dwDelayTime = 20000;

	unsigned int dwServerTime = g_pTimerManager->GetServerTime();

	g_pObjectManager->EffectFrameMove(m_pEffectContainer, dwServerTime);

	SAFE_DELETE(m_pHumanContainer);

	SAFE_DELETE(m_pItemContainer);

	SAFE_DELETE(m_pControlContainer);

	g_pObjectManager->m_pPreviousScene = nullptr;
	g_pObjectManager->CleanUp();

	m_pGround = nullptr;

	g_nUnDelMobCount = 0;

	g_pTimerManager->m_dwDelayTime = 0;
}

SControlContainer* TMScene::GetCtrlContainer()
{
	return m_pControlContainer;
}

int TMScene::LoadRC(const char* szFileName)
{
	if (!m_pControlContainer)
		m_pControlContainer = new SControlContainer(this);

	char szBinFileName[128]{};

	sprintf_s(szBinFileName, "%s", szFileName);

	int nLength = strlen(szBinFileName);

	if (strchr(szBinFileName, '_'))
		sprintf_s(&szBinFileName[nLength - 7], sizeof(szBinFileName) - nLength - 7, ".bin");
	else
		sprintf_s(&szBinFileName[nLength - 3], sizeof(szBinFileName) - nLength - 3, "bin");

	return ReadRCBin(szBinFileName);
}

int TMScene::ParseRC(FILE* fp, FILE* fpBinary, char* szControlType)
{
	return 0;
}

// Visual 7.502: painel de inventario com os slots do cliente antigo (coordenadas do FieldScene2 7.502).
static int s_nClassicChatRootX = 0;
static int s_nClassicChatRootY = 0;

// HUD 7.48: status no canto superior esquerdo, chat embaixo a esquerda, EXP e skills embaixo a direita.
// nAnchor 0: coordenada direta; 1: base do layout 800x600 ancorada no canto inferior direito;
// 2: ancorada no canto inferior esquerdo. Pai 65628 fica em y=525 do layout.
static bool ApplyClassicHudLayout(int nID, int& nX, int& nY, int& nWidth, int& nHeight, int* pParent, int* pTexture, int* pStyle)
{
	static const struct { int nID, nParent, nX, nY, nWidth, nHeight, nTexture, nStyle, nAnchor; } stHud[] =
	{
		{ 65621, 65610, 14, 39, 77, 77, 419, 0, 0 }, { 65623, 65610, 95, 40, 104, 28, 421, 2, 0 }, { 65625, 65610, 125, 60, 73, 8, 422, 1, 0 },
		{ 65612, 65610, 122, 13, 80, 16, 0, -1, 0 }, { 65613, 65610, 137, 13, 80, 16, 0, -1, 0 },
		{ 65614, 65610, 24, 114, 20, 16, 0, -1, 0 }, { 65615, 65610, 57, 114, 20, 16, 0, -1, 0 },
		{ 65616, 65610, 137, 34, 20, 16, 0, -1, 0 }, { 65617, 65610, 168, 34, 20, 16, 0, -1, 0 },

		{ 65641, 0, 0, 37, 311, 38, 410, -1, 2 },

		{ 65636, 0, 441, 22, 50, 36, 0, -1, 1 },
		{ 65637, 0, 453, 28, 10, 10, 413, -1, 1 }, { 65638, 0, 465, 28, 10, 10, 413, -1, 1 }, { 65639, 0, 477, 28, 10, 10, 413, -1, 1 },
		{ 65644, 0, 516, 47, 240, 23, 0, -1, 1 }, { 65645, 0, 516, 47, 240, 23, 0, -1, 1 },
		{ 65648, 0, 516, 42, 240, 5, 0, -1, 1 }, { 65643, 0, 497, 50, 20, 12, 0, -1, 1 },
		{ 66560, 0, 316, 0, 24, 24, 0, -1, 1 }, { 66561, 0, 343, 0, 24, 24, 0, -1, 1 }, { 66562, 0, 370, 0, 24, 24, 0, -1, 1 }, { 66563, 0, 397, 0, 24, 24, 0, -1, 1 }, { 66564, 0, 424, 0, 24, 24, 0, -1, 1 },
		{ 66559, 0, 315, -1, 26, 26, 0, -1, 1 }, { 66558, 0, 342, -1, 26, 26, 0, -1, 1 }, { 66527, 0, 369, -1, 26, 26, 0, -1, 1 }, { 66556, 0, 396, -1, 26, 26, 0, -1, 1 }, { 66555, 0, 423, -1, 26, 26, 0, -1, 1 },
		{ 66565, 0, 328, 12, 13, 13, 0, -1, 1 }, { 66566, 0, 355, 12, 13, 13, 0, -1, 1 }, { 66567, 0, 382, 12, 13, 13, 0, -1, 1 }, { 66568, 0, 409, 12, 13, 13, 0, -1, 1 }, { 66569, 0, 436, 12, 13, 13, 0, -1, 1 },
		{ 65787, 0, 762, 40, 29, 29, 0, -1, 1 }, { 66570, 0, 762, 10, 29, 29, 0, -1, 1 },
		{ 65786, 0, 470, -6, 30, 30, 0, -1, 1 }, { 65785, 0, 468, -6, 35, 15, 0, -1, 1 },
	};

	float fShiftX = RenderDevice::m_fResWidthBase - 800.0f;
	float fShiftY = RenderDevice::m_fResHeightBase - 600.0f;
	int nRightX = fShiftX > 0.0f ? (int)fShiftX : 0;
	int nBottomY = fShiftY > 0.0f ? (int)fShiftY : 0;

	if (nID >= 65655 && nID <= 65664)
	{
		int nSegment = nID - 65655;
		nX = 316 + (nSegment * 176) / 10 + nRightX;
		nY = 41 + nBottomY;
		nWidth = 18;
		nHeight = 13;
		if (pTexture)
			*pTexture = 414;
		return true;
	}

	if (nID >= 65775 && nID <= 65784)
	{
		nX = (65784 - nID) * 24;
		nY = 0;
		nWidth = 23;
		nHeight = 5;
		return true;
	}

	if (nID == 65672)
	{
		// Abas do chat dentro da moldura antiga; o pai (99050) e posicionado pela resolucao.
		nX = 8 - s_nClassicChatRootX;
		nY = (int)RenderDevice::m_fResHeightBase - 19 - s_nClassicChatRootY;
		return true;
	}

	for (const auto& stItem : stHud)
	{
		if (stItem.nID != nID)
			continue;

		nX = stItem.nX + (stItem.nAnchor == 1 ? nRightX : 0);
		nY = stItem.nY + (stItem.nAnchor != 0 ? nBottomY : 0);
		nWidth = stItem.nWidth;
		nHeight = stItem.nHeight;
		if (pParent && stItem.nParent)
			*pParent = stItem.nParent;
		if (pTexture && stItem.nTexture)
			*pTexture = stItem.nTexture;
		if (pStyle && stItem.nStyle >= 0)
			*pStyle = stItem.nStyle;
		return true;
	}

	return false;
}

static void AddClassicHudArt(SControlContainer* pContainer)
{
	SControl* pStatus = pContainer->FindControl(65610);
	SControl* pBottom = pContainer->FindControl(65628);
	if (pStatus == nullptr || pBottom == nullptr)
		return;

	float fRightX = RenderDevice::m_fResWidthBase > 800.0f ? RenderDevice::m_fResWidthBase - 800.0f : 0.0f;
	float fBottomY = RenderDevice::m_fResHeightBase > 600.0f ? RenderDevice::m_fResHeightBase - 600.0f : 0.0f;

	// Textos de ataque e defesa ficam antes da arte: filhos adicionados por ultimo sao desenhados por baixo.
	static const struct { unsigned int dwID; float fX; } stValues[] =
	{
		{ 9100001, 335.0f }, { 9100002, 367.0f }, { 9100003, 424.0f }, { 9100004, 454.0f },
	};
	for (const auto& stValue : stValues)
	{
		auto pText = new SText(-1, "", 0xFFFFFFFF, stValue.fX + fRightX, 56.0f + fBottomY, 26.0f, 16.0f, 0, 0, 1, 1);
		pText->SetControlID(stValue.dwID);
		pBottom->AddChild(static_cast<TreeNode*>(pText));
	}

	static const struct { bool bStatus; int nTexture; float fX, fY, fWidth, fHeight; } stArt[] =
	{
		{ true, 411, 0.0f, 0.0f, 206.0f, 132.0f },
		{ false, 409, 311.0f, 26.0f, 185.0f, 49.0f },
		{ false, 406, 496.0f, 37.0f, 264.0f, 38.0f },
	};
	for (const auto& stItem : stArt)
	{
		float fX = stItem.fX + (stItem.bStatus ? 0.0f : fRightX);
		float fY = stItem.fY + (stItem.bStatus ? 0.0f : fBottomY);
		auto pPanel = new SPanel(stItem.nTexture, fX, fY, stItem.fWidth, stItem.fHeight, 0xFFFFFFFF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		pPanel->m_bPickable = 0;
		(stItem.bStatus ? pStatus : pBottom)->AddChild(static_cast<TreeNode*>(pPanel));
	}
}

static void ApplyClassicInventoryLayout(const char* szBinFileName, int nID, int& nX, int& nY, int& nWidth, int& nHeight,
	int* pParent = nullptr, int* pTexture = nullptr, int* pStyle = nullptr)
{
	if (strstr(szBinFileName, "FieldScene2") == nullptr)
		return;

	if (nID == 99050)
	{
		s_nClassicChatRootX = nX;
		s_nClassicChatRootY = nY;
		return;
	}

	if (ApplyClassicHudLayout(nID, nX, nY, nWidth, nHeight, pParent, pTexture, pStyle))
		return;

	static const struct { int nID, nX, nY, nWidth, nHeight; } stLayout[] =
	{
		// Grade 9x7 (as 4 paginas ocupam o mesmo retangulo)
		{ 67072, 6, 225, 216, 168 }, { 67073, 6, 225, 216, 168 }, { 67074, 6, 225, 216, 168 }, { 67075, 6, 225, 216, 168 },
		// Equipamentos
		{ 65546, 60, 32, 23, 23 }, { 65547, 144, 32, 23, 23 }, { 65548, 60, 86, 23, 23 }, { 65549, 144, 86, 23, 23 },
		{ 65550, 60, 138, 23, 23 }, { 65551, 144, 138, 23, 23 }, { 65552, 60, 192, 23, 23 }, { 65553, 144, 192, 23, 23 },
		{ 65555, 90, 32, 47, 47 }, { 65556, 174, 48, 47, 95 }, { 65557, 6, 48, 47, 95 }, { 65558, 90, 88, 47, 47 },
		{ 65559, 90, 144, 47, 71 }, { 65560, 6, 152, 47, 47 }, { 65561, 174, 152, 47, 47 },
		// Slots extras do 7.87 nas 3 celulas livres da grade e lixeira rapida
		{ 1048976, 150, 369, 24, 24 }, { 1048977, 174, 369, 24, 24 }, { 1048978, 198, 369, 24, 24 }, { 67080, 172, 395, 22, 22 },
		// Honra e cash abaixo da grade
		{ 1048981, 6, 395, 50, 12 }, { 1048982, 6, 406, 50, 12 },

		// Bau: grade 9x14
		{ 67328, 6, 57, 216, 336 }, { 67329, 6, 57, 216, 336 }, { 67330, 6, 57, 216, 336 }, { 67331, 6, 57, 216, 336 },

		// Personagem (posicoes do painel 7.502)
		{ 65703, 55, 42, 90, 16 }, { 65704, 7, 42, 90, 16 }, { 65705, 55, 58, 90, 16 }, { 65706, 7, 58, 90, 16 },
		{ 65707, 55, 74, 90, 16 }, { 65708, 127, 74, 90, 16 }, { 65709, 7, 74, 90, 16 },
		{ 65710, 55, 90, 67, 16 }, { 65711, 134, 90, 67, 16 }, { 65712, 7, 90, 67, 16 },
		{ 65713, 179, 42, 33, 16 }, { 65714, 143, 42, 33, 16 },
		{ 65715, 55, 130, 67, 16 }, { 65717, 7, 130, 67, 16 }, { 65716, 93, 132, 13, 13 },
		{ 65718, 55, 146, 67, 16 }, { 65720, 7, 146, 67, 16 }, { 65719, 93, 148, 13, 13 },
		{ 65721, 55, 162, 67, 16 }, { 65723, 7, 162, 67, 16 }, { 65722, 93, 164, 13, 13 },
		{ 65724, 55, 178, 67, 16 }, { 65726, 7, 178, 67, 16 }, { 65725, 93, 180, 13, 13 },
		{ 65727, 55, 194, 67, 16 }, { 65728, 7, 194, 67, 16 },
		{ 65729, 67, 215, 67, 16 }, { 65730, 7, 215, 67, 16 }, { 65731, 67, 231, 67, 16 }, { 65732, 7, 231, 67, 16 },
		{ 65733, 67, 247, 67, 16 }, { 65734, 7, 247, 67, 16 }, { 65739, 67, 263, 67, 16 }, { 65740, 7, 263, 67, 16 },
		{ 65737, 174, 215, 67, 16 }, { 65738, 115, 215, 67, 16 }, { 65735, 174, 231, 67, 16 }, { 65736, 115, 231, 67, 16 },
		{ 65741, 174, 247, 67, 16 }, { 65742, 115, 247, 67, 16 }, { 65743, 174, 263, 67, 16 }, { 65744, 115, 263, 67, 16 },
		{ 65745, 174, 132, 67, 16 }, { 65746, 127, 132, 67, 16 }, { 65747, 174, 148, 67, 16 }, { 65748, 127, 148, 67, 16 },
		{ 65749, 174, 164, 67, 16 }, { 65750, 127, 164, 67, 16 }, { 65751, 174, 180, 67, 16 }, { 65752, 127, 180, 67, 16 },
		{ 65753, 155, 309, 67, 16 }, { 65755, 67, 309, 67, 16 }, { 65754, 199, 311, 13, 13 },
		{ 65756, 155, 325, 67, 16 }, { 65758, 67, 325, 67, 16 }, { 65757, 199, 327, 13, 13 },
		{ 65759, 155, 341, 67, 16 }, { 65761, 67, 341, 67, 16 }, { 65760, 199, 343, 13, 13 },
		{ 65762, 155, 357, 67, 16 }, { 65764, 67, 357, 67, 16 }, { 65763, 199, 359, 13, 13 },
		{ 65765, 165, 373, 67, 16 }, { 65766, 67, 373, 67, 16 },

		// Tecnicas: 12 molduras por categoria
		{ 65569, 81, 5, 90, 16 }, { 65570, 33, 40, 90, 16 }, { 65571, 33, 130, 90, 16 }, { 65572, 33, 220, 90, 16 }, { 65573, 33, 310, 90, 16 },
		{ 65574, 30, 63, 21, 21 }, { 65575, 59, 63, 21, 21 }, { 65576, 88, 63, 21, 21 }, { 65577, 117, 63, 21, 21 }, { 65578, 146, 63, 21, 21 }, { 65579, 175, 63, 21, 21 },
		{ 65580, 30, 95, 21, 21 }, { 65581, 59, 95, 21, 21 }, { 94208, 88, 95, 21, 21 }, { 94209, 117, 95, 21, 21 }, { 94210, 146, 95, 21, 21 }, { 94211, 175, 95, 21, 21 },
		{ 65582, 30, 153, 21, 21 }, { 65583, 59, 153, 21, 21 }, { 65584, 88, 153, 21, 21 }, { 65585, 117, 153, 21, 21 }, { 65586, 146, 153, 21, 21 }, { 65587, 175, 153, 21, 21 },
		{ 65588, 30, 185, 21, 21 }, { 65589, 59, 185, 21, 21 }, { 94212, 88, 185, 21, 21 }, { 94213, 117, 185, 21, 21 }, { 94214, 146, 185, 21, 21 }, { 94215, 175, 185, 21, 21 },
		{ 65590, 30, 243, 21, 21 }, { 65591, 59, 243, 21, 21 }, { 65592, 88, 243, 21, 21 }, { 65593, 117, 243, 21, 21 }, { 65594, 146, 243, 21, 21 }, { 65595, 175, 243, 21, 21 },
		{ 65596, 30, 275, 21, 21 }, { 65597, 59, 275, 21, 21 }, { 94216, 88, 275, 21, 21 }, { 94217, 117, 275, 21, 21 }, { 94218, 146, 275, 21, 21 }, { 94219, 175, 275, 21, 21 },
		{ 65598, 30, 333, 174, 64 }, { 65599, 50, 400, 60, 12 }, { 65600, 138, 400, 33, 12 },

		// Loja: grade 9x13
		{ 65694, 6, 81, 216, 312 },

		// Mestre de tecnicas
		{ 65604, 33, 40, 90, 16 }, { 65605, 33, 130, 90, 16 }, { 65606, 33, 220, 90, 16 },

		// Titulos e valores nas posicoes do 7.543
		{ 65701, 169, 115, 90, 16 }, { 65687, 89, 4, 90, 16 }, { 65689, 37, 397, 74, 18 }, { 65693, 89, 5, 90, 16 },
	};

	for (const auto& stItem : stLayout)
	{
		if (stItem.nID == nID)
		{
			nX = stItem.nX;
			nY = stItem.nY;
			nWidth = stItem.nWidth;
			nHeight = stItem.nHeight;
			return;
		}
	}

	// Fundos de slot, botoes de pagina e datas das bolsas nao existem na arte antiga.
	if ((nID >= 3080 && nID <= 3112) || nID == 43087 || (nID >= 67076 && nID <= 67079) || nID == 77834 || nID == 77835 ||
		(nID >= 3000 && nID <= 3079) || (nID >= 67332 && nID <= 67335) ||
		(nID >= 66496 && nID <= 66554 && nID != 66527) || (nID >= 3120 && nID <= 3144) ||
		nID == 469636 || nID == 569636 || nID == 69635 || nID == 69636 || nID == 65681)
	{
		nX = -3000;
		nY = -3000;
	}
}

int TMScene::ReadRCBin(char* szBinFileName)
{
	FILE* fpBinary = nullptr;

	fopen_s(&fpBinary, szBinFileName, "rb");

	if (!fpBinary)
		return 0;

	CONTROL_TYPE nControlType{};

	while (fread(&nControlType, 4, 1, fpBinary))
	{
		switch (nControlType)
		{
		case CONTROL_TYPE::CTRL_TYPE_PANEL:
		{
			BinPanel binPanelData;
			if (!fread(&binPanelData, sizeof(binPanelData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}

			stBinControlBase controlBase{ binPanelData.nID, binPanelData.nStartX, binPanelData.nStartY, binPanelData.nWidth, binPanelData.nHeight,
				binPanelData.nTextureSetIndex };
			 

			ResizeItem(&controlBase);


			binPanelData.nStartX = controlBase.nStartX;
			binPanelData.nStartY = controlBase.nStartY;
			binPanelData.nWidth = controlBase.nWidth;
			binPanelData.nHeight = controlBase.nHeight;
			ApplyClassicInventoryLayout(szBinFileName, binPanelData.nID, binPanelData.nStartX, binPanelData.nStartY, binPanelData.nWidth, binPanelData.nHeight, &binPanelData.nParentID, &binPanelData.nTextureSetIndex);
			 
			auto pPanel = new SPanel(
				binPanelData.nTextureSetIndex,
				(float)binPanelData.nStartX,
				(float)binPanelData.nStartY,
				(float)binPanelData.nWidth,
				(float)binPanelData.nHeight,
				binPanelData.nColor,
				static_cast<RENDERCTRLTYPE>(binPanelData.nFillType));

			if (!pPanel)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binPanelData.nID, GetSceneType());
				return 0;
			}

			pPanel->SetControlID(binPanelData.nID);
			pPanel->SetCenterPos(binPanelData.nID,
				(float)binPanelData.nStartX,
				(float)binPanelData.nStartY,
				(float)binPanelData.nWidth,
				(float)binPanelData.nHeight);


			if (binPanelData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(binPanelData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(pPanel));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(pPanel));
			}

			pPanel->m_bPickable = binPanelData.nPickable;

		}
		break;
		case CONTROL_TYPE::CTRL_TYPE_GRID:
		{
			BinGrid binGridData;

			if (!fread(&binGridData, sizeof(binGridData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}

			stBinControlBase controlBase{ binGridData.nID, binGridData.nStartX, binGridData.nStartY, binGridData.nWidth, binGridData.nHeight,
				binGridData.nTextureSetIndex };
			ResizeItem(&controlBase);


			binGridData.nStartX = controlBase.nStartX;
			binGridData.nStartY = controlBase.nStartY;
			binGridData.nWidth = controlBase.nWidth;
			binGridData.nHeight = controlBase.nHeight;
			ApplyClassicInventoryLayout(szBinFileName, binGridData.nID, binGridData.nStartX, binGridData.nStartY, binGridData.nWidth, binGridData.nHeight);

			auto pGrid = new SGridControl(
				binGridData.nTextureSetIndex,
				binGridData.nRowCount,
				binGridData.nColumnCount,
				(float)binGridData.nStartX,
				(float)binGridData.nStartY,
				(float)binGridData.nWidth,
				(float)binGridData.nHeight,
				static_cast<TMEITEMTYPE>(binGridData.nType));

			if (!pGrid)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binGridData.nID, GetSceneType());
				return 0;
			}

			pGrid->SetControlID(binGridData.nID);

			pGrid->SetCenterPos(binGridData.nID,
				(float)binGridData.nStartX,
				(float)binGridData.nStartY,
				(float)binGridData.nWidth,
				(float)binGridData.nHeight);

			if (m_pControlContainer)
				pGrid->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
			else
				pGrid->SetEventListener(nullptr);

			if (binGridData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(binGridData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(pGrid));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(pGrid));
			}
		}
		break;
		case CONTROL_TYPE::CTRL_TYPE_3DOBJ:
		{
			Bin3DObj bin3DObjData;

			if (!fread(&bin3DObjData, sizeof(bin3DObjData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}

			stBinControlBase controlBase{ bin3DObjData.nID, bin3DObjData.nStartX, bin3DObjData.nStartY, bin3DObjData.nWidth, bin3DObjData.nHeight,
				bin3DObjData.n3DObjIndex };
			ResizeItem(&controlBase);


			bin3DObjData.nStartX = controlBase.nStartX;
			bin3DObjData.nStartY = controlBase.nStartY;
			bin3DObjData.nWidth = controlBase.nWidth;
			bin3DObjData.nHeight = controlBase.nHeight;
			auto p3DObj = new S3DObj(bin3DObjData.n3DObjIndex,
				(float)bin3DObjData.nStartX,
				(float)bin3DObjData.nStartY,
				(float)bin3DObjData.nWidth,
				(float)bin3DObjData.nHeight);

			if (!p3DObj)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", bin3DObjData.nID, GetSceneType());
				return 0;
			}

			p3DObj->SetControlID(bin3DObjData.nID);
			p3DObj->SetCenterPos(bin3DObjData.nID,
				(float)bin3DObjData.nStartX,
				(float)bin3DObjData.nStartY,
				(float)bin3DObjData.nWidth,
				(float)bin3DObjData.nHeight);

			if (bin3DObjData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(bin3DObjData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(p3DObj));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(p3DObj));
			}
		}
		break;
		case CONTROL_TYPE::CTRL_TYPE_BUTTON:
		{
			BinButton binButtonData;

			if (!fread(&binButtonData, sizeof(binButtonData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}


			stBinControlBase controlBase{ binButtonData.nID, binButtonData.nStartX, binButtonData.nStartY, binButtonData.nWidth, binButtonData.nHeight,
				binButtonData.nTextureSetIndex };
			ResizeItem(&controlBase);


			binButtonData.nStartX = controlBase.nStartX;
			binButtonData.nStartY = controlBase.nStartY;
			binButtonData.nWidth = controlBase.nWidth;
			binButtonData.nHeight = controlBase.nHeight;
			ApplyClassicInventoryLayout(szBinFileName, binButtonData.nID, binButtonData.nStartX, binButtonData.nStartY, binButtonData.nWidth, binButtonData.nHeight);

			/*texto no btn*/
			char strbuf[128]{};
			sprintf_s(strbuf, "%s", g_UIString[binButtonData.nStringIndex]);

			auto pButton = new SButton(
				binButtonData.nTextureSetIndex,
				(float)binButtonData.nStartX,
				(float)binButtonData.nStartY,
				(float)binButtonData.nWidth,
				(float)binButtonData.nHeight,
				binButtonData.nColor,
				binButtonData.nSound,
				strbuf);

			if (!pButton)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binButtonData.nID, GetSceneType());
				return 0;
			}

			pButton->SetControlID(binButtonData.nID);

			pButton->SetCenterPos(binButtonData.nID,
				(float)binButtonData.nStartX,
				(float)binButtonData.nStartY,
				(float)binButtonData.nWidth,
				(float)binButtonData.nHeight);

			if (binButtonData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(binButtonData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(pButton));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(pButton));
			}

			if (m_pControlContainer)
				pButton->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
			else
				pButton->SetEventListener(nullptr);
		}
		break;
		case CONTROL_TYPE::CTRL_TYPE_TEXT:
		{
			BinText binTextData;

			if (!fread(&binTextData, sizeof(binTextData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}

			stBinControlBase controlBase{ binTextData.nID, binTextData.nStartX, binTextData.nStartY, binTextData.nWidth, binTextData.nHeight,
				binTextData.nTextureSetIndex };
			ResizeItem(&controlBase);


			binTextData.nStartX = controlBase.nStartX;
			binTextData.nStartY = controlBase.nStartY;
			binTextData.nWidth = controlBase.nWidth;
			binTextData.nHeight = controlBase.nHeight;
			ApplyClassicInventoryLayout(szBinFileName, binTextData.nID, binTextData.nStartX, binTextData.nStartY, binTextData.nWidth, binTextData.nHeight, &binTextData.nParentID);
			char strbuf[128]{};

			sprintf_s(strbuf, "%s", g_UIString[binTextData.nStringIndex]);

			auto pText = new SText(
				binTextData.nTextureSetIndex,
				strbuf,
				binTextData.nFontColor,
				(float)binTextData.nStartX,
				(float)binTextData.nStartY,
				(float)binTextData.nWidth,
				(float)binTextData.nHeight,
				binTextData.nBorder,
				binTextData.nBorderColor,
				binTextData.nTextType,
				binTextData.nAlignType);

			if (!pText)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binTextData.nID, GetSceneType());
				return 0;
			}

			pText->SetControlID(binTextData.nID);

			pText->SetCenterPos(binTextData.nID,
				(float)binTextData.nStartX,
				(float)binTextData.nStartY,
				(float)binTextData.nWidth,
				(float)binTextData.nHeight);

			if (binTextData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(binTextData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(pText));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(pText));
			}
		}
		break;
		case CONTROL_TYPE::CTRL_TYPE_EDITABLETEXT:
		{
			BinEdit binEditData;

			if (!fread(&binEditData, sizeof(binEditData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}

			stBinControlBase controlBase{ binEditData.nID, binEditData.nStartX, binEditData.nStartY, binEditData.nWidth, binEditData.nHeight,
					binEditData.nTextureSetIndex };
			ResizeItem(&controlBase);


			binEditData.nStartX = controlBase.nStartX;
			binEditData.nStartY = controlBase.nStartY;
			binEditData.nWidth = controlBase.nWidth;
			binEditData.nHeight = controlBase.nHeight;

			auto pEdit = new SEditableText(
				binEditData.nTextureSetIndex,
				binEditData.szString,
				binEditData.nMaxStringLength,
				binEditData.nPassword,
				binEditData.nFontColor,
				(float)binEditData.nStartX,
				(float)binEditData.nStartY,
				(float)binEditData.nWidth,
				(float)binEditData.nHeight,
				binEditData.nBorder,
				binEditData.nBorderColor,
				binEditData.nTextType,
				binEditData.nAlignType);

			if (!pEdit)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binEditData.nID, GetSceneType());
				return 0;
			}

			pEdit->SetControlID(binEditData.nID);

			pEdit->SetCenterPos(binEditData.nID,
				(float)binEditData.nStartX,
				(float)binEditData.nStartY,
				(float)binEditData.nWidth,
				(float)binEditData.nHeight);


			if (binEditData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(binEditData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(pEdit));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(pEdit));
			}

			if (m_pControlContainer)
				pEdit->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
			else
				pEdit->SetEventListener(nullptr);
		}
		break;
		case CONTROL_TYPE::CTRL_TYPE_PROGRESSBAR:
		{
			BinProgress binProgressData;

			if (!fread(&binProgressData, sizeof(binProgressData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}

			stBinControlBase controlBase{ binProgressData.nID, binProgressData.nStartX, binProgressData.nStartY, binProgressData.nWidth, binProgressData.nHeight,
				binProgressData.nTextureSetIndex };
			ResizeItem(&controlBase);


			binProgressData.nStartX = controlBase.nStartX;
			binProgressData.nStartY = controlBase.nStartY;
			binProgressData.nWidth = controlBase.nWidth;
			binProgressData.nHeight = controlBase.nHeight;
			ApplyClassicInventoryLayout(szBinFileName, binProgressData.nID, binProgressData.nStartX, binProgressData.nStartY, binProgressData.nWidth, binProgressData.nHeight, &binProgressData.nParentID, &binProgressData.nTextureSetIndex, &binProgressData.nStyle);

			auto pProgress = new SProgressBar(
				binProgressData.nTextureSetIndex,
				binProgressData.nCurrent,
				binProgressData.nMaxValue,
				(float)binProgressData.nStartX,
				(float)binProgressData.nStartY,
				(float)binProgressData.nWidth,
				(float)binProgressData.nHeight,
				binProgressData.nProgressColor,
				binProgressData.nColor,
				binProgressData.nStyle);

			if (!pProgress)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binProgressData.nID, GetSceneType());
				return 0;
			}

			pProgress->SetControlID(binProgressData.nID);

			pProgress->SetCenterPos(binProgressData.nID,
				(float)binProgressData.nStartX,
				(float)binProgressData.nStartY,
				(float)binProgressData.nWidth,
				(float)binProgressData.nHeight);

			if (binProgressData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(binProgressData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(pProgress));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(pProgress));
			}

			if (m_pControlContainer)
				pProgress->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
			else
				pProgress->SetEventListener(nullptr);
		}
		break;
		case CONTROL_TYPE::CTRL_TYPE_CHECKBOX:
		{
			BinCheckBox binCheckBoxData;

			if (!fread(&binCheckBoxData, sizeof(binCheckBoxData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}

			stBinControlBase controlBase{ binCheckBoxData.nID, binCheckBoxData.nStartX, binCheckBoxData.nStartY, binCheckBoxData.nWidth, binCheckBoxData.nHeight,
											binCheckBoxData.nTextureSetIndex };
			ResizeItem(&controlBase);


			binCheckBoxData.nStartX = controlBase.nStartX;
			binCheckBoxData.nStartY = controlBase.nStartY;
			binCheckBoxData.nWidth = controlBase.nWidth;
			binCheckBoxData.nHeight = controlBase.nHeight;

			auto pCheckBox = new SCheckBox(
				binCheckBoxData.nTextureSetIndex,
				(float)binCheckBoxData.nStartX,
				(float)binCheckBoxData.nStartY,
				(float)binCheckBoxData.nWidth,
				(float)binCheckBoxData.nHeight,
				binCheckBoxData.nColor);

			if (!pCheckBox)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binCheckBoxData.nID, GetSceneType());
				return 0;
			}

			pCheckBox->SetControlID(binCheckBoxData.nID);

			pCheckBox->SetCenterPos(binCheckBoxData.nID,
				(float)binCheckBoxData.nStartX,
				(float)binCheckBoxData.nStartY,
				(float)binCheckBoxData.nWidth,
				(float)binCheckBoxData.nHeight);

			if (binCheckBoxData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(binCheckBoxData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(pCheckBox));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(pCheckBox));
			}

			if (m_pControlContainer)
				pCheckBox->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
			else
				pCheckBox->SetEventListener(nullptr);
		}
		break;
		case CONTROL_TYPE::CTRL_TYPE_LISTBOX:
		{
			BinListBox binListBoxData;

			if (!fread(&binListBoxData, sizeof(binListBoxData), 1, fpBinary))
			{
				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
				fclose(fpBinary);
				return 0;
			}

			stBinControlBase controlBase{ binListBoxData.nID, binListBoxData.nStartX, binListBoxData.nStartY, binListBoxData.nWidth, binListBoxData.nHeight, binListBoxData.nTextureSetIndex };


			ResizeItem(&controlBase);


			binListBoxData.nStartX = controlBase.nStartX;
			binListBoxData.nStartY = controlBase.nStartY;
			binListBoxData.nWidth = controlBase.nWidth;
			binListBoxData.nHeight = controlBase.nHeight;

			auto pListBox = new SListBox(
				binListBoxData.nTextureSetIndex,
				binListBoxData.nMaxCount,
				binListBoxData.nVisibleCount,
				(float)binListBoxData.nStartX,
				(float)binListBoxData.nStartY,
				(float)binListBoxData.nWidth,
				(float)binListBoxData.nHeight,
				binListBoxData.nColor,
				static_cast<RENDERCTRLTYPE>(binListBoxData.nFillType),
				binListBoxData.nSelect,
				binListBoxData.nScroll,
				0);

			if (!pListBox)
			{
				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binListBoxData.nID, GetSceneType());
				return 0;
			}

			pListBox->SetControlID(binListBoxData.nID);

			pListBox->SetCenterPos(binListBoxData.nID,
				(float)binListBoxData.nStartX,
				(float)binListBoxData.nStartY,
				(float)binListBoxData.nWidth,
				(float)binListBoxData.nHeight);


			if (binListBoxData.nParentID)
			{
				auto pParent = m_pControlContainer->FindControl(binListBoxData.nParentID);

				if (pParent)
					pParent->AddChild(static_cast<TreeNode*>(pListBox));
			}
			else
			{
				m_pControlContainer->AddItem(static_cast<SControl*>(pListBox));
			}

			if (m_pControlContainer)
				pListBox->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
			else
				pListBox->SetEventListener(nullptr);
		}
		break;
		default:
			LOG_WRITELOG("Not Support Control Type [%d]\r\n", nControlType);
			return 0;
		}
	}

	if (strstr(szBinFileName, "FieldScene2") != nullptr)
		AddClassicHudArt(m_pControlContainer);

	fclose(fpBinary);


	return 1;
}
//int TMScene::ReadRCBin(char* szBinFileName)
//{
//	FILE* fpBinary = nullptr;
//
//	fopen_s(&fpBinary, szBinFileName, "rb");
//
//	if (!fpBinary)
//		return 0;
//
//	CONTROL_TYPE nControlType{};
//
//	while (fread(&nControlType, 4, 1, fpBinary))
//	{
//		switch (nControlType)
//		{
//		case CONTROL_TYPE::CTRL_TYPE_PANEL:
//		{
//			BinPanel binPanelData;
//
//			if (!fread(&binPanelData, sizeof(binPanelData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			auto pPanel = new SPanel(
//				binPanelData.nTextureSetIndex,
//				(float)binPanelData.nStartX,
//				(float)binPanelData.nStartY,
//				(float)binPanelData.nWidth,
//				(float)binPanelData.nHeight,
//				binPanelData.nColor,
//				static_cast<RENDERCTRLTYPE>(binPanelData.nFillType));
//
//			if (!pPanel)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binPanelData.nID, GetSceneType());
//				return 0;
//			}
//
//			pPanel->SetControlID(binPanelData.nID);
//			pPanel->SetCenterPos(binPanelData.nID,
//				(float)binPanelData.nStartX,
//				(float)binPanelData.nStartY,
//				(float)binPanelData.nWidth,
//				(float)binPanelData.nHeight);
//
//			if (binPanelData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(binPanelData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(pPanel));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(pPanel));
//			}
//
//			pPanel->m_bPickable = binPanelData.nPickable;
//		}
//		break;
//		case CONTROL_TYPE::CTRL_TYPE_GRID:
//		{
//			BinGrid binGridData;
//
//			if (!fread(&binGridData, sizeof(binGridData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			auto pGrid = new SGridControl(
//				binGridData.nTextureSetIndex,
//				binGridData.nRowCount,
//				binGridData.nColumnCount,
//				(float)binGridData.nStartX,
//				(float)binGridData.nStartY,
//				(float)binGridData.nWidth,
//				(float)binGridData.nHeight,
//				static_cast<TMEITEMTYPE>(binGridData.nType));
//
//			if (!pGrid)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binGridData.nID, GetSceneType());
//				return 0;
//			}
//
//			pGrid->SetControlID(binGridData.nID);
//
//			pGrid->SetCenterPos(binGridData.nID,
//				(float)binGridData.nStartX,
//				(float)binGridData.nStartY,
//				(float)binGridData.nWidth,
//				(float)binGridData.nHeight);
//
//			if (m_pControlContainer)
//				pGrid->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
//			else
//				pGrid->SetEventListener(nullptr);
//
//			if (binGridData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(binGridData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(pGrid));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(pGrid));
//			}
//		}
//		break;
//		case CONTROL_TYPE::CTRL_TYPE_3DOBJ:
//		{
//			Bin3DObj bin3DObjData;
//
//			if (!fread(&bin3DObjData, sizeof(bin3DObjData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			auto p3DObj = new S3DObj(bin3DObjData.n3DObjIndex,
//				(float)bin3DObjData.nStartX,
//				(float)bin3DObjData.nStartY,
//				(float)bin3DObjData.nWidth,
//				(float)bin3DObjData.nHeight);
//
//			if (!p3DObj)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", bin3DObjData.nID, GetSceneType());
//				return 0;
//			}
//
//			p3DObj->SetControlID(bin3DObjData.nID);
//			p3DObj->SetCenterPos(bin3DObjData.nID,
//				(float)bin3DObjData.nStartX,
//				(float)bin3DObjData.nStartY,
//				(float)bin3DObjData.nWidth,
//				(float)bin3DObjData.nHeight);
//
//			if (bin3DObjData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(bin3DObjData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(p3DObj));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(p3DObj));
//			}
//		}
//		break;
//		case CONTROL_TYPE::CTRL_TYPE_BUTTON:
//		{
//			BinButton binButtonData;
//
//			if (!fread(&binButtonData, sizeof(binButtonData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			char strbuf[128]{};
//
//			sprintf_s(strbuf, "%s", g_UIString[binButtonData.nStringIndex]);
//
//			auto pButton = new SButton(
//				binButtonData.nTextureSetIndex,
//				(float)binButtonData.nStartX,
//				(float)binButtonData.nStartY,
//				(float)binButtonData.nWidth,
//				(float)binButtonData.nHeight,
//				binButtonData.nColor,
//				binButtonData.nSound,
//				strbuf);
//
//			if (!pButton)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binButtonData.nID, GetSceneType());
//				return 0;
//			}
//
//			pButton->SetControlID(binButtonData.nID);
//
//			pButton->SetCenterPos(binButtonData.nID,
//				(float)binButtonData.nStartX,
//				(float)binButtonData.nStartY,
//				(float)binButtonData.nWidth,
//				(float)binButtonData.nHeight);
//
//			if (binButtonData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(binButtonData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(pButton));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(pButton));
//			}
//
//			if (m_pControlContainer)
//				pButton->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
//			else
//				pButton->SetEventListener(nullptr);
//		}
//		break;
//		case CONTROL_TYPE::CTRL_TYPE_TEXT:
//		{
//			BinText binTextData;
//
//			if (!fread(&binTextData, sizeof(binTextData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			char strbuf[128]{};
//
//			sprintf_s(strbuf, "%s", g_UIString[binTextData.nStringIndex]);
//
//			auto pText = new SText(
//				binTextData.nTextureSetIndex,
//				strbuf,
//				binTextData.nFontColor,
//				(float)binTextData.nStartX,
//				(float)binTextData.nStartY,
//				(float)binTextData.nWidth,
//				(float)binTextData.nHeight,
//				binTextData.nBorder,
//				binTextData.nBorderColor,
//				binTextData.nTextType,
//				binTextData.nAlignType);
//
//			if (!pText)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binTextData.nID, GetSceneType());
//				return 0;
//			}
//
//			pText->SetControlID(binTextData.nID);
//
//			pText->SetCenterPos(binTextData.nID,
//				(float)binTextData.nStartX,
//				(float)binTextData.nStartY,
//				(float)binTextData.nWidth,
//				(float)binTextData.nHeight);
//
//			if (binTextData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(binTextData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(pText));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(pText));
//			}
//		}
//		break;
//		case CONTROL_TYPE::CTRL_TYPE_EDITABLETEXT:
//		{
//			BinEdit binEditData;
//
//			if (!fread(&binEditData, sizeof(binEditData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			auto pEdit = new SEditableText(
//				binEditData.nTextureSetIndex,
//				binEditData.szString,
//				binEditData.nMaxStringLength,
//				binEditData.nPassword,
//				binEditData.nFontColor,
//				(float)binEditData.nStartX,
//				(float)binEditData.nStartY,
//				(float)binEditData.nWidth,
//				(float)binEditData.nHeight,
//				binEditData.nBorder,
//				binEditData.nBorderColor,
//				binEditData.nTextType,
//				binEditData.nAlignType);
//
//			if (!pEdit)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binEditData.nID, GetSceneType());
//				return 0;
//			}
//
//			pEdit->SetControlID(binEditData.nID);
//
//			pEdit->SetCenterPos(binEditData.nID,
//				(float)binEditData.nStartX,
//				(float)binEditData.nStartY,
//				(float)binEditData.nWidth,
//				(float)binEditData.nHeight);
//
//			if (binEditData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(binEditData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(pEdit));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(pEdit));
//			}
//
//			if (m_pControlContainer)
//				pEdit->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
//			else
//				pEdit->SetEventListener(nullptr);
//		}
//		break;
//		case CONTROL_TYPE::CTRL_TYPE_PROGRESSBAR:
//		{
//			BinProgress binProgressData;
//
//			if (!fread(&binProgressData, sizeof(binProgressData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			auto pProgress = new SProgressBar(
//				binProgressData.nTextureSetIndex,
//				binProgressData.nCurrent,
//				binProgressData.nMaxValue,
//				(float)binProgressData.nStartX,
//				(float)binProgressData.nStartY,
//				(float)binProgressData.nWidth,
//				(float)binProgressData.nHeight,
//				binProgressData.nProgressColor,
//				binProgressData.nColor,
//				binProgressData.nStyle);
//
//			if (!pProgress)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binProgressData.nID, GetSceneType());
//				return 0;
//			}
//
//			pProgress->SetControlID(binProgressData.nID);
//
//			pProgress->SetCenterPos(binProgressData.nID,
//				(float)binProgressData.nStartX,
//				(float)binProgressData.nStartY,
//				(float)binProgressData.nWidth,
//				(float)binProgressData.nHeight);
//
//			if (binProgressData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(binProgressData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(pProgress));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(pProgress));
//			}
//
//			if (m_pControlContainer)
//				pProgress->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
//			else
//				pProgress->SetEventListener(nullptr);
//		}
//		break;
//		case CONTROL_TYPE::CTRL_TYPE_CHECKBOX:
//		{
//			BinCheckBox binCheckBoxData;
//
//			if (!fread(&binCheckBoxData, sizeof(binCheckBoxData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			auto pCheckBox = new SCheckBox(
//				binCheckBoxData.nTextureSetIndex,
//				(float)binCheckBoxData.nStartX,
//				(float)binCheckBoxData.nStartY,
//				(float)binCheckBoxData.nWidth,
//				(float)binCheckBoxData.nHeight,
//				binCheckBoxData.nColor);
//
//			if (!pCheckBox)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binCheckBoxData.nID, GetSceneType());
//				return 0;
//			}
//
//			pCheckBox->SetControlID(binCheckBoxData.nID);
//
//			pCheckBox->SetCenterPos(binCheckBoxData.nID,
//				(float)binCheckBoxData.nStartX,
//				(float)binCheckBoxData.nStartY,
//				(float)binCheckBoxData.nWidth,
//				(float)binCheckBoxData.nHeight);
//
//			if (binCheckBoxData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(binCheckBoxData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(pCheckBox));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(pCheckBox));
//			}
//
//			if (m_pControlContainer)
//				pCheckBox->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
//			else
//				pCheckBox->SetEventListener(nullptr);
//		}
//		break;
//		case CONTROL_TYPE::CTRL_TYPE_LISTBOX:
//		{
//			BinListBox binListBoxData;
//
//			if (!fread(&binListBoxData, sizeof(binListBoxData), 1, fpBinary))
//			{
//				LOG_WRITELOG("Can't Read Resource Data[%d] in [%s]\r\n", nControlType, szBinFileName);
//				fclose(fpBinary);
//				return 0;
//			}
//
//			auto pListBox = new SListBox(
//				binListBoxData.nTextureSetIndex,
//				binListBoxData.nMaxCount,
//				binListBoxData.nVisibleCount,
//				(float)binListBoxData.nStartX,
//				(float)binListBoxData.nStartY,
//				(float)binListBoxData.nWidth,
//				(float)binListBoxData.nHeight,
//				binListBoxData.nColor,
//				static_cast<RENDERCTRLTYPE>(binListBoxData.nFillType),
//				binListBoxData.nSelect,
//				binListBoxData.nScroll,
//				0);
//
//			if (!pListBox)
//			{
//				LOG_WRITELOG("Can't Create [%d] in Scene [%d]\r\n", binListBoxData.nID, GetSceneType());
//				return 0;
//			}
//
//			pListBox->SetControlID(binListBoxData.nID);
//
//			pListBox->SetCenterPos(binListBoxData.nID,
//				(float)binListBoxData.nStartX,
//				(float)binListBoxData.nStartY,
//				(float)binListBoxData.nWidth,
//				(float)binListBoxData.nHeight);
//
//			if (binListBoxData.nParentID)
//			{
//				auto pParent = m_pControlContainer->FindControl(binListBoxData.nParentID);
//
//				if (pParent)
//					pParent->AddChild(static_cast<TreeNode*>(pListBox));
//			}
//			else
//			{
//				m_pControlContainer->AddItem(static_cast<SControl*>(pListBox));
//			}
//
//			if (m_pControlContainer)
//				pListBox->SetEventListener(static_cast<IEventListener*>(m_pControlContainer));
//			else
//				pListBox->SetEventListener(nullptr);
//		}
//		break;
//		default:
//			LOG_WRITELOG("Not Support Control Type [%d]\r\n", nControlType);
//			return 0;
//		}
//	}
//
//	fclose(fpBinary);
//	return 1;
//}

int TMScene::FindID(char* szID)
{
	int nID = 0;
	int bFindID = 0;

	if (!strcmp(szID, "NONE"))
		return 0;

	for (int i = 0; i < 2560; ++i)
	{
		if (!strcmp(szID, g_pObjectManager->m_ResourceList[i].szString))
		{
			nID = g_pObjectManager->m_ResourceList[i].nNumber;
			bFindID = 1;
			break;
		}
	}

	if (!bFindID)
		LOG_WRITELOG("Cannot Match Resource ID [%s] in [%s].\r\n", szID, "UI\\TMResource.h");

	return nID;
}

int TMScene::InitializeScene()
{
	SetFocus(g_pApp->m_hWnd);
	return 1;
}

int TMScene::OnPacketEvent(unsigned int dwCode, char* pSBuffer)
{
	if (g_pCurrentScene != this)
		return 0;

	auto pStd = reinterpret_cast<MSG_STANDARD*>(pSBuffer);
	auto dwServerTime = g_pTimerManager->GetServerTime();

	//if (!pSBuffer && (!m_dwDelayDisconnectTime || m_dwDelayDisconnectTime + 15000000000000000 < dwServerTime))
	//{
	//	if (!m_pMessagePanel->IsVisible())
	//	{
	//		m_pMessagePanel->SetMessage(g_pMessageStringTable[13], 4000u);
	//		m_pMessagePanel->SetVisible(1, 1);
	//	}

	//	if (m_eSceneType != ESCENE_TYPE::ESCENE_LOGIN)
	//		g_pObjectManager->SetCurrentState(ObjectManager::TM_GAME_STATE::TM_SELECTSERVER_STATE);

	//	m_dwDelayDisconnectTime = 0;
	//	return 1;
	//}


	if (!pStd)
	{
		if(m_pMyHuman != nullptr)
		{
			if (pReconectData.PosX <= 0)
			{
				pReconectData.PosX = (int)m_pMyHuman->m_vecPosition.x;
			}
			if (pReconectData.PosY <= 0)
			{
				pReconectData.PosY = (int)m_pMyHuman->m_vecPosition.y;
			}

			char strs[56]{};
			sprintf(strs, "X: %d - Y:%d", pReconectData.PosX, pReconectData.PosY);

			m_pReconnectPanel->m_pPosicaoInfo->SetText((char*)strs, 0);

			m_pReconnectPanel->UpdateMacroCC(1);
		}

		if (!m_pMessagePanel->IsVisible())
		{
			m_pMessagePanel->SetMessage(g_pMessageStringTable[13], 4000u);
			m_pMessagePanel->SetVisible(1, 1);
		}

		if (m_eSceneType != ESCENE_TYPE::ESCENE_LOGIN && this->m_pMyHuman)
		{
			pReconectData.PosX = (int)m_pMyHuman->m_vecPosition.x;
			pReconectData.PosY = (int)m_pMyHuman->m_vecPosition.y; 
			g_pObjectManager->SetCurrentState(ObjectManager::TM_GAME_STATE::TM_SELECTSERVER_STATE);
		}

		m_dwDelayDisconnectTime = 0;
		return 1;
	}

	if (pStd->Type == 0x194)
	{
		g_pObjectManager->m_bBilling = 1;
		//m_pMessageBox->SetMessage(g_pMessageStringTable[132], B_CREATE_ID, nullptr);
		m_pMessageBox->SetVisible(1);
		return 1;
	}

	if (pStd->Type == MSG_Encode_Opcode)
	{
		MSG_Encode* pEncode = reinterpret_cast<MSG_Encode*>(pStd);

		int EncodeByte1 = *(DWORD*)&pEncode->Parm[40];
		int EncodeByte2 = *(DWORD*)&pEncode->Parm[41];

		int type = (3 * (EncodeByte1 / 7) + 7 * (EncodeByte2 / 3) + 220) % 3;
		if (type)
		{
			if (type == 1)
				pStd->Type = 0x13BD;
			else if (type == 2)
				pStd->Type = 0x7BE;
		}
		else
			pStd->Type = 0xFBC;
		return 1;
	}

	if (pStd->Type == 0xFBC)
	{
		// need to decompile here... and other encode packets
		return 1;
	}

	if (pStd->Type == 0x13BD)
	{
		// need to decompile here... and other encode packets
		return 1;
	}

	if (pStd->Type == 0x7BE)
	{
		// need to decompile here... and other encode packets
		return 1;
	}
	if (!pStd->ID && (pStd->Type == MSG_MessagePanel_Opcode || pStd->Type == 0x102 || pStd->Type == 0x104 || pStd->Type == 0x105 || pStd->Type == 0x106))
	{
		char szStr[128] = { 0 };
		if (pStd->Type != MSG_MessagePanel_Opcode)
		{
			if (pStd->Type == 0x105)
			{
				auto pMessageChat = reinterpret_cast<MSG_MessageChat*>(pStd);
				if (!pMessageChat->String[0])
				{
					char str[128]{};
					char num[5]{};

					int index = *reinterpret_cast<short*>(&pMessageChat->String[2]) + 1000;
					g_pMessageStringTable[index][127] = 0;
					g_pMessageStringTable[index][126] = 0;

					strcpy(str, g_pMessageStringTable[index]);
					if (strlen(str) < 1)
						strcpy(str, _itoa(*reinterpret_cast<short*>(&pMessageChat->String[2]), num, 10));

					if (index == 465 || index == 466 || index == 485 || index == 484)
						m_pMessagePanel->SetMessage(str, 600000);
					else
						m_pMessagePanel->SetMessage(str, 4000);

					m_pMessagePanel->SetVisible(1, 1);
				}
			}
			else if (pStd->Type == 0x106)
			{
				auto pMessageChat = reinterpret_cast<MSG_MessageChat*>(pStd);
				if (!pMessageChat->String[0])
				{
					char szStr[128]{};
					char szParse[6][128]{};
					int Param = 0;
					strcpy(szStr, &pMessageChat->String[4]);

					for (int k = 0; k < 128; ++k)
					{
						if (szStr[k] == ',')
						{
							szStr[k] = ' ';
							++Param;
						}
						if (!szStr[k])
							break;
					}

					sscanf(szStr, "%s %s %s %s %s %s", szParse[0], szParse[1], szParse[2], szParse[3], szParse[4], szParse[5]);


					int index = *reinterpret_cast<short*>(&pMessageChat->String[2]) + 1000;

					memset(szStr, 0, sizeof(szStr));
					sprintf(szStr, g_pMessageStringTable[index], szParse[0], szParse[1], szParse[2], szParse[3], szParse[4], szParse[5]);

					char buffer[4]{};

					if (strlen(szStr) < 1)
						strcpy(szStr, _itoa(*reinterpret_cast<short*>(&pMessageChat->String[2]), buffer, 10));

					if (index == 465 || index == 466 || index == 485 || index == 484)
						m_pMessagePanel->SetMessage(szStr, 600000);
					else
						m_pMessagePanel->SetMessage(szStr, 4000);

					m_pMessagePanel->SetVisible(1, 1);
				}
			}

			return 1;
		}

		auto pMsgPanel = reinterpret_cast<MSG_MessagePanel*>(pStd);
		pMsgPanel->String[127] = 0;
		pMsgPanel->String[126] = 0;

		if (m_eSceneType == ESCENE_TYPE::ESCENE_SELCHAR && pMsgPanel->String[0] == '^')
		{
			char szMsg[128]{ 0 };
			sprintf_s(szMsg, "%s", &pMsgPanel->String[1]);

			m_pMessageBox2->SetMessage(szMsg, 0, nullptr);
			m_pMessageBox2->SetVisible(1);
		}
		else if (pMsgPanel->String[0] == '^')
		{
			char szMsg[128]{ 0 };
			sprintf_s(szMsg, "%s", &pMsgPanel->String[1]);

			m_pMessagePanel->SetMessage(szMsg, 7000);
			m_pMessagePanel->SetVisible(1, 1);
		}
		else if (pMsgPanel->String[0] == '!' && pMsgPanel->String[1] == '#')
		{
			for (int i = 2; i < 6; i++)
			{
				if (pMsgPanel->String[i] < '0' || pMsgPanel->String[i] > '9')
					pMsgPanel->String[i] = '0';
			}

			TMFieldScene* pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
			pFScene->m_nYear = static_cast<unsigned short>(pMsgPanel->String[3] - 48)
				+ 10 * static_cast<unsigned short>(pMsgPanel->String[2] - 48);
			pFScene->m_nDays = static_cast<unsigned short>(pMsgPanel->String[6] - 48)
				+ 10 * static_cast<unsigned short>(pMsgPanel->String[5] - 48)
				+ 100 * static_cast<unsigned short>(pMsgPanel->String[4] - 48);

			return 1;
		}
		else if (pMsgPanel->String[1] == '!' && pMsgPanel->String[2] == '!' && pMsgPanel->String[3] == '!')
		{
			auto pFocusedObject = static_cast<TMObject*>(m_pMyHuman);

			if ((int)pFocusedObject->m_vecPosition.x >> 7 == 31 && (int)pFocusedObject->m_vecPosition.y >> 7 == 31)
			{
				for (int nType = 0; nType < 5; ++nType)
				{
					for (int j = 0; j < 5; ++j)
					{
						auto pFireWork = new TMEffectFireWork({ (pFocusedObject->m_vecPosition.x) - 10.0f + (5.0f * nType), 7.0f, ((pFocusedObject->m_vecPosition.y - 10.0f) + 5.0f * j) + 8.0f }, nType);

						g_pCurrentScene->AddChild(pFireWork);
					}
				}
			}

			if (pFocusedObject->IsInTown() == 1 || (int)pFocusedObject->m_vecPosition.x >> 7 != 31 || (int)pFocusedObject->m_vecPosition.y >> 7 != 31)
			{
				m_pMessagePanel->SetMessage(pMsgPanel->String, 4000);
				m_pMessagePanel->SetVisible(1, 1);
			}
		}
		else if (pMsgPanel->String[1] == '!' && pMsgPanel->String[2] == '!' && pMsgPanel->String[3] == '#')
		{
			if (g_pCurrentScene->m_eSceneType == ESCENE_TYPE::ESCENE_FIELD && pMsgPanel->String[4] == 'E')
			{
				auto pKilled = static_cast<TMHuman*>(g_pObjectManager->GetHumanByID(static_cast<TMFieldScene*>(g_pCurrentScene)->m_dwKhepraID));

				if (pKilled)
				{
					pKilled->m_stScore.Hp = 0;
					pKilled->Die();
				}

				static_cast<TMFieldScene*>(g_pCurrentScene)->m_dwKhepraID = 0;

				for (int iSt = 0; iSt < 5; ++iSt)
					pMsgPanel->String[iSt] = '_';
			}
		}
		else if (pMsgPanel->String[0] == '!' || pMsgPanel->String[1] == '!')
		{
			if (g_pCurrentScene->m_eSceneType == ESCENE_TYPE::ESCENE_FIELD)
			{
				for (int n = 2; n < 8; ++n)
				{
					if (pMsgPanel->String[n] < '0' || pMsgPanel->String[n] > '9')
						pMsgPanel->String[n] = '0';
				}

				TMFieldScene* pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
				pFScene->m_NightmareTime.wHour = pMsgPanel->String[3] - 48 + 10 * (pMsgPanel->String[2] - 48);
				pFScene->m_NightmareTime.wMonth = pMsgPanel->String[5] - 48 + 10 * (pMsgPanel->String[4] - 48);
				pFScene->m_NightmareTime.wSecond = pMsgPanel->String[7] - 48 + 10 * (pMsgPanel->String[6] - 48);
				pFScene->m_dwLastNightmareTime = g_pTimerManager->GetServerTime();
				return 1;
			}
		}
		else if (pMsgPanel->String[0] == '!')
		{
			SYSTEMTIME sysTime;
			GetLocalTime(&sysTime);

			if (g_pCurrentScene->m_eSceneType == ESCENE_TYPE::ESCENE_FIELD)
			{
				auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

				auto pItem3 = new SListBoxItem(" ", 0xFFFFFFFF, 0.0f, 0.0f, 300.0f, 16.0f, 0, 0x77777777u, 1u, 0);
				pFScene->m_pHelpList[3]->AddItem(pItem3);

				char szTime[128]{ 0 };
				char _Buffer[128]{ 0 };

				sprintf_s(szTime, "SMS [%02d:%02d:%02d]", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
				auto pItem = new SListBoxItem(szTime, 0xFFBBFFFF, 0.0f, 0.0f, 300.0f, 16.0f, 0, 0x77777777u, 1u, 0);
				pFScene->m_pHelpList[3]->AddItem(pItem);

				sprintf_s(_Buffer, "%s", &pMsgPanel->String[1]);

				auto pItem2 = new SListBoxItem(_Buffer, 0xFFBBFFCC, 0.0f, 0.0f, 300.0f, 16.0f, 0, 0x77777777u, 1u, 0);
				pFScene->m_pHelpList[3]->AddItem(pItem2);

				if (pFScene->m_pHelpMemo)
					pFScene->m_pHelpMemo->SetVisible(1);
			}
			else
			{
				for (int l = 0; l < 98; ++l)//
				{
					if (!g_pObjectManager->m_stMemo[l].szString[0] && !g_pObjectManager->m_stMemo[l + 1].szString[0])
					{
						g_pObjectManager->m_stMemo[l].dwColor = -1;
						sprintf_s(g_pObjectManager->m_stMemo[l].szString, "");
						g_pObjectManager->m_stMemo[l + 1].dwColor = 0xFFBBFFFF;
						sprintf_s(g_pObjectManager->m_stMemo[l + 1].szString, "SMS [%02d:%02d:%02d]", sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

						g_pObjectManager->m_stMemo[l + 2].dwColor = 0xFFBBFFCC;
						sprintf_s(g_pObjectManager->m_stMemo[l + 2].szString, "%s", &pMsgPanel->String[1]);
					}
				}
			}

			m_pMessagePanel->SetMessage(&pMsgPanel->String[1], 4000u);
			m_pMessagePanel->SetVisible(1, 1);
		}
		else if (pMsgPanel->String[0] == '2' && pMsgPanel->String[1] == '0' && pMsgPanel->String[2] == '0')
		{
			char Msg[128]{ 0 };
			if (m_pMyHuman)
				sprintf_s(Msg, "%s %d %d %d", pMsgPanel->String, g_pObjectManager->m_nServerIndex, static_cast<int>(m_pMyHuman->m_vecPosition.x),
					static_cast<int>(m_pMyHuman->m_vecPosition.y));
			else
				sprintf_s(Msg, "%s %d", pMsgPanel->String, g_pObjectManager->m_nServerIndex);

			m_pMessagePanel->SetMessage(Msg, 4000u);
			m_pMessagePanel->SetVisible(1, 1);
		}
		else
		{
			m_pMessagePanel->SetMessage(pMsgPanel->String, 4000u);
			m_pMessagePanel->SetVisible(1, 1);
		}

		SListBox* pChatList = static_cast<SListBox*>(m_pControlContainer->FindControl(65667));

		int len = strlen(pMsgPanel->String);
		int size = 50;
		if (len > size)
		{
			char szMsg2[128]{};
			char szMsg3[128]{};

			if (IsClearString(pMsgPanel->String, size - 1))
			{
				strncpy(szMsg3, pMsgPanel->String, size);
				sprintf(szMsg2, "%s", &pMsgPanel->String[size]);
			}
			else
			{
				strncpy(szMsg3, pMsgPanel->String, size - 1);
				sprintf(szMsg2, "%s", &pMsgPanel->String[size - 1]);
			}

			SListBoxItem* ipNewItem = new SListBoxItem(szMsg3, 0xFFCCAAFF, 0.0f, 0.0f, 280.0f, 16.0f, 0, 0x77777777, 1u, 0);
			if (ipNewItem && pChatList)
				pChatList->AddItem(ipNewItem);

			SListBoxItem* ipNewItem2 = new SListBoxItem(szMsg2, 0xFFCCAAFF, 0.0f, 0.0f, 280.0f, 16.0, 0, 0x77777777, 1u, 0);
			if (ipNewItem2 && pChatList)
				pChatList->AddItem(ipNewItem2);
		}
		else
		{
			if (g_pCurrentScene->m_eSceneType == ESCENE_TYPE::ESCENE_FIELD)
			{
				TMFieldScene* pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
				if (!strcmp(pMsgPanel->String, "Whisper : Off"))
				{
					sprintf(pMsgPanel->String, "%s%s", g_pMessageStringTable[448], g_pMessageStringTable[447]);
					pFScene->SetWhisper(0);
				}
				if (!strcmp(pMsgPanel->String, "Whisper : On"))
				{
					sprintf(pMsgPanel->String, "%s%s", g_pMessageStringTable[448], g_pMessageStringTable[446]);
					pFScene->SetWhisper(1);
				}
				if (!strcmp(pMsgPanel->String, "Citizen Chatting : Off"))
				{
					sprintf(pMsgPanel->String, "%s%s", g_pMessageStringTable[449], g_pMessageStringTable[447]);
					pFScene->SetPartyChat(0);
				}
				if (!strcmp(pMsgPanel->String, "Citizen Chatting : On"))
				{
					sprintf(pMsgPanel->String, "%s%s", g_pMessageStringTable[449], g_pMessageStringTable[446]);
					pFScene->SetPartyChat(1);
				}
				if (!strcmp(pMsgPanel->String, "Guild Chatting : Off"))
				{
					sprintf(pMsgPanel->String, "%s%s", g_pMessageStringTable[450], g_pMessageStringTable[447]);
					pFScene->SetGuildChat(0);
				}
				if (!strcmp(pMsgPanel->String, "Guild Chatting : On"))
				{
					sprintf(pMsgPanel->String, "%s%s", g_pMessageStringTable[450], g_pMessageStringTable[446]);
					pFScene->SetGuildChat(1);
				}
				if (!strcmp(pMsgPanel->String, "Kingdom Chatting : On"))
				{
					sprintf(pMsgPanel->String, "%s%s", g_pMessageStringTable[451], g_pMessageStringTable[446]);
					pFScene->SetKingDomChat(1);
				}
				if (!strcmp(pMsgPanel->String, "Kingdom Chatting : Off"))
				{
					sprintf(pMsgPanel->String, "%s%s", g_pMessageStringTable[451], g_pMessageStringTable[447]);
					pFScene->SetKingDomChat(0);
				}
			}

			SListBoxItem* ipNewItem = new SListBoxItem(pMsgPanel->String, 0xFFCCAAFF, 0.0f, 0.0f, 280.0f, 16.0f, 0, 0x77777777, 1u, 0);
			if (ipNewItem && pChatList)
				pChatList->AddItem(ipNewItem);
		}

		SControl* pLoginOK = m_pControlContainer->FindControl(65873);
		if (pLoginOK)
			pLoginOK->SetEnable(1);
		return 1;
	}
	if (pStd->Type == MSG_MessageShout_Opcode)
	{
		pD1D* pShoutMessage = reinterpret_cast<pD1D*>(pStd);
		pShoutMessage->eMsg[111] = 0;
		pShoutMessage->eMsg[110] = 0;

		TMFieldScene* pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
		if (!pFScene->m_pChatGeneral || pFScene->GetSceneType() != ESCENE_TYPE::ESCENE_FIELD || pFScene->m_pCHP->m_bSelectEnable)
		{
			int nIndex = 0;
			unsigned int dwColor = pShoutMessage->Color; //0xFF00CD00;

			SListBox* pChatList = pFScene->m_pChatList;

			int nLen = strlen(pShoutMessage->eMsg);

			SListBoxItem* ipNewItem = new SListBoxItem(pShoutMessage->eMsg,
				pShoutMessage->Color,
				0.0f,
				0.0f,
				(float)nLen * 6.6999998f,
				16.0f, //testa a cor ai agr
				0,
				0xFFFFFF00,
				1u,
				0);

			ipNewItem->m_bBGColor = 0;
			if (ipNewItem && pChatList)
				pChatList->AddItem(ipNewItem);

			pFScene->m_dwChatTime = g_pTimerManager->GetServerTime();
			return 1;
		}
		else
			return 1;
	}
	if (pStd->Type == 0x7DB)
	{
		MSG_STANDARDPARM* m = reinterpret_cast<MSG_STANDARDPARM*>(pStd);

		g_pApp->china_bWrite = 1;
		g_pApp->china_Playtime = m->Parm / 60;
		LOG_WRITELOG("get start Time : %d \r\n", g_pApp->china_Playtime);
		return 1;
	}

	if (m_pControlContainer && m_pControlContainer->OnPacketEvent(dwCode, pSBuffer) == 1)
		return 1;

	TreeNode::OnPacketEvent(dwCode, pSBuffer);
	return 0;
}

int TMScene::OnKeyDownEvent(unsigned int iKeyCode)
{
	if (g_pCurrentScene != this)
		return 0;

	if (m_pControlContainer && m_pControlContainer->OnKeyDownEvent(iKeyCode) == 1)
		return 1;

	TreeNode::OnKeyDownEvent(iKeyCode);
	return 0;
}

int TMScene::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (g_pCurrentScene != this)
		return 0;

	if (m_pControlContainer && m_pControlContainer->OnMouseEvent(dwFlags, wParam, nX, nY) == 1)
		return 1;

	TreeNode::OnMouseEvent(dwFlags, wParam, nX, nY);
	return 0;
}

int TMScene::OnKeyUpEvent(unsigned int iKeyCode)
{

	if (g_pCurrentScene != this)
		return 0;

	if (m_pControlContainer && m_pControlContainer->OnKeyUpEvent(iKeyCode) == 1)
		return 1;

	TMFieldScene* pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	if (m_eSceneType == ESCENE_TYPE::ESCENE_FIELD && pFScene->m_pDropList && pFScene->m_pDropList->m_bVisible)
	{
		auto ListBox = (SListBox*)pFScene->m_pControlContainer->FindControl(478472);
		auto GridDrop = (SGridControl*)pFScene->m_pControlContainer->FindControl(478473);
		GridDrop->Empty();
		int selected = ListBox->m_nSelectedItem;
		int total = ListBox->m_nNumItem;

		if (selected < 0 || selected >= total || !total) return 0;
		return (pFScene->OnKeySelectMobSexy(iKeyCode, selected) == 1);
	}

	TreeNode::OnKeyUpEvent(iKeyCode);
	return 0;
}

int TMScene::OnCharEvent(char iCharCode, int lParam)
{

	if (g_pCurrentScene != this)
		return 0;

	if (m_pControlContainer && m_pControlContainer->OnCharEvent(iCharCode, lParam) == 1)
		return 1;

	return TreeNode::OnCharEvent(iCharCode, lParam);
}

int TMScene::OnIMEEvent(char* ipComposeString)
{
	if (g_pCurrentScene != this)
		return 0;

	if (m_pControlContainer && m_pControlContainer->OnIMEEvent(ipComposeString) == 1)
		return 1;

	return 0;
}

int TMScene::OnChangeIME()
{
	return m_pControlContainer && m_pControlContainer->OnChangeIME() == 1;
}

int TMScene::OnAccel(int nMsg)
{
	return 1;
}

int TMScene::FrameMove(unsigned int dwServerTime)
{
	if (g_pCurrentScene != this)
		return 1;

	if (m_pControlContainer)
		m_pControlContainer->FrameMove(dwServerTime);

	if (m_sPlayDemo >= 0)
		CameraAction();

	if (m_bCriticalError == 1)
		return 1;

	m_pMouseOverHuman = nullptr;
	m_pMouseOverItem = nullptr;

	auto pFocusedObject = static_cast<TMObject*>(m_pMyHuman);

	if (pFocusedObject && m_pGround)
	{
		float dX = pFocusedObject->m_vecPosition.x - m_pGround->m_vecOffset.x;
		float dY = pFocusedObject->m_vecPosition.y - m_pGround->m_vecOffset.y;

		int nShold = 14;

		if (((int)pFocusedObject->m_vecPosition.x >> 7) > 26 &&
			((int)pFocusedObject->m_vecPosition.x >> 7) < 31 &&
			((int)pFocusedObject->m_vecPosition.y >> 7) > 20 &&
			((int)pFocusedObject->m_vecPosition.y >> 7) < 25)
		{
			nShold = 20;
		}

		if (m_pGround->m_vecOffsetIndex.x == 17 && m_pGround->m_vecOffsetIndex.y == 10)
			nShold = 20;

		if (dX >= 0.0f && (float)nShold > dX)
		{
			if (!m_pGround->m_pLeftGround)
				GroundNewAttach(EDirection::EDIR_LEFT);
		}
		else if (dX < 0.0f && dX >(float) - nShold)
		{
			if (m_pGround->m_pLeftGround)
			{
				m_pGround = m_pGround->m_pLeftGround;

				m_nCurrentGroundIndex = (m_nCurrentGroundIndex + 1) % 2;

				m_pGround->SetMiniMapData();
			}
		}
		else if (dX > (float)(128 - nShold) && dX < 128.0f)
		{
			if (!m_pGround->m_pRightGround)
				GroundNewAttach(EDirection::EDIR_RIGHT);
		}
		else if (dX >= 128.0f && (float)(nShold + 128) > dX && m_pGround->m_pRightGround)
		{
			m_pGround = m_pGround->m_pRightGround;

			m_nCurrentGroundIndex = (m_nCurrentGroundIndex + 1) % 2;

			m_pGround->SetMiniMapData();
		}

		if (dY >= 0.0f && (float)nShold > dY)
		{
			if (!m_pGround->m_pUpGround)
				GroundNewAttach(EDirection::EDIR_UP);
		}
		else if (dY < 0.0f && dY >(float) - nShold)
		{
			if (m_pGround->m_pUpGround)
			{
				m_pGround = m_pGround->m_pUpGround;

				m_nCurrentGroundIndex = (m_nCurrentGroundIndex + 1) % 2;

				m_pGround->SetMiniMapData();
			}
		}
		else if (dY > (float)(128 - nShold) && (float)128 > dY)
		{
			if (!m_pGround->m_pDownGround)
				GroundNewAttach(EDirection::EDIR_DOWN);
		}
		else if (dY >= (float)128 && (float)(nShold + 128) > dY && m_pGround->m_pDownGround)
		{
			m_pGround = m_pGround->m_pDownGround;

			m_nCurrentGroundIndex = (m_nCurrentGroundIndex + 1) % 2;

			m_pGround->SetMiniMapData();
		}
	}

	return 1;
}

int TMScene::ReloadScene()
{
	if (m_pControlContainer != nullptr)
	{
		delete m_pControlContainer;

		m_pControlContainer = nullptr;
	}

	return InitializeScene();
}

ESCENE_TYPE TMScene::GetSceneType()
{
	return m_eSceneType;
}

void TMScene::Cleanup()
{
	;
}

char heightMapData[128][128]{};
int TMScene::GroundNewAttach(EDirection eDir)
{
	memset(heightMapData, 0, sizeof(heightMapData));
	if (!m_pGround)
		return 0;

	int x{};
	int y{};

	if (eDir == EDirection::EDIR_LEFT)
	{
		if (m_pGround->m_pLeftGround || m_pGround->m_cLeftEnable != 1)
			return 0;

		x = m_pGround->m_vecOffsetIndex.x - 1;
		y = m_pGround->m_vecOffsetIndex.y;
	}
	else if (eDir == EDirection::EDIR_RIGHT)
	{
		if (m_pGround->m_pRightGround || m_pGround->m_cRightEnable != 1)
			return 0;

		x = m_pGround->m_vecOffsetIndex.x + 1;
		y = m_pGround->m_vecOffsetIndex.y;
	}
	else if (eDir == EDirection::EDIR_UP)
	{
		if (m_pGround->m_pUpGround || m_pGround->m_cUpEnable != 1)
			return 0;

		x = m_pGround->m_vecOffsetIndex.x;
		y = m_pGround->m_vecOffsetIndex.y - 1;
	}
	else if (eDir == EDirection::EDIR_DOWN)
	{
		if (m_pGround->m_pDownGround || m_pGround->m_cDownEnable != 1)
			return 0;

		x = m_pGround->m_vecOffsetIndex.x;
		y = m_pGround->m_vecOffsetIndex.y + 1;
	}

	char fileNameTrn[128]{};
	char fileNameDat[128]{};

	sprintf_s(fileNameTrn, "Env\\Field%02d%02d.trn", x, y);
	sprintf_s(fileNameDat, "Env\\Field%02d%02d.dat", x, y);

	int gId = (m_nCurrentGroundIndex + 1) % 2;

	if (m_pGroundList[gId])
	{
		if (m_pGroundList[gId]->m_vecOffsetIndex.x == m_pGround->m_vecOffsetIndex.x + 1 &&
			m_pGroundList[gId]->m_vecOffsetIndex.y == m_pGround->m_vecOffsetIndex.y)
		{
			for (int i = 0; i < 128; ++i)
				memcpy(heightMapData[i], m_HeightMapData[i], 128);
		}
		else if (m_pGroundList[gId]->m_vecOffsetIndex.x == m_pGround->m_vecOffsetIndex.x &&
			m_pGroundList[gId]->m_vecOffsetIndex.y == m_pGround->m_vecOffsetIndex.y + 1)
		{
			for (int i = 0; i < 128; ++i)
				memcpy(heightMapData[i], m_HeightMapData[i], 128);
		}
		else if (m_pGroundList[gId]->m_vecOffsetIndex.x == m_pGround->m_vecOffsetIndex.x - 1 &&
			m_pGroundList[gId]->m_vecOffsetIndex.y == m_pGround->m_vecOffsetIndex.y)
		{
			for (int i = 0; i < 128; ++i)
				memcpy(heightMapData[i], &m_HeightMapData[i][128], 128);
		}
		else if (m_pGroundList[gId]->m_vecOffsetIndex.x == m_pGround->m_vecOffsetIndex.x &&
			m_pGroundList[gId]->m_vecOffsetIndex.y == m_pGround->m_vecOffsetIndex.y - 1)
		{
			for (int i = 0; i < 128; ++i)
				memcpy(heightMapData[i], m_HeightMapData[i + 128], 128);
		}
	}
	else
	{
		for (int i = 0; i < 128; ++i)
			memcpy(heightMapData[i], m_HeightMapData[i], 128);
	}

	auto pGround = new TMGround();

	if (!pGround->LoadTileMap(fileNameTrn))
	{
		delete pGround;
		return 0;
	}

	if (m_pGroundList[gId])
	{
		delete m_pGroundList[gId];

		m_pGroundList[gId] = nullptr;
	}

	if (m_pObjectContainerList[gId])
	{
		delete m_pObjectContainerList[gId];

		m_pObjectContainerList[gId] = nullptr;
	}

	m_pGroundList[gId] = pGround;

	m_pGroundObjectContainer->AddChild(m_pGroundList[gId]);

	if (m_pGroundList[m_nCurrentGroundIndex])
		m_pGroundList[m_nCurrentGroundIndex]->Attach(m_pGroundList[gId]);

	m_pObjectContainerList[gId] = new TMObjectContainer(m_pGroundList[gId]);

	if (!m_pObjectContainerList[gId]->Load(fileNameDat))
	{
		if (m_pObjectContainerList[gId])
		{
			delete m_pObjectContainerList[gId];

			m_pObjectContainerList[gId] = nullptr;
		}

		LOG_WRITELOG("DataFile Not Found : %s\r\n", fileNameDat);

		if (!m_bCriticalError)
			LogMsgCriticalError(12, 0, 0, 0, 0);

		m_bCriticalError = 1;
		return 0;
	}

	if (m_pObjectContainerList[m_nCurrentGroundIndex])
	{
		if (eDir == EDirection::EDIR_DOWN)
			m_pObjectContainerList[m_nCurrentGroundIndex]->SetPrevNode(m_pObjectContainerList[gId]);
		else
			m_pObjectContainerList[m_nCurrentGroundIndex]->SetNextNode(m_pObjectContainerList[gId]);
	}

	g_pTextureManager->ReleaseNotUsingTexture();

	memset(m_HeightMapData, 0, 4);

	switch (eDir)
	{
	case EDirection::EDIR_LEFT:
		for (int i = 0; i < 128; ++i)
		{
			memcpy(m_HeightMapData[i], m_pGround->m_pLeftGround->m_pMaskData[i], 128);
			memcpy(&m_HeightMapData[i][128], heightMapData[i], 128);
		}
		g_HeightPosX = (int)m_pGround->m_pLeftGround->m_vecOffset.x;
		g_HeightPosY = (int)m_pGround->m_pLeftGround->m_vecOffset.y;
		break;
	case EDirection::EDIR_RIGHT:
		for (int i = 0; i < 128; ++i)
		{
			memcpy(&m_HeightMapData[i][128], m_pGround->m_pRightGround->m_pMaskData[i], 128);
			memcpy(m_HeightMapData[i], heightMapData[i], 128);
		}
		g_HeightPosX = (int)m_pGround->m_vecOffset.x;
		g_HeightPosY = (int)m_pGround->m_vecOffset.y;
		break;
	case EDirection::EDIR_UP:
		for (int i = 0; i < 128; ++i)
		{
			memcpy(&m_HeightMapData[i], m_pGround->m_pUpGround->m_pMaskData[i], 128);
			memcpy(&m_HeightMapData[i + 128], heightMapData[i], 128);
		}
		g_HeightPosX = (int)m_pGround->m_pUpGround->m_vecOffset.x;
		g_HeightPosY = (int)m_pGround->m_pUpGround->m_vecOffset.y;
		break;
	case EDirection::EDIR_DOWN:
		for (int i = 0; i < 128; ++i)
		{
			memcpy(&m_HeightMapData[i + 128], m_pGround->m_pDownGround->m_pMaskData[i], 128);
			memcpy(&m_HeightMapData[i], heightMapData[i], 128);
		}
		g_HeightPosX = (int)m_pGround->m_vecOffset.x;
		g_HeightPosY = (int)m_pGround->m_vecOffset.y;
		break;
	}

	BASE_ApplyAttribute((char*)m_HeightMapData, 256);

	memcpy(m_GateMapData, m_HeightMapData, sizeof(m_HeightMapData));

	SaveHeightMap(fileNameTrn);

	m_pGround->SetMiniMapData();

	m_nAdjustTime = 0;
	m_dwInitTime = g_pTimerManager->GetServerTime();

	return 1;
}

D3DXVECTOR3 TMScene::GroundGetPickPos()
{
	D3DXVECTOR3 vPickPos{ 0.0f, -10000.0f, 0.0f };
	D3DXVECTOR3 vPickTempPos{ 0.0f, -10000.0f, 0.0f };
	D3DXVECTOR3 vFocusePos{ 0.0f, -9000.0f, 0.0f };

	auto pFocusedObject = static_cast<TMObject*>(m_pMyHuman);

	if (pFocusedObject)
	{
		vFocusePos.x = pFocusedObject->m_vecPosition.x;
		vFocusePos.y = pFocusedObject->m_fHeight;
		vFocusePos.z = pFocusedObject->m_vecPosition.x;
	}

	if (!m_pGround)
		return vPickPos;

	vPickTempPos = m_pGround->GetPickPos();

	if (fabsf(vFocusePos.y - vPickTempPos.y) < 4.0f)
		vPickPos = vPickTempPos;

	if (vPickPos.y >= -5000.0f && fabsf(vFocusePos.y - vPickPos.y) < 2.0f)
		return vPickPos;

	if (m_pGround->m_pLeftGround)
	{
		vPickTempPos = m_pGround->m_pLeftGround->GetPickPos();

		if (fabsf(vFocusePos.y - vPickPos.y) > fabsf(vFocusePos.y - vPickTempPos.y) || fabsf(vFocusePos.y - vPickTempPos.y) < 4.0f)
		{
			vPickPos = vPickTempPos;

			if (vPickTempPos.y > -5000.0f && fabsf(vFocusePos.y - vPickPos.y) < 2.0f)
				return vPickPos;
		}
	}

	if (m_pGround->m_pRightGround)
	{
		vPickTempPos = m_pGround->m_pRightGround->GetPickPos();

		if (fabsf(vFocusePos.y - vPickPos.y) > fabsf(vFocusePos.y - vPickTempPos.y))
		{
			vPickPos = vPickTempPos;

			if (vPickTempPos.y > -5000.0f && fabsf(vFocusePos.y - vPickPos.y) < 2.0f || fabsf(vFocusePos.y - vPickTempPos.y < 4.0f))
				return vPickPos;
		}
	}

	if (m_pGround->m_pUpGround)
	{
		vPickTempPos = m_pGround->m_pUpGround->GetPickPos();

		if (fabsf(vFocusePos.y - vPickPos.y) > fabsf(vFocusePos.y - vPickTempPos.y))
		{
			vPickPos = vPickTempPos;

			if (vPickTempPos.y > -5000.0f && fabsf(vFocusePos.y - vPickPos.y) < 2.0f || fabsf(vFocusePos.y - vPickTempPos.y) < 4.0f)
				return vPickPos;
		}
	}

	if (m_pGround->m_pDownGround)
	{
		vPickTempPos = m_pGround->m_pDownGround->GetPickPos();

		if (fabsf(vFocusePos.y - vPickPos.y) > fabsf(vFocusePos.y - vPickTempPos.y))
		{
			vPickPos = vPickTempPos;

			if (vPickTempPos.y > -5000.0 && fabsf(vFocusePos.y - vPickPos.y) < 2.0f || fabsf(vFocusePos.y - vPickTempPos.y) < 4.0f)
				return vPickPos;

		}
	}

	return vPickPos;
}

int TMScene::GroundGetTileType(TMVector2 vecPosition)
{
	int nTileType{};

	if (m_pGround)
	{
		if (vecPosition.x >= m_pGround->m_vecOffset.x &&
			vecPosition.x < (m_pGround->m_vecOffset.x + 128.0f) &&
			vecPosition.y >= m_pGround->m_vecOffset.y &&
			vecPosition.y < (m_pGround->m_vecOffset.y + 128.0f))
		{
			nTileType = m_pGround->GetTileType(vecPosition);
		}
		else if (m_pGround->m_pLeftGround
			&& vecPosition.x >= m_pGround->m_pLeftGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pLeftGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pLeftGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pLeftGround->m_vecOffset.y + 128.0f))
		{
			nTileType = m_pGround->m_pLeftGround->GetTileType(vecPosition);
		}
		else if (m_pGround->m_pRightGround
			&& vecPosition.x >= m_pGround->m_pRightGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pRightGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pRightGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pRightGround->m_vecOffset.y + 128.0f))
		{
			nTileType = m_pGround->m_pRightGround->GetTileType(vecPosition);
		}
		else if (m_pGround->m_pUpGround
			&& vecPosition.x >= m_pGround->m_pUpGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pUpGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pUpGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pUpGround->m_vecOffset.y + 128.0f))
		{
			nTileType = m_pGround->m_pUpGround->GetTileType(vecPosition);
		}
		else if (m_pGround->m_pDownGround
			&& vecPosition.x >= m_pGround->m_pDownGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pDownGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pDownGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pDownGround->m_vecOffset.y + 128.0f))
		{
			nTileType = m_pGround->m_pDownGround->GetTileType(vecPosition);
		}
	}

	return nTileType;
}

int TMScene::GroundGetMask(TMVector2 vecPosition)
{
	int nXIndex = (int)vecPosition.x - g_HeightPosX;
	int nYIndex = (int)vecPosition.y - g_HeightPosY;

	if (nXIndex < 0)
		nXIndex = 0;

	if (nYIndex < 0)
		nYIndex = 0;

	if (nXIndex > 256)
		nXIndex = 255;

	if (nYIndex > 256)
		nYIndex = 255;

	int value = m_HeightMapData[nYIndex][nXIndex];
	return m_HeightMapData[nYIndex][nXIndex];
}

int TMScene::GroundGetMask(IVector2 vecPosition)
{
	int nXIndex = vecPosition.x - g_HeightPosX;
	int nYIndex = vecPosition.y - g_HeightPosY;

	if (nXIndex < 0)
		nXIndex = 0;

	if (nYIndex < 0)
		nYIndex = 0;

	if (nXIndex > 256)
		nXIndex = 255;

	if (nYIndex > 256)
		nYIndex = 255;

	return m_HeightMapData[nYIndex][nXIndex];
}

float TMScene::GroundGetHeight(TMVector2 vecPosition)
{
	float fHeight{ -10000.0f };

	if (m_pGround != nullptr)
	{
		if (vecPosition.x >= m_pGround->m_vecOffset.x &&
			vecPosition.x < (m_pGround->m_vecOffset.x + 128.0f) &&
			vecPosition.y >= m_pGround->m_vecOffset.y &&
			vecPosition.y < (m_pGround->m_vecOffset.y + 128.0f))
		{
			fHeight = m_pGround->GetHeight(vecPosition);
		}
		else if (m_pGround->m_pLeftGround
			&& vecPosition.x >= m_pGround->m_pLeftGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pLeftGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pLeftGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pLeftGround->m_vecOffset.y + 128.0f))
		{
			fHeight = m_pGround->m_pLeftGround->GetHeight(vecPosition);
		}
		else if (m_pGround->m_pRightGround
			&& vecPosition.x >= m_pGround->m_pRightGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pRightGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pRightGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pRightGround->m_vecOffset.y + 128.0f))
		{
			fHeight = m_pGround->m_pRightGround->GetHeight(vecPosition);
		}
		else if (m_pGround->m_pUpGround
			&& vecPosition.x >= m_pGround->m_pUpGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pUpGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pUpGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pUpGround->m_vecOffset.y + 128.0f))
		{
			fHeight = m_pGround->m_pUpGround->GetHeight(vecPosition);
		}
		else if (m_pGround->m_pDownGround
			&& vecPosition.x >= m_pGround->m_pDownGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pDownGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pDownGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pDownGround->m_vecOffset.y + 128.0f))
		{
			fHeight = m_pGround->m_pDownGround->GetHeight(vecPosition);
		}
	}

	return fHeight;
}

D3DCOLORVALUE TMScene::GroundGetColor(TMVector2 vecPosition)
{
	D3DCOLORVALUE color{ 1.0f, 1.0f, 1.0f, 1.0f };

	if (m_pGround != nullptr)
	{
		if (vecPosition.x >= m_pGround->m_vecOffset.x &&
			vecPosition.x < (m_pGround->m_vecOffset.x + 128.0f) &&
			vecPosition.y >= m_pGround->m_vecOffset.y &&
			vecPosition.y < (m_pGround->m_vecOffset.y + 128.0f))
		{
			color = m_pGround->GetColor(vecPosition);
		}

		else if (m_pGround->m_pLeftGround
			&& vecPosition.x >= m_pGround->m_pLeftGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pLeftGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pLeftGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pLeftGround->m_vecOffset.y + 128.0f))
		{
			color = m_pGround->m_pLeftGround->GetColor(vecPosition);
		}

		else if (m_pGround->m_pRightGround
			&& vecPosition.x >= m_pGround->m_pRightGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pRightGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pRightGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pRightGround->m_vecOffset.y + 128.0f))
		{
			color = m_pGround->m_pRightGround->GetColor(vecPosition);
		}

		else if (m_pGround->m_pUpGround
			&& vecPosition.x >= m_pGround->m_pUpGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pUpGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pUpGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pUpGround->m_vecOffset.y + 128.0f))
		{
			color = m_pGround->m_pUpGround->GetColor(vecPosition);
		}

		else if (m_pGround->m_pDownGround
			&& vecPosition.x >= m_pGround->m_pDownGround->m_vecOffset.x
			&& vecPosition.x < (m_pGround->m_pDownGround->m_vecOffset.x + 128.0f)
			&& vecPosition.y >= m_pGround->m_pDownGround->m_vecOffset.y
			&& vecPosition.y < (m_pGround->m_pDownGround->m_vecOffset.y + 128.0f))
		{
			color = m_pGround->m_pDownGround->GetColor(vecPosition);
		}
	}

	return color;
}

void TMScene::GroundSetColor(TMVector2 vecPosition, unsigned int dwColor)
{
	if (!m_pGround)
		return;

	if (vecPosition.x >= m_pGround->m_vecOffset.x && vecPosition.x < (m_pGround->m_vecOffset.x + 128.0f) &&
		vecPosition.y >= m_pGround->m_vecOffset.y && vecPosition.y < (m_pGround->m_vecOffset.y + 128.0f))
	{
		m_pGround->SetColor(vecPosition, dwColor);
	}
	else if (m_pGround->m_pLeftGround
		&& vecPosition.x >= m_pGround->m_pLeftGround->m_vecOffset.x
		&& vecPosition.x < (m_pGround->m_pLeftGround->m_vecOffset.x + 128.0f)
		&& vecPosition.y >= m_pGround->m_pLeftGround->m_vecOffset.y
		&& vecPosition.y < (m_pGround->m_pLeftGround->m_vecOffset.y + 128.0f))
	{
		m_pGround->m_pLeftGround->SetColor(vecPosition, dwColor);
	}
	else if (m_pGround->m_pRightGround
		&& vecPosition.x >= m_pGround->m_pRightGround->m_vecOffset.x
		&& vecPosition.x < (m_pGround->m_pRightGround->m_vecOffset.x + 128.0f)
		&& vecPosition.y >= m_pGround->m_pRightGround->m_vecOffset.y
		&& vecPosition.y < (m_pGround->m_pRightGround->m_vecOffset.y + 128.0f))
	{
		m_pGround->m_pRightGround->SetColor(vecPosition, dwColor);
	}
	else if (m_pGround->m_pUpGround
		&& vecPosition.x >= m_pGround->m_pUpGround->m_vecOffset.x
		&& vecPosition.x < (m_pGround->m_pUpGround->m_vecOffset.x + 128.0f)
		&& vecPosition.y >= m_pGround->m_pUpGround->m_vecOffset.y
		&& vecPosition.y < (m_pGround->m_pUpGround->m_vecOffset.y + 128.0f))
	{
		m_pGround->m_pUpGround->SetColor(vecPosition, dwColor);
	}
	else if (m_pGround->m_pDownGround
		&& vecPosition.x >= m_pGround->m_pDownGround->m_vecOffset.x
		&& vecPosition.x < (m_pGround->m_pDownGround->m_vecOffset.x + 128.0f)
		&& vecPosition.y >= m_pGround->m_pDownGround->m_vecOffset.y
		&& vecPosition.y < (m_pGround->m_pDownGround->m_vecOffset.y + 128.0f))
	{
		m_pGround->m_pDownGround->SetColor(vecPosition, dwColor);
	}
}

int TMScene::GroundIsInWater(TMVector2 vecPosition, float fHeight, float* pfWaterHeight)
{
	if (!m_pGround)
		return 0;

	if (vecPosition.x >= m_pGround->m_vecOffset.x && (float)(m_pGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_vecOffset.y && (float)(m_pGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->IsInWater(vecPosition, fHeight, pfWaterHeight);
	}
	if (m_pGround->m_pLeftGround &&
		vecPosition.x >= m_pGround->m_pLeftGround->m_vecOffset.x && (float)(m_pGround->m_pLeftGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_pLeftGround->m_vecOffset.y && (float)(m_pGround->m_pLeftGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pLeftGround->IsInWater(vecPosition, fHeight, pfWaterHeight);
	}
	if (m_pGround->m_pRightGround
		&& vecPosition.x >= m_pGround->m_pRightGround->m_vecOffset.x && (float)(m_pGround->m_pRightGround->m_vecOffset.x + 128.0f) > vecPosition.x
		&& vecPosition.y >= m_pGround->m_pRightGround->m_vecOffset.y && (float)(m_pGround->m_pRightGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pRightGround->IsInWater(vecPosition, fHeight, pfWaterHeight);
	}
	if (m_pGround->m_pUpGround
		&& vecPosition.x >= m_pGround->m_pUpGround->m_vecOffset.x && (float)(m_pGround->m_pUpGround->m_vecOffset.x + 128.0f) > vecPosition.x
		&& vecPosition.y >= m_pGround->m_pUpGround->m_vecOffset.y && (float)(m_pGround->m_pUpGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pUpGround->IsInWater(vecPosition, fHeight, pfWaterHeight);
	}
	if (m_pGround->m_pDownGround
		&& vecPosition.x >= m_pGround->m_pDownGround->m_vecOffset.x && (float)(m_pGround->m_pDownGround->m_vecOffset.x + 128.0f) > vecPosition.x
		&& vecPosition.y >= m_pGround->m_pDownGround->m_vecOffset.y && (float)(m_pGround->m_pDownGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pDownGround->IsInWater(vecPosition, fHeight, pfWaterHeight);
	}

	return 0;
}

int TMScene::GroundIsInWater2(TMVector2 vecPosition, float* pfWaterHeight)
{
	if (!m_pGround)
		return 0;

	if (vecPosition.x >= m_pGround->m_vecOffset.x && (float)(m_pGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_vecOffset.y && (float)(m_pGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->IsInWater(vecPosition, (float)GroundGetMask(vecPosition) * 0.1f, pfWaterHeight);
	}
	if (m_pGround->m_pLeftGround &&
		vecPosition.x >= m_pGround->m_pLeftGround->m_vecOffset.x && (float)(m_pGround->m_pLeftGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_pLeftGround->m_vecOffset.y && (float)(m_pGround->m_pLeftGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pLeftGround->IsInWater(vecPosition, (float)GroundGetMask(vecPosition) * 0.1f, pfWaterHeight);
	}
	if (m_pGround->m_pRightGround &&
		vecPosition.x >= m_pGround->m_pRightGround->m_vecOffset.x && (float)(m_pGround->m_pRightGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_pRightGround->m_vecOffset.y && (float)(m_pGround->m_pRightGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pRightGround->IsInWater(vecPosition, (float)GroundGetMask(vecPosition) * 0.1f, pfWaterHeight);
	}
	if (m_pGround->m_pUpGround &&
		vecPosition.x >= m_pGround->m_pUpGround->m_vecOffset.x && (float)(m_pGround->m_pUpGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_pUpGround->m_vecOffset.y && (float)(m_pGround->m_pUpGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pUpGround->IsInWater(vecPosition, (float)GroundGetMask(vecPosition) * 0.1f, pfWaterHeight);
	}
	if (!m_pGround->m_pDownGround ||
		vecPosition.x < m_pGround->m_pDownGround->m_vecOffset.x || (float)(m_pGround->m_pDownGround->m_vecOffset.x + 128.0f) <= vecPosition.x ||
		vecPosition.y < m_pGround->m_pDownGround->m_vecOffset.y || (float)(m_pGround->m_pDownGround->m_vecOffset.y + 128.0f) <= vecPosition.y)
	{
		return 0;
	}

	return m_pGround->m_pDownGround->IsInWater(vecPosition, (float)GroundGetMask(vecPosition) * 0.1f, pfWaterHeight);
}

float TMScene::GroundGetWaterHeight(TMVector2 vecPosition, float* pfWaterHeight)
{
	if (!m_pGround)
		return -100.0f;

	if (vecPosition.x >= m_pGround->m_vecOffset.x && (float)(m_pGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_vecOffset.y && (float)(m_pGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->GetWaterHeight(vecPosition, pfWaterHeight);
	}
	if (m_pGround->m_pLeftGround &&
		vecPosition.x >= m_pGround->m_pLeftGround->m_vecOffset.x && (float)(m_pGround->m_pLeftGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_pLeftGround->m_vecOffset.y && (float)(m_pGround->m_pLeftGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pLeftGround->GetWaterHeight(vecPosition, pfWaterHeight);
	}
	if (m_pGround->m_pRightGround &&
		vecPosition.x >= m_pGround->m_pRightGround->m_vecOffset.x && (float)(m_pGround->m_pRightGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_pRightGround->m_vecOffset.y && (float)(m_pGround->m_pRightGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pRightGround->GetWaterHeight(vecPosition, pfWaterHeight);
	}
	if (m_pGround->m_pUpGround &&
		vecPosition.x >= m_pGround->m_pUpGround->m_vecOffset.x && (float)(m_pGround->m_pUpGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_pUpGround->m_vecOffset.y && (float)(m_pGround->m_pUpGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pUpGround->GetWaterHeight(vecPosition, pfWaterHeight);
	}
	if (m_pGround->m_pDownGround &&
		vecPosition.x >= m_pGround->m_pDownGround->m_vecOffset.x && (float)(m_pGround->m_pDownGround->m_vecOffset.x + 128.0f) > vecPosition.x &&
		vecPosition.y >= m_pGround->m_pDownGround->m_vecOffset.y && (float)(m_pGround->m_pDownGround->m_vecOffset.y + 128.0f) > vecPosition.y)
	{
		return m_pGround->m_pDownGround->GetWaterHeight(vecPosition, pfWaterHeight);
	}

	return -100.0f;
}

int TMScene::GetMask2(TMVector2 vecPosition)
{
	int nMaskX = (int)(vecPosition.x - (float)g_HeightPosX);
	int nMaskY = (int)(vecPosition.y - (float)g_HeightPosY);

	if (nMaskX >= 0 && nMaskY >= 0 && nMaskX < 256 && nMaskY < 256)
		return m_GateMapData[nMaskY][nMaskX];

	return -10000;
}

void TMScene::Warp()
{
	if (m_bCriticalError == 1)
		return;

	auto pFocusedObject = static_cast<TMObject*>(m_pMyHuman);

	if (pFocusedObject)
	{
		if (m_pGround)
			Warp2((int)(pFocusedObject->m_vecPosition.x / 128.0f), (int)(pFocusedObject->m_vecPosition.y / 128.0f));
	}
}

void TMScene::Warp2(int nZoneX, int nZoneY)
{
	if (m_bCriticalError == 1 || !m_pGround)
		return;

	m_bAutoRun = 0;
	if (m_eSceneType == ESCENE_TYPE::ESCENE_FIELD && static_cast<TMFieldScene*>(this)->m_pAutoRunBtn)
		static_cast<TMFieldScene*>(this)->m_pAutoRunBtn->SetSelected(m_bAutoRun);

	TMGround* pNeighbor = nullptr;
	if (m_pGround->m_pLeftGround)
		pNeighbor = m_pGround->m_pLeftGround;
	if (m_pGround->m_pRightGround)
		pNeighbor = m_pGround->m_pRightGround;
	if (m_pGround->m_pUpGround)
		pNeighbor = m_pGround->m_pUpGround;
	if (m_pGround->m_pDownGround)
		pNeighbor = m_pGround->m_pDownGround;

	if (!pNeighbor &&
		(nZoneX != m_pGround->m_vecOffsetIndex.x || nZoneY != m_pGround->m_vecOffsetIndex.y) ||
		pNeighbor &&
		(nZoneX != m_pGround->m_vecOffsetIndex.x && nZoneX != pNeighbor->m_vecOffsetIndex.x ||
			nZoneY != m_pGround->m_vecOffsetIndex.y && nZoneY != pNeighbor->m_vecOffsetIndex.y))
	{
		char szMapPath[128]{};
		char szDataPath[128]{};
		sprintf(szMapPath, "env\\Field%02d%02d.trn", nZoneX, nZoneY);
		sprintf(szDataPath, "env\\Field%02d%02d.dat", nZoneX, nZoneY);

		TMGround* pGround = new TMGround();

		if (!pGround->LoadTileMap(szMapPath))
		{
			if (!m_bCriticalError)
				LogMsgCriticalError(10, 0, 0, 0, 0);

			m_bCriticalError = 1;
			return;
		}

		TMGround* pOldGround = m_pGround;
		m_pGround = pGround;

		for (int i = 0; i < 2; ++i)
		{
			SAFE_DELETE(m_pObjectContainerList[i]);
			SAFE_DELETE(m_pGroundList[i]);
		}

		g_HeightPosX = nZoneX << 7;
		g_HeightPosY = nZoneY << 7;

		m_nCurrentGroundIndex = 0;
		m_pGroundList[0] = m_pGround;

		m_pObjectContainerList[0] = new TMObjectContainer(m_pGround);

		if (!m_pObjectContainerList[0]->Load(szDataPath))
		{
			LOG_WRITELOG("DataFile Not Found : %s\r\n", szDataPath);
			if (!m_bCriticalError)
				LogMsgCriticalError(11, 0, 0, 0, 0);

			m_bCriticalError = 1;
			return;
		}

		memset(m_HeightMapData, 0, 4u);

		for (int nY = 0; nY < 128; ++nY)
			memcpy(m_HeightMapData[nY], m_pGround->m_pMaskData[nY], 128);

		m_pGround->SetMiniMapData();
		m_pGroundObjectContainer->AddChild(m_pObjectContainerList[0]);
		m_pGroundObjectContainer->AddChild(m_pGroundList[0]);

		BASE_ApplyAttribute((char*)m_HeightMapData, 256);

		memcpy(m_GateMapData, m_HeightMapData, sizeof(m_GateMapData));

		SaveHeightMap(szMapPath);
		m_nAdjustTime = 0;
		m_dwInitTime = g_pTimerManager->GetServerTime();
	}
	else if (pNeighbor && nZoneX == pNeighbor->m_vecOffsetIndex.x && nZoneY == pNeighbor->m_vecOffsetIndex.y)
	{
		m_pGround = pNeighbor;
		m_nCurrentGroundIndex = (m_nCurrentGroundIndex + 1) % 2;
		m_pGround->SetMiniMapData();
	}
	if (m_eSceneType == ESCENE_TYPE::ESCENE_FIELD)
	{
		auto pSoundManager = g_pSoundManager;
		if (pSoundManager)
		{
			auto pSoundData = pSoundManager->GetSoundData(6);
			if (pSoundData && pSoundData->IsSoundPlaying())
			{
				pSoundData->Stop();
			}
		}
		if (RenderDevice::m_bDungeon && RenderDevice::m_bDungeon != 3 && RenderDevice::m_bDungeon != 4)
		{
			g_nWeather = 0;
			static_cast<TMFieldScene*>(this)->m_pRain->m_bVisible = 0;
			static_cast<TMFieldScene*>(this)->m_pSnow->m_bVisible = 0;
			static_cast<TMFieldScene*>(this)->m_pSnow2->m_bVisible = 0;
			static_cast<TMFieldScene*>(this)->m_bQuater = 1;
			static_cast<TMFieldScene*>(this)->UpdateScoreUI(0);
			static_cast<TMFieldScene*>(this)->m_pMiniMapPanel->m_GCPanel.dwColor = 0x80FFFFFF;
		}
		else
		{
			if (RenderDevice::m_bDungeon == 3 || RenderDevice::m_bDungeon == 4)
			{
				static_cast<TMFieldScene*>(this)->m_pSnow->m_bVisible = 0;
				static_cast<TMFieldScene*>(this)->m_pSnow2->m_bVisible = 0;
			}

			static_cast<TMFieldScene*>(this)->SetWeather(g_nWeather);
			g_pObjectManager->m_pCamera->m_fHorizonAngle = 0.78539819f;
			static_cast<TMFieldScene*>(this)->m_pMiniMapPanel->m_GCPanel.dwColor = 0x80FFFFFF;
		}
	}
}

void TMScene::SaveHeightMap(char* szFileName)
{
	;
}

void TMScene::CameraAction()
{
	if (m_dwStartCamTime == 0)
		return;

	DWORD dwTick = (g_pTimerManager->GetServerTime() - m_dwStartCamTime) + 100;

	TMCamera* pCamera = g_pObjectManager->m_pCamera;

	pCamera->m_fSightLength = 11.0f;

	for (int i = 0; i < (m_nCameraLoop - 1); ++i)
	{
		if (m_stCameraTick[i].dwTick >= dwTick || m_stCameraTick[i + 1].dwTick <= dwTick)
			continue;

		float fRatio =
			(float)(dwTick - m_stCameraTick[i].dwTick) /
			(float)(m_stCameraTick[i + 1].dwTick - m_stCameraTick[i].dwTick);

		auto fHorizonAngle = ((1.0f - fRatio) * m_stCameraTick[i].fHorizonAngle) + (m_stCameraTick[i + 1].fHorizonAngle * fRatio);

		pCamera->m_fHorizonAngle = fHorizonAngle;
		pCamera->m_fBackHorizonAngle = fHorizonAngle;

		auto fVerticalAngle = ((1.0f - fRatio) * m_stCameraTick[i].fVerticalAngle) + (m_stCameraTick[i + 1].fVerticalAngle * fRatio);

		pCamera->m_fVerticalAngle = fVerticalAngle;
		pCamera->m_fBackVerticalAngle = fVerticalAngle;

		TMVector3 vecLoc1{ 0.0f, 0.0f, 0.0f };
		TMVector3 vecLoc2{ 0.0f, 0.0f, 0.0f };

		if (m_stCameraTick[i].sLocal == 1 && m_pMyHuman != nullptr)
		{
			vecLoc1.x = m_pMyHuman->m_vecPosition.x;
			vecLoc1.y = m_pMyHuman->m_fHeight;
			vecLoc1.z = m_pMyHuman->m_vecPosition.y;
		}

		if (m_stCameraTick[i + 1].sLocal == 1 && m_pMyHuman != nullptr)
		{
			vecLoc2.x = m_pMyHuman->m_vecPosition.x;
			vecLoc2.y = m_pMyHuman->m_fHeight;
			vecLoc2.z = m_pMyHuman->m_vecPosition.y;
		}

		pCamera->m_cameraPos.x = ((vecLoc1.x + m_stCameraTick[i].fX) * (1.0f - fRatio))
			+ ((vecLoc2.x + m_stCameraTick[i + 1].fX) * fRatio);

		pCamera->m_cameraPos.y = ((vecLoc1.y + m_stCameraTick[i].fY) * (1.0f - fRatio))
			+ ((vecLoc2.y + m_stCameraTick[i + 1].fY) * fRatio);

		pCamera->m_cameraPos.z = ((vecLoc1.z + m_stCameraTick[i].fZ) * (1.0f - fRatio))
			+ ((vecLoc2.z + m_stCameraTick[i + 1].fZ) * fRatio);

		return;
	}

	if (m_stCameraTick[m_nCameraLoop - 1].dwTick < dwTick)
	{
		pCamera->m_fHorizonAngle = m_stCameraTick[m_nCameraLoop - 1].fHorizonAngle;
		pCamera->m_fVerticalAngle = m_stCameraTick[m_nCameraLoop - 1].fVerticalAngle;

		TMVector3 vecLoc3{ 0.0f, 0.0f, 0.0f };

		if (m_pMyHuman && m_stCameraTick[m_nCameraLoop - 1].sLocal == 1)
		{
			vecLoc3.x = m_pMyHuman->m_vecPosition.x;
			vecLoc3.y = m_pMyHuman->m_fHeight;
			vecLoc3.z = m_pMyHuman->m_vecPosition.y;
		}

		pCamera->m_cameraPos.x = m_stCameraTick[m_nCameraLoop - 1].fX + vecLoc3.x;
		pCamera->m_cameraPos.y = m_stCameraTick[m_nCameraLoop - 1].fY + vecLoc3.y;
		pCamera->m_cameraPos.z = m_stCameraTick[m_nCameraLoop - 1].fZ + vecLoc3.z;
	}
}

void TMScene::ReadCameraPos(const char* szFileName)
{
	m_nCameraLoop = 0;

	memset(m_stCameraTick, 0, sizeof(m_stCameraTick));

	char szBinFileName[128]{};

	sprintf_s(szBinFileName, "%s.bin", szFileName);

	FILE* fpBin = nullptr;

	fopen_s(&fpBin, szBinFileName, "rb");

	fread(&m_nCameraLoop, 1, 4, fpBin);

	if (m_nCameraLoop > 1000)
		m_nCameraLoop = 1000;

	else if (m_nCameraLoop < 0)
		m_nCameraLoop = 0;

	fread(m_stCameraTick, 1, 28 * m_nCameraLoop, fpBin);
	fclose(fpBin);
}

int TMScene::LoadMsgText(SListBox* pListBox, const char* szFileName)
{
	FILE* fp{};

	fopen_s(&fp, szFileName, "rt");

	if (!fp)
		return 0;

	if (!pListBox)
		return 0;

	char szText[256]{};
	char szTemp[256]{};

	for (int i = 0; i < 100 && fgets(szTemp, 256, fp); ++i)
	{
		char szCol[7]{};

		strncpy(szCol, szTemp, 6);

		DWORD dwCol{};

		sscanf(szCol, "%x", &dwCol);

		char* szRet = strstr(szTemp, "\n");

		if (szRet)
			*szRet = 0;

		if (szTemp[6] == 32)
		{
			sprintf_s(szText, "%s", &szTemp[6]);

			pListBox->AddItem(new SListBoxItem(
				szText,
				dwCol | 0xFF000000,
				0.0f,
				0.0f,
				pListBox->m_nWidth,
				16.0f,
				0,
				0x77777777u,
				1u,
				0));
		}
	}

	if (pListBox->m_pScrollBar)
		pListBox->m_pScrollBar->SetCurrentPos(0);

	fclose(fp);
	return 1;
}

int TMScene::LoadMsgText2(SListBox* pListBox, const char* szFileName, int nStartLine, int nEndLine)
{
	FILE* fp{};

	fopen_s(&fp, szFileName, "rt");

	if (!fp)
		return 0;

	if (!pListBox)
		return 0;

	char szText[256]{};
	char szTemp[256]{};

	int nCount = 0;

	pListBox->Empty();

	for (int i = 0; i < 560 && fgets(szTemp, 256, fp); ++i)
	{
		char szCol[7]{};

		strncpy(szCol, szTemp, 6);

		DWORD dwCol{};

		sscanf(szCol, "%x", &dwCol);

		char* szRet = strstr(szTemp, "\n");

		if (szRet)
			*szRet = 0;

		if (szTemp[6] == 32 && i >= nStartLine && i <= nEndLine)
		{
			sprintf_s(szText, "%s", &szTemp[6]);

			pListBox->AddItem(new SListBoxItem(
				szText,
				dwCol | 0xFF000000,
				0.0f,
				0.0f,
				pListBox->m_nWidth,
				16.0f,
				0,
				0x77777777u,
				1u,
				0));

			if (++nCount > 100)
				break;
		}
	}

	if (pListBox->m_pScrollBar)
		pListBox->m_pScrollBar->SetCurrentPos(0);

	fclose(fp);
	return 1;
}

int TMScene::LoadMsgText3(SListBox* pListBox, const char* szFileName, int nLv, int ntrans)
{
	FILE* fp{};

	fopen_s(&fp, szFileName, "rt");

	if (!fp)
		return 0;

	if (!pListBox)
		return 0;

	char szText[256]{};
	char szTemp[256]{};

	int nLvLimit = 300;

	pListBox->Empty();

	for (int i = 0; i < 100 && fgets(szTemp, 256, fp); ++i)
	{
		char szCol[11]{};

		strncpy(szCol, szTemp, 10);

		DWORD dwCol{};

		sscanf_s(szCol, "%d %x", &nLvLimit, &dwCol);

		if (nLvLimit > nLv)
			dwCol = 0xFF777777;

		if (nLvLimit > 350 && ntrans < 6)
			dwCol = 0xFF777777;

		if (g_pCurrentScene)
		{
			if (m_pMyHuman->Is2stClass() == 2)
				dwCol = 0xFF777777;

			else if (nLvLimit > nLv)
				dwCol = 0xFF777777;
		}

		char* szRet = strstr(szTemp, "\n");

		if (szRet)
			*szRet = 0;

		if (szTemp[10] == 32)
		{
			sprintf(szText, "%s", &szTemp[10]);

			pListBox->AddItem(new SListBoxItem(
				szText,
				dwCol | 0xFF000000,
				0.0f,
				0.0f,
				pListBox->m_nWidth,
				16.0f,
				0,
				0x77777777u,
				1u,
				0));
		}
	}

	if (pListBox->m_pScrollBar)
		pListBox->m_pScrollBar->SetCurrentPos(0);

	fclose(fp);
	return 1;
}

unsigned int TMScene::LoadMsgText4(char* pStr, int dwStrSize, const char* szFileName, int nLv, int ntrans)
{
	FILE* fp{};

	fopen_s(&fp, szFileName, "rt");

	if (!fp)
		return 0;

	int nLvLimit = 300;
	DWORD dwCol = 0;

	char szTemp[256]{};

	for (int i = 0; i < 100 && fgets(szTemp, 256, fp); ++i)
	{
		char szCol[11]{};

		strncpy(szCol, szTemp, 10);

		sscanf_s(szCol, "%d %x", &nLvLimit, &dwCol);

		if (nLvLimit == nLv)
		{
			if (dwCol == 0xFFAAAA && ntrans >= 6)
			{
				dwCol = 0;
			}
			else if (nLvLimit <= 350 || ntrans >= 6)
			{
				char* szRet = strstr(szTemp, "\n");

				if (szRet)
					*szRet = 0;

				if (szTemp[10] == 32)
					sprintf_s(pStr, dwStrSize, "%s", &szTemp[10]);
			}
			else
			{
				dwCol = 0;
			}

			break;
		}
	}

	fclose(fp);

	return dwCol | 0xFF000000;
}

int TMScene::LoadMsgLevel(char* LevelQuest, const char* szFileName, char cType)
{
	FILE* fp{};

	fopen_s(&fp, szFileName, "rt");

	if (!fp)
		return 0;

	char szTemp[256]{};

	for (int i = 0; i < 100 && fgets(szTemp, 256, fp); ++i)
	{
		char szCol[11]{};

		strncpy(szCol, szTemp, 10);

		int nLvLimit{};
		DWORD dwCol{};

		sscanf_s(szCol, "%d %x", &nLvLimit, &dwCol);

		LevelQuest[nLvLimit - 1] = cType;
	}
	fclose(fp);
	return 1;
}

void TMScene::CheckPKNonePK(int nServerIndex)
{
	g_NonePKServer = 1;
	for (int i = 0; i < 2; ++i)
	{
		if (nServerIndex == g_pPKServerNum[i])
			g_NonePKServer = 0;
	}
	g_NonePKServer = 0;
}

void TMScene::LogMsgCriticalError(int Type, int ID, int nMesh, int X, int Y)
{
	if (!m_pMyHuman)
		return;

	MSG_MessageLog stLog{};
	stLog.Header.ID = m_pMyHuman->m_dwID;
	stLog.Header.Type = MSG_MessageLog_Opcode;

	if (Type == 10)
		sprintf(stLog.String, "00000000 , Load Tile Map Fail");
	else
		sprintf(stLog.String, "%08d , Critical Data Err Cl,%d,%d,%d,%d,%d, %d", ID, nMesh, (int)m_pMyHuman->m_vecPosition.x, (int)m_pMyHuman->m_vecPosition.y,
			X,
			Y,
			Type);

	g_pSocketManager->SendOneMessage((char*)&stLog, sizeof(stLog));
}

void TMScene::DeleteOwnerAllContainer()
{
	m_pEffectContainer->DeleteOwner(nullptr);
	m_pShadeContainer->DeleteOwner(nullptr);
	m_pHumanContainer->DeleteOwner(nullptr);
	m_pItemContainer->DeleteOwner(nullptr);
	m_pExtraContainer->DeleteOwner(nullptr);
}
unsigned int TMScene::GetSceneTypeId(ESCENE_TYPE sceneType)
{
	if (sceneType == ESCENE_TYPE::ESCENE_SELECT_SERVER)
		return 1;
	if (sceneType == ESCENE_TYPE::ESCENE_SELCHAR)
		return 2;
	if (sceneType == ESCENE_TYPE::ESCENE_FIELD)
		return 3;

	return 0;
}


void TMScene::ResizeItem(stBinControlBase* item)
{
	if (item->Id == 65686 && item->nTextureId == 238)
		return;

	auto it = std::find_if(std::begin(g_pApp->m_SceneResize), std::end(g_pApp->m_SceneResize), [&item, this](const stSceneResizeItem& resizeItem) {
		return resizeItem.SceneId == GetSceneTypeId(m_eSceneType) && resizeItem.Id == item->Id;
		});

	if (it == std::end(g_pApp->m_SceneResize))
		return;

	auto pResSize = g_pApp->GetResolutionSize();	 

	item->nStartX = it->PosX;
	item->nStartY = it->PosY;
	item->nHeight = it->Height;
	item->nWidth = it->Width;
}