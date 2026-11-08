#include "pch.h"
#include "EventTranslator.h"
#include "SGrid.h"
#include "SControl.h"
#include "TMGlobal.h"
#include "Basedef.h"
#include "SControlContainer.h"
#include "TMFieldScene.h"
#include "TMSelectCharScene.h" 
#include <WinSock2.h>
#include "TMUtil.h"

unsigned int SControl::m_dwStaticID{ 0 };
int SControl::m_nGridCellSize{ 35 };
int SCursor::m_nCursorType{ 0 };
HCURSOR SCursor::m_hCursor1{};
HCURSOR SCursor::m_hCursor2{};

SControl::SControl(float inPosX, float inPosY, float inWidth, float inHeight)
	: TreeNode(0)
{
	m_bAlwaysOnTop = 0;
	m_bVisible = 1;
	m_bEnable = 1;
	m_bFocused = 0;
	m_bOver = 0;
	m_bDeleteThisObject = 0;
	m_bSelectEnable = 1;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_NONE;
	m_dwControlID = 0;
	m_pEventListener = nullptr;
	m_bModal = 0;

	float fWidthRatio = (float)g_pDevice->m_dwScreenWidth / RenderDevice::m_fResWidthBase;
	float fHeightRatio = (float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fResHeightBase;


	m_nSkillBetSize = GetSkillSize();







	m_nPosX = inPosX * fWidthRatio;
	m_nPosY = inPosY * fHeightRatio;
	m_nWidth = inWidth * fWidthRatio;
	m_nHeight = inHeight * fHeightRatio;
	m_dwUniqueID = SControl::m_dwStaticID++;
}

SControl::~SControl()
{
}

int SControl::OnPacketEvent(unsigned int dwCode, char* buf)
{
	return 0;
}

int SControl::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	return 0;
}

int SControl::OnKeyDownEvent(unsigned int iKeyCode)
{
	return 0;
}

int SControl::OnKeyUpEvent(unsigned int iKeyCode)
{
	return 0;
}

int SControl::OnCharEvent(char iCharCode, int lParam)
{
	return 0;
}

int SControl::OnChangeIME()
{
	return 0;
}

int SControl::OnIMEEvent(char* ipComposeString)
{
	return 0;
}

int SControl::IsIMENative()
{
	return 0;
}

void SControl::SetControlID(unsigned int idwControlID)
{
	m_dwControlID = idwControlID;
}

unsigned int SControl::GetControlID()
{
	return m_dwControlID;
}

unsigned int SControl::GetUniqueID()
{
	return m_dwUniqueID;
}

void SControl::SetEventListener(IEventListener* ipEventListener)
{
	m_pEventListener = ipEventListener;
}

void SControl::Update()
{
}

void SControl::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
}

void SControl::SetAlwaysOnTop(int bAlwaysOnTop)
{
	m_bAlwaysOnTop = bAlwaysOnTop;
}

void SControl::SetVisible(int bVisible)
{
	m_bVisible = bVisible;
}

void SControl::SetEnable(int bEnable)
{
	m_bEnable = bEnable;
}

void SControl::SetFocused(int bFocused)
{
	m_bFocused = bFocused;
}

int SControl::IsVisible()
{
	return m_bVisible;
}

int SControl::IsFocused()
{
	return m_bFocused;
}

int SControl::IsOver()
{
	return m_bOver;
}

TMVector2 SControl::GetPos()
{
	return TMVector2(m_nPosX, m_nPosY);
}

int SControl::ChildCount()
{
	return 0;
}

void SControl::SetPos(float nPosX, float nPosY)
{
	m_nPosX = nPosX * 1.0f;
	m_nPosY = nPosY * 1.0f;
}

void SControl::SetSize(float nWidth, float nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

void SControl::SetRealPos(float nPosX, float nPosY)
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

void SControl::SetRealSize(float nWidth, float nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

void SControl::SetAutoSize()
{
	m_nPosX = (float)(m_nPosX / 800.0f) * (float)g_pDevice->m_dwScreenWidth;
	m_nPosY = (float)(m_nPosY / 600.0f) * (float)g_pDevice->m_dwScreenHeight;
}

void SControl::SetCenterSize()
{
	m_nPosX = (float)((g_pDevice->m_dwScreenWidth - 800) >> 1) + m_nPosX;
	m_nPosY = (float)((g_pDevice->m_dwScreenHeight - 600) >> 1) + m_nPosY;
}

void SControl::SetStickLeft()
{
	m_nPosX = 0.0f;
}

void SControl::SetHeight(float nHeight)
{
	m_nHeight = nHeight;
}


void SControl::SetStickRight()
{
	m_nPosX = (float)g_pDevice->m_dwScreenWidth - m_nWidth;
}

void SControl::SetStickTop()
{
	m_nPosY = 0.0f;
}

void SControl::SetStickBottom()
{
	m_nPosY = (float)g_pDevice->m_dwScreenHeight - m_nHeight;
}

int SControl::PtInControl(int inPosX, int inPosY)
{
	return PointInRect(inPosX, inPosY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);
}

CONTROL_TYPE SControl::GetControlType()
{
	return CONTROL_TYPE::CTRL_TYPE_NONE;
}

void SControl::SetCenterPos(unsigned int dwControlID, float inPosX, float inPosY, float inWidth, float inHeight)
{
	if (g_pDevice == nullptr)
		return;

	static unsigned int dwCenterUI[6] = { 769, 4622, 65870, 4617, 5638, 0 };

	for (int i = 0; i < 5; ++i)
	{
		if (dwControlID == dwCenterUI[i])
		{
			m_nPosX = ((float)g_pDevice->m_dwScreenWidth * 0.5f) - inWidth * 0.5f;
		}
	}
}

float SControl::GetSkillSize()
{
	auto nResIndex = RenderDevice::m_fResBase;
	switch (nResIndex)
	{
	case 1:
		m_nSkillBetSize = 24.0f;
		break;
	case 2:
		m_nSkillBetSize = 25.5f;
		break;
	case 3:
		m_nSkillBetSize = 26.5f;
		break;
	case 4:
		m_nSkillBetSize = 26.5f;
		break;
	case 5:
		m_nSkillBetSize = 26.5f;
		break;
	}
	return m_nSkillBetSize;
}
// aqui debuga mais rapido
void SControl::ConsolePainelMacro(int indice)
{
	if (m_bOver)
	{
		switch (indice)
		{
			case 1:
			if (g_pEventTranslator->m_bCtrl)
				printf("\n SPanel:: %d,%d,%2.f,%2.f,%2.f,%2.f", (int)g_pCurrentScene->GetSceneType() / 10000, m_dwControlID, m_nPosX, m_nPosY, m_nWidth, m_nHeight);///mostra o handle do painel
			break;
		case 2:
			if (g_pEventTranslator->m_bCtrl)
				printf("\n SText:: %d,%d,%2.f,%2.f,%2.f,%2.f", (int)g_pCurrentScene->GetSceneType() / 10000, m_dwControlID, m_nPosX, m_nPosY, m_nWidth, m_nHeight);///mostra o handle do painel
			break;
		case 3:
			if (g_pEventTranslator->m_bCtrl)
				printf("\n SEditableText:: %d,%d,%2.f,%2.f,%2.f,%2.f", (int)g_pCurrentScene->GetSceneType() / 10000, m_dwControlID, m_nPosX, m_nPosY, m_nWidth, m_nHeight);///mostra o handle do painel
			break;
		case 4:
			if (g_pEventTranslator->m_bCtrl)
				printf("\n SButton:: %d,%d,%2.f,%2.f,%2.f,%2.f", (int)g_pCurrentScene->GetSceneType() / 10000, m_dwControlID, m_nPosX, m_nPosY, m_nWidth, m_nHeight);///mostra o handle do painel
			break;
		case 5:
			if (g_pEventTranslator->m_bCtrl)
				printf("\n SScrollBar:: %d,%d,%2.f,%2.f,%2.f,%2.f", (int)g_pCurrentScene->GetSceneType() / 10000, m_dwControlID, m_nPosX, m_nPosY, m_nWidth, m_nHeight);///mostra o handle do painel
			break;
		case 6:
			if (g_pEventTranslator->m_bCtrl)
				printf("\n SListBox:: %d,%d,%2.f,%2.f,%2.f,%2.f", (int)g_pCurrentScene->GetSceneType() / 10000, m_dwControlID, m_nPosX, m_nPosY, m_nWidth, m_nHeight);///mostra o handle do painel
			break;
		case 7:
			if (g_pEventTranslator->m_bCtrl)
				printf("\n SGrid:: %d,%d,%2.f,%2.f,%2.f,%2.f", (int)g_pCurrentScene->GetSceneType() / 10000, m_dwControlID, m_nPosX, m_nPosY, m_nWidth, m_nHeight);///mostra o handle do painel
			break;

		default:
			break;
		}
	}
	return;
}
SPanel::SPanel(int inTextureSetIndex, float inX, float inY, float inWidth, float inHeight, unsigned int idwColor, RENDERCTRLTYPE eRenderType)
	: SControl(inX, inY, inWidth, inHeight)
{
	m_GCPanel = GeomControl(eRenderType, inTextureSetIndex, 0.0f, 0.0f, inWidth, inHeight, 0, idwColor);
	m_pDescPanel = 0;
	m_bPickable = 0;
	m_bPicked = 0;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_PANEL;
}

SPanel::~SPanel()
{
	SControlContainer* pControlContainer = g_pCurrentScene->m_pControlContainer;

	if (pControlContainer != nullptr && pControlContainer->m_pPickedControl == this)
		pControlContainer->m_pPickedControl = nullptr;

	if (pControlContainer != nullptr && m_GCPanel.nLayer >= 0)
		RemoveRenderControlItem(pControlContainer->m_pDrawControl, &m_GCPanel, m_GCPanel.nLayer);
}

int SPanel::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (m_bSelectEnable == 0)
		return 0;

	if (m_dwControlID == 65943 || m_dwControlID == 65947)
		return 0;

	int bInCaption = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, 24.0f);
	m_bOver = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);

	if (m_bOver == 0 && m_pDescPanel != nullptr)
		m_pDescPanel->SetVisible(0);

	if (m_bOver)
	{
		ConsolePainelMacro(1);
	}







	switch (dwFlags)
	{
	case WM_MOUSEMOVE:
	{
		if (m_bPicked != 0 && m_bPickable != 0)
		{
			m_nPosX = (float)(nX - m_nPickPosX) + m_nPosX;
			m_nPosY = (float)(nY - m_nPickPosY) + m_nPosY;
			if (m_nPosX < 0.0)
				m_nPosX = 0.0;
			if (m_nPosY < 0.0)
				m_nPosY = 0.0;

			if ((float)(m_nPosX + m_nWidth) > (float)g_pApp->m_dwScreenWidth)
				m_nPosX = (float)g_pApp->m_dwScreenWidth - m_nWidth;
			if ((float)(m_nPosY + m_nHeight) > (float)g_pApp->m_dwScreenHeight)
				m_nPosY = (float)g_pApp->m_dwScreenHeight - m_nHeight;

			m_nPickPosX = nX;
			m_nPickPosY = nY;
		}
		if (m_bOver == 1 && (wParam & 1))
			return 1;

		if (m_bOver == 1 && m_pDescPanel != nullptr)
			m_pDescPanel->SetVisible(1);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (m_bPickable != 0 && bInCaption != 0 && g_pCurrentScene->m_pControlContainer->m_pPickedControl == nullptr)
		{
			g_pCurrentScene->m_pControlContainer->m_pPickedControl = this;
			m_bPicked = 1;
			m_nPickPosX = nX;
			m_nPickPosY = nY;
		}
	}
	break;
	case WM_LBUTTONUP:
	{
		if (g_pCurrentScene->m_pControlContainer->m_pPickedControl == this)
			g_pCurrentScene->m_pControlContainer->m_pPickedControl = nullptr;

		if (m_bPickable != 0)
			m_bPicked = 0;
	}
	break;
	case WM_LBUTTONDBLCLK:
	{
		if (m_bModal == 1 && m_bOver == 1)
			return 1;

		return SControl::OnMouseEvent(dwFlags, wParam, nX, nY);
	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (m_bOver == 1)
			return 1;
	}
	break;
	}

	if (m_bOver == 1)
		return 1;

	return m_bModal == 1 && m_bOver == 1;
}

void SPanel::SetTextureSetIndex(int inTextureSetIndex)
{
	m_GCPanel.nTextureSetIndex = inTextureSetIndex;
}

GeomControl* SPanel::GetGeomControl()
{
	return &m_GCPanel;
}

void SPanel::SetVisible(int bVisible)
{
	m_bVisible = bVisible;

	if (m_bVisible == 0 && g_pCurrentScene != nullptr && g_pCurrentScene->m_pControlContainer != nullptr)
	{
		if (g_pCurrentScene->m_pControlContainer->m_pPickedControl == this)
			g_pCurrentScene->m_pControlContainer = nullptr;
	}
}

void SPanel::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	 

	if (m_GCPanel.nTextureSetIndex >= 0 || (m_GCPanel.dwColor & 0xFF000000))
	{
		m_GCPanel.nPosX = ivParentPos.x + m_nPosX;
		m_GCPanel.nPosY = ivParentPos.y + m_nPosY;
		m_GCPanel.nWidth = m_nWidth;
		m_GCPanel.nHeight = m_nHeight;
		m_GCPanel.nLayer = inParentLayer;
		if ((float)(m_GCPanel.nPosX + m_GCPanel.nWidth) >= 0.0f &&
			(float)(m_GCPanel.nPosY + m_GCPanel.nHeight) >= 0.0f &&
			m_GCPanel.nPosX <= (float)(RenderDevice::m_fResWidthBase * RenderDevice::m_fWidthRatio) &&
			m_GCPanel.nPosY <= (float)(RenderDevice::m_fResHeightBase * RenderDevice::m_fHeightRatio))
		{
			AddRenderControlItem(pDrawList, &m_GCPanel, inParentLayer);
		}
	}
}

S3DObj::S3DObj(int nObjIndex, float inX, float inY, float inWidth, float inHeight)
	: SControl(inX, inY, inWidth, inHeight)
{
	m_GCObj = GeomControl(RENDERCTRLTYPE::RENDER_3DOBJ, 0, 0.0f, 0.0f, inWidth, inHeight, 0, 0xFFFFFFFF);
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_3DOBJ;
	m_GCObj.n3DObjIndex = nObjIndex;
}

S3DObj::~S3DObj()
{
	SControlContainer* pControlContainer = g_pCurrentScene->m_pControlContainer;

	if (pControlContainer != nullptr && m_GCObj.nLayer >= 0)
		RemoveRenderControlItem(pControlContainer->m_pDrawControl, &m_GCObj, m_GCObj.nLayer);
}

void S3DObj::SetObjIndex(int nObjIndex)
{
	m_GCObj.n3DObjIndex = nObjIndex;
}

GeomControl* S3DObj::GetGeomControl()
{
	return &m_GCObj;
}

int S3DObj::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (m_bSelectEnable == 0)
		return 0;

	if (dwFlags == WM_MOUSEMOVE)
		return 0;
	 
	return OnMouseEvent(dwFlags, wParam, nX, nY);
}

void S3DObj::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	float fAngle = 0.0f;
	if (m_bOver == 1)
	{
		fAngle = (float)(g_pTimerManager->GetServerTime() % 3000);
		fAngle = (fAngle * 6.28f) / 3000.0f;
	}

	m_GCObj.fAngle = fAngle;
	m_GCObj.nPosX = ivParentPos.x + m_nPosX;
	m_GCObj.nPosY = ivParentPos.y + m_nPosY;
	m_GCObj.nWidth = m_nWidth;
	m_GCObj.nHeight = m_nHeight;
	m_GCObj.nLayer = inParentLayer;

	if ((float)(m_GCObj.nPosX + m_GCObj.nWidth) >= 0.0f &&
		(float)(m_GCObj.nPosY + m_GCObj.nHeight) >= 0.0f &&
		m_GCObj.nPosX <= (float)(RenderDevice::m_fResWidthBase * RenderDevice::m_fWidthRatio) &&
		m_GCObj.nPosY <= (float)(RenderDevice::m_fResHeightBase * RenderDevice::m_fHeightRatio))
	{
		AddRenderControlItem(pDrawList, &m_GCObj, inParentLayer);
	}
}

SCursor::SCursor(int inTextureSetIndex, float inX, float inY, float inWidth, float inHeight)
	: SPanel(inTextureSetIndex, inX, inY, inWidth, inHeight, 0x77777777, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH)
{
	m_eStyle = ECursorStyle::TMC_CURSOR_HAND;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_CURSOR;
	m_GCPanel.nTextureIndex = 0;
	m_nPosX = inX;
	m_nPosY = inY;
	g_pCursor = this;
	m_pAttachedItem = nullptr;
}

SCursor::~SCursor()
{
	if (m_eStyle == ECursorStyle::TMC_CURSOR_PICKUP)
	{
		SControlContainer* pControlContainer = g_pCurrentScene->m_pControlContainer;
		if (pControlContainer != nullptr)
		{
			if (m_GeomItem.nLayer >= 0)
				RemoveRenderControlItem(pControlContainer->m_pDrawControl, &m_GeomItem, m_GeomItem.nLayer);
		}
	}
}

int SCursor::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (dwFlags == WM_MOUSEMOVE)
		SetPosition(nX, nY);

	return SControl::OnMouseEvent(dwFlags, wParam, nX, nY);
}

void SCursor::FrameMove2(stGeomList* pDrawList, TMVector2 ivParenPos, int inParentLayer, int nFlag)
{  
	m_GCPanel.nPosX = m_nPosX;
	m_GCPanel.nPosY = m_nPosY;

	if (SCursor::m_nCursorType == 2)
	{
		m_GCPanel.nPosX = -100.0f;
		m_GCPanel.nPosY = -100.0f;
	}

	m_GCPanel.nWidth = m_nWidth;
	m_GCPanel.nHeight = m_nHeight;
	m_GCPanel.nLayer = inParentLayer;
	m_bAlwaysOnTop = 1;

	if (m_eStyle == ECursorStyle::TMC_CURSOR_PICKUP)
	{
		if (m_pAttachedItem == nullptr)
		{
			m_eStyle = ECursorStyle::TMC_CURSOR_HAND;
			return;
		}

		m_GeomItem = *m_pAttachedItem->GetGeomControl();

		if (m_GeomItem.eRenderType == RENDERCTRLTYPE::RENDER_3DOBJ)
		{
			m_GeomItem.nPosX += m_pAttachedItem->GetPos().x;
			m_GeomItem.nPosY += m_pAttachedItem->GetPos().y;

			m_GeomItem.nLayer = m_GCPanel.nLayer;
			m_GeomItem.fScale = 1.5f;

			unsigned int dwServerTime = g_pTimerManager->GetServerTime() % 3000;
			m_GeomItem.fAngle = ((float)dwServerTime * 6.28f) / 3000.0f;
		}
		else
		{
			m_GeomItem.nPosX = (float)(m_nPosX + m_pAttachedItem->GetPos().x) - 12.0f;
			m_GeomItem.nPosY = (float)(m_nPosY + m_pAttachedItem->GetPos().y) - 12.0f;
			m_GeomItem.nLayer = m_GCPanel.nLayer;
		}

		AddRenderControlItem(pDrawList, &m_GeomItem, inParentLayer);
		AddRenderControlItem(pDrawList, &m_GCPanel, inParentLayer);
	}
	else
	{
		AddRenderControlItem(pDrawList, &m_GCPanel, inParentLayer);
	}
}

void SCursor::SetPosition(int iX, int iY)
{
	m_nPosX = (float)iX;
	m_nPosY = (float)iY;
}

void SCursor::SetVisible(int bVisible)
{
	m_bVisible = bVisible;
}

void SCursor::SetStyle(ECursorStyle eStyle)
{
	m_eStyle = eStyle;
}

ECursorStyle SCursor::GetStyle()
{
	return m_eStyle;
}

GeomControl* SCursor::GetGeomControl()
{
	return &m_GCPanel;
}

int SCursor::AttachItem(SGridControlItem* pItem)
{
	if (pItem == nullptr)
		return 0;
	if (m_eStyle != ECursorStyle::TMC_CURSOR_HAND)
		return 0;

	m_eStyle = ECursorStyle::TMC_CURSOR_PICKUP;
	m_pAttachedItem = pItem;
	return 1;
}

SGridControlItem* SCursor::DetachItem()
{
	SGridControlItem* pItem = m_pAttachedItem;
	m_pAttachedItem = nullptr;

	if (m_eStyle == ECursorStyle::TMC_CURSOR_PICKUP)
		m_eStyle = ECursorStyle::TMC_CURSOR_HAND;

	return pItem;
}

SText::SText(int inTextureSetIndex, const char* istrText, unsigned int idwFontColor, float inX, float inY, float inWidth, float inHeight, int ibBorder, unsigned int idwBorderColor, unsigned int dwType, unsigned int dwAlignType)
	: SControl(inX, inY, inWidth, inHeight)
{
	m_GCText = GeomControl(RENDERCTRLTYPE::RENDER_TEXT, inTextureSetIndex, 0.0f, 0.0f, 0.0f, 0.0f, 0, idwFontColor);
	m_GCText2 = GeomControl(RENDERCTRLTYPE::RENDER_TEXT, inTextureSetIndex, 0.0f, 0.0f, 0.0f, 0.0f, 0, idwFontColor);
	m_GCText3 = GeomControl(RENDERCTRLTYPE::RENDER_TEXT, inTextureSetIndex, 0.0f, 0.0f, 0.0f, 0.0f, 0, idwFontColor);
	m_GCText4 = GeomControl(RENDERCTRLTYPE::RENDER_TEXT, inTextureSetIndex, 0.0f, 0.0f, 0.0f, 0.0f, 0, idwFontColor);

	m_dwAlignType = dwAlignType;
	m_dwTextType = dwType;
	m_cBorder = ibBorder;

	m_GCBorder = GeomControl(RENDERCTRLTYPE::RENDER_IMAGE_STRETCH, inTextureSetIndex, 0.0f, 0.0f, 0.0f, 0.0f, 0, idwBorderColor);

	m_cComma = 0;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_TEXT;

	SetType(m_dwTextType);

	m_GCText.strString[0] = 0;
	m_GCText.pFont = &m_Font;
	m_GCText2.strString[0] = 0;
	m_GCText2.pFont = &m_Font2;
	m_GCText3.strString[0] = 0;
	m_GCText3.pFont = &m_Font3;
	m_GCText4.strString[0] = 0;
	m_GCText4.pFont = &m_Font4;

	if (istrText != nullptr)
	{
		BASE_UnderBarToSpace((char*)istrText);

		if (!strcmp(istrText, " "))
			istrText = nullptr;
	}
	if (istrText != nullptr)
	{
		if (strlen(istrText) != 0)
		{
			strcpy(m_GCText.strString, istrText);
			m_GCText.pFont->SetText(m_GCText.strString, m_GCText.dwColor, 0);
		}
	}
}

SText::~SText()
{
	if (g_pCurrentScene != nullptr)
	{
		SControlContainer* pControlContainer = g_pCurrentScene->m_pControlContainer;
		if (pControlContainer != nullptr)
		{
			if (m_GCBorder.nLayer >= 0)
				RemoveRenderControlItem(pControlContainer->m_pDrawControl, &m_GCBorder, m_GCBorder.nLayer);
			if (m_GCText.nLayer >= 0)
				RemoveRenderControlItem(pControlContainer->m_pDrawControl, &m_GCText, m_GCText.nLayer);

			if (m_cComma == 2)
			{
				if (m_GCText2.nLayer >= 0)
					RemoveRenderControlItem(
						pControlContainer->m_pDrawControl,
						&m_GCText2,
						m_GCText2.nLayer);

				if (m_GCText3.nLayer >= 0)
					RemoveRenderControlItem(
						pControlContainer->m_pDrawControl,
						&m_GCText3,
						m_GCText3.nLayer);

				if (m_GCText4.nLayer >= 0)
					RemoveRenderControlItem(
						pControlContainer->m_pDrawControl,
						&m_GCText4,
						m_GCText4.nLayer);
			}
		}
	}
}

void SText::SetText(char* istrText, int bCheckZero)
{
	if (istrText != nullptr)
	{
		BASE_UnderBarToSpace(istrText);
		if (!strcmp(istrText, " "))
			istrText = 0;
	}

	size_t count = 0;

	if (istrText == nullptr)
	{
		sprintf(m_GCText.strString, "");
		m_Font.SetText(m_GCText.strString, m_GCText.dwColor, bCheckZero);

		return;
	}
	else if (!strcmp(m_GCText.strString, istrText) && !m_cComma)
		return;

	if (strlen(istrText) != 0 && (m_cComma == 2 || m_cComma == 1))
	{
		for (size_t i = 0; i < strlen(istrText); ++i)
		{
			if (!isdigit(istrText[i]) && istrText[i] != ' ')
				++count;
		}

		if (!count && atoi(istrText))
		{
			char str[100]{};
			for (size_t i = 0; i < strlen(istrText); ++i)
			{
				if (istrText[i] != ' ')
					continue;

				++count;
				for (size_t j = i; j < strlen(istrText); ++j)
				{
					istrText[j] = istrText[j + 1];
				}
				i -= 1;
			}

			size_t i = 0;
			int j = 0;
			while (1)
			{
				if (i >= strlen(istrText))
					break;

				if (((strlen(istrText) - i) % 3) || i == 0)
					str[j++] = istrText[i];
				else
				{
					str[j] = ',';
					str[j + 1] = istrText[i];
					j += 2;
				}
				++i;
			}

			strcpy(istrText, str);

			if (count > 0)
			{
				char temp1[128]{};
				char temp2[128]{};
				strcpy(temp1, istrText);

				for (i = 0; i < count; ++i)
					temp2[i] = ' ';

				strcat(temp2, temp1);
				strcpy(istrText, temp2);
			}
		}
	}

	if (strlen(istrText) > 255)
		istrText[255] = 0;

	strcpy(m_GCText.strString, istrText);
	m_Font.SetText(m_GCText.strString, m_GCText.dwColor, bCheckZero);

	if (m_cComma == 2)
	{
		int nStartPos = strlen(istrText);
		memset(m_GCText.strString, 0, sizeof(m_GCText.strString));
		memset(m_GCText2.strString, 0, sizeof(m_GCText2.strString));
		memset(m_GCText3.strString, 0, sizeof(m_GCText3.strString));
		memset(m_GCText4.strString, 0, sizeof(m_GCText4.strString));

		int i = 0;
		int count = 0;
		while (i < nStartPos)
		{
			if (istrText[i] == ',')
				++count;

			if (i >= 255)
				break;

			if (count != 0)
				m_GCText.strString[i] = ' ';
			else
				m_GCText.strString[i] = istrText[i];
			if (count == 1)
				m_GCText2.strString[i] = istrText[i];
			else
				m_GCText2.strString[i] = ' ';
			if (count == 2)
				m_GCText3.strString[i] = istrText[i];
			else
				m_GCText3.strString[i] = ' ';
			if (count <= 2)
				m_GCText4.strString[i] = ' ';
			else
				m_GCText4.strString[i] = istrText[i];

			++i;
		}

		m_Font.SetText(m_GCText.strString, 0xFFDDFFDD, bCheckZero);
		m_Font2.SetText(m_GCText2.strString, 0xFF88FF88, bCheckZero);
		m_Font3.SetText(m_GCText3.strString, 0xFFFFFF66, bCheckZero);
		m_Font4.SetText(m_GCText4.strString, 0xFFFFFFFF, bCheckZero);
	}
}

void SText::SetTextColor(unsigned int dwFontColor)
{
	if (RenderDevice::m_nBright > 58)
	{
		int nR = WYDCOLOR_RED(dwFontColor) - (RenderDevice::m_nBright - 40);
		int nG = WYDCOLOR_GREEN(dwFontColor) - (RenderDevice::m_nBright - 40);
		int nB = WYDCOLOR_BLUE(dwFontColor) - (RenderDevice::m_nBright - 40);

		if (nR < 0)
			nR = 0;
		if (nG < 0)
			nG = 0;
		if (nB < 0)
			nB = 0;

		dwFontColor = nB | (nG << 8) | dwFontColor & 0xFF000000 | (nR << 16);
	}

	m_GCText.dwColor = dwFontColor;
	m_GCText.pFont->m_dwColor = dwFontColor;
}

char* SText::GetText()
{
	return m_GCText.strString;
}

void SText::SetType(unsigned int dwType)
{ 
	m_dwTextType = dwType;
	if (m_dwTextType == 0)
	{
		m_GCText.eRenderType = RENDERCTRLTYPE::RENDER_TEXT;
		m_GCText2.eRenderType = RENDERCTRLTYPE::RENDER_TEXT;
		m_GCText3.eRenderType = RENDERCTRLTYPE::RENDER_TEXT;
		m_GCText4.eRenderType = RENDERCTRLTYPE::RENDER_TEXT;
	}
	else if (m_dwTextType == 1)
	{
		m_GCText.eRenderType = RENDERCTRLTYPE::RENDER_SHADOW;
		m_GCText2.eRenderType = RENDERCTRLTYPE::RENDER_SHADOW;
		m_GCText3.eRenderType = RENDERCTRLTYPE::RENDER_SHADOW;
		m_GCText4.eRenderType = RENDERCTRLTYPE::RENDER_SHADOW;
	}
	else if (m_dwTextType == 2)
	{
		m_GCText.eRenderType = RENDERCTRLTYPE::RENDER_SHADOW;
		m_GCText2.eRenderType = RENDERCTRLTYPE::RENDER_SHADOW;
		m_GCText3.eRenderType = RENDERCTRLTYPE::RENDER_SHADOW;
		m_GCText4.eRenderType = RENDERCTRLTYPE::RENDER_SHADOW;
		m_GCBorder.nTextureSetIndex = -2;
		m_GCText2.nTextureSetIndex = -2;
		m_GCText3.nTextureSetIndex = -2;
		m_GCText4.nTextureSetIndex = -2;
	}
}

void SText::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{ 
	SetType(m_dwTextType);
	 
	if (m_cBorder == 1)
	{
		m_GCBorder.nPosX = ivParentPos.x + m_nPosX;
		m_GCBorder.nPosY = ivParentPos.y + m_nPosY;
		m_GCBorder.nWidth = m_nWidth;
		m_GCBorder.nHeight = m_nHeight;
		m_GCBorder.nLayer = inParentLayer;

		if ((float)(m_GCBorder.nPosX + m_GCBorder.nWidth) < 0.0f ||
			(float)(m_GCBorder.nPosY + m_GCBorder.nHeight) < 0.0f ||
			m_GCBorder.nPosX > (float)(RenderDevice::m_fResWidthBase * RenderDevice::m_fWidthRatio) ||
			m_GCBorder.nPosY > (float)(RenderDevice::m_fResHeightBase * RenderDevice::m_fHeightRatio))
		{
			return;
		}

		AddRenderControlItem(pDrawList, &m_GCBorder, inParentLayer);
	}

	float fWidthRatio = RenderDevice::m_fWidthRatio;
	float fHeightRatio = RenderDevice::m_fHeightRatio;

	if (m_dwAlignType == 0)
	{
		m_GCText.nPosX = (float)(ivParentPos.x + m_nPosX) + (8.0f * fWidthRatio);
		if (m_cComma == 2)
		{
			m_GCText2.nPosX = (float)(ivParentPos.x + m_nPosX) + (8.0f * fWidthRatio);
			m_GCText3.nPosX = (float)(ivParentPos.x + m_nPosX) + (8.0f * fWidthRatio);
			m_GCText4.nPosX = (float)(ivParentPos.x + m_nPosX) + (8.0f * fWidthRatio);
		}
	}
	else if (m_dwAlignType == 1)
	{
		// This code is from chinese cliente version.
		//int len = strlen(m_GCText.strString);
		//m_GCText.nPosX = (float)(ivParentPos.x + m_nPosX)
		//	+ (float)((float)(m_nWidth - (float)((float)(6 * len) * fHeightRatio)) / 2.0f);

		// This code is from WYD BR version.
		SIZE size;
		GetTextExtentPoint32(g_pDevice->m_hDC, m_GCText.strString, strlen(m_GCText.strString), &size);
		m_GCText.nPosX = (float)(ivParentPos.x + m_nPosX) + ((m_nWidth - (float)size.cx) / 2.0f);

		if (m_cComma == 2)
		{
			SIZE size2;
			GetTextExtentPoint32(g_pDevice->m_hDC, m_GCText2.strString, strlen(m_GCText2.strString), &size2);
			SIZE size3;
			GetTextExtentPoint32(g_pDevice->m_hDC, m_GCText3.strString, strlen(m_GCText3.strString), &size3);
			SIZE size4;
			GetTextExtentPoint32(g_pDevice->m_hDC, m_GCText4.strString, strlen(m_GCText4.strString), &size4);

			auto fullSize = size2.cx + size3.cx + size4.cx;
			m_GCText.nPosX = (float)(ivParentPos.x + m_nPosX) + ((m_nWidth - (float)fullSize) / 2.0f);
			m_GCText2.nPosX = (float)(size2.cx / 2) + m_GCText.nPosX;
			m_GCText3.nPosX = (float)(size3.cx / 2) + m_GCText2.nPosX;
			m_GCText4.nPosX = (float)(size4.cx / 2) + m_GCText3.nPosX;
		}
	}
	else if (m_dwAlignType == 2)
	{
		int len = strlen(m_GCText.strString);
		m_GCText.nPosX = (float)((float)(ivParentPos.x + m_nPosX) + m_nWidth)
			- (float)((float)(8 * len + 8) * fWidthRatio);

		if (m_cComma == 2)
		{
			m_GCText2.nPosX = (float)((float)(ivParentPos.x + m_nPosX) + m_nWidth)
				- (float)((float)(8 * len + 8) * fWidthRatio);
			m_GCText3.nPosX = (float)((float)(ivParentPos.x + m_nPosX) + m_nWidth)
				- (float)((float)(8 * len + 8) * fWidthRatio);
			m_GCText4.nPosX = (float)((float)(ivParentPos.x + m_nPosX) + m_nWidth)
				- (float)((float)(8 * len + 8) * fWidthRatio);
		}
	}
	else if (m_dwAlignType == 3)
	{
		m_GCText.nPosX = (float)(ivParentPos.x + m_nPosX)
			+ (float)(2.0f * RenderDevice::m_fWidthRatio);

		if (m_cComma == 2)
		{
			m_GCText2.nPosX = (float)(ivParentPos.x + m_nPosX) + (2.0f * fWidthRatio);
			m_GCText3.nPosX = (float)(ivParentPos.x + m_nPosX) + (2.0f * fWidthRatio);
			m_GCText4.nPosX = (float)(ivParentPos.x + m_nPosX) + (2.0f * fWidthRatio);
		}
	}
	else if (m_dwAlignType == 4)
	{
		m_GCText.nPosX = (float)((float)(ivParentPos.x + m_nPosX)
			+ (float)(8.0f * RenderDevice::m_fWidthRatio))
			+ (float)104;
		if (m_cComma == 2)
		{
			m_GCText2.nPosX = (float)(ivParentPos.x + m_nPosX + (8.0f * fWidthRatio))
				+ (float)104;
			m_GCText3.nPosX = (float)(ivParentPos.x + m_nPosX + (8.0f * fWidthRatio))
				+ (float)104;
			m_GCText4.nPosX = (float)(ivParentPos.x + m_nPosX + (8.0f * fWidthRatio))
				+ (float)104;
		}
	}

	m_GCText.nPosY = (float)(ivParentPos.y + m_nPosY)
		+ (float)((float)((float)(m_nHeight - (float)(16.0f * fHeightRatio)) / 2.0f) + 2.0f);
	m_GCText.nWidth = m_nWidth;
	m_GCText.nHeight = m_nHeight;
	m_GCText.nLayer = inParentLayer;


	if ((float)(m_GCText.nPosX + m_GCText.nWidth) >= 0.0f &&
		(float)(m_GCText.nPosY + m_GCText.nHeight) >= 0.0f &&
		m_GCText.nPosX <= (float)(RenderDevice::m_fResWidthBase * RenderDevice::m_fWidthRatio) &&
		m_GCText.nPosY <= (float)(RenderDevice::m_fResHeightBase * RenderDevice::m_fHeightRatio))
	{
		AddRenderControlItem(pDrawList, &m_GCText, inParentLayer);

		if (m_cComma == 2)
		{
			m_GCText2.nPosY = ((ivParentPos.y + m_nPosY) +
				(((m_nHeight - (16.0f * fHeightRatio)) / 2.0f) + 2.0f));
			m_GCText2.nWidth = m_nWidth;
			m_GCText2.nHeight = m_nHeight;
			m_GCText2.nLayer = inParentLayer;
			AddRenderControlItem(pDrawList, &m_GCText2, inParentLayer);

			m_GCText3.nPosY = ((ivParentPos.y + m_nPosY)
				+ (((m_nHeight - (16.0f * fHeightRatio)) / 2.0f)
					+ 2.0f));
			m_GCText3.nWidth = m_nWidth;
			m_GCText3.nHeight = m_nHeight;
			m_GCText3.nLayer = inParentLayer;
			AddRenderControlItem(pDrawList, &m_GCText3, inParentLayer);

			m_GCText4.nPosY = ((ivParentPos.y + m_nPosY)
				+ (((m_nHeight - (16.0f * fHeightRatio)) / 2.0f)
					+ 2.0f));
			m_GCText4.nWidth = m_nWidth;
			m_GCText4.nHeight = m_nHeight;
			m_GCText4.nLayer = inParentLayer;
			AddRenderControlItem(pDrawList, &m_GCText4, inParentLayer);
		}
	}
}

int SText::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{ 
	m_bOver = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);


	if (m_bOver)
	{
		ConsolePainelMacro(2);
	}

	Update();
	return 0;
}

SEditableText::SEditableText(int inTextureSetIndex, const char* istrText, size_t inMaxStringLen, int ibPasswd, unsigned int idwFontColor, float inX, float inY, float inWidth, float inHeight, int ibBorder, unsigned int idwBorderColor, unsigned int dwType, unsigned int dwAlignType)
	: SText(inTextureSetIndex,
		0,
		idwFontColor,
		inX,
		inY,
		inWidth,
		inHeight,
		ibBorder,
		idwBorderColor,
		dwType,
		dwAlignType)
{
	m_cTempChar = 0;
	m_cReserved = 0;
	m_nMaxStringLen = inMaxStringLen;
	m_nCursorVisible = 0;
	m_bPasswd = ibPasswd;
	m_bEncrypt = 0;
	m_bKorean = 0;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_EDITABLETEXT;
	m_nMaxStringLen = m_nMaxStringLen;

	if (istrText != nullptr)
	{
		BASE_UnderBarToSpace(const_cast<char*>(istrText));
		if (!strcmp(istrText, " "))
			istrText = nullptr;
	}

	int nCopyLen = 0;
	if (istrText != nullptr)
	{
		nCopyLen = strlen(istrText) <= inMaxStringLen ? strlen(istrText) : inMaxStringLen;
		strncpy(m_strText, istrText, nCopyLen);
	}
	else
		nCopyLen = 0;

	m_strText[nCopyLen] = 0;
	m_strComposeText[0] = 0;
}

SEditableText::~SEditableText()
{
}

void SEditableText::SetText(char* istrText)
{
	if (istrText[0] == 0)
	{
		memset(m_strText, 0, sizeof(m_strText));
		Update();
		return;
	}

	int nCopyLen = strlen(istrText) <= m_nMaxStringLen ? strlen(istrText) : m_nMaxStringLen;

	strncpy(m_strText, istrText, nCopyLen);
	m_strText[nCopyLen] = 0;
	Update();
}

char* SEditableText::GetText()
{
	return m_strText;
}

int SEditableText::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (m_bSelectEnable == 0)
		return 0;

	int bOver = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);

	if (dwFlags != WM_LBUTTONDOWN)
		return SText::OnMouseEvent(dwFlags, wParam, nX, nY);

	if (bOver != 1)
		return 0;

	if (m_bOver)
	{
		ConsolePainelMacro(3);
	}
	m_bFocused = 1;
	return 1;
}

int SEditableText::OnCharEvent(char iCharCode, int lParam)
{
	if (m_bEnable == 0)
		return 0;

	if (m_bFocused != 1)
		return 0;

	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	 
	auto PanelPerfil = (SPanel*)pFScene->m_pControlContainer->FindControl(148700);
	 

	if (iCharCode == VK_LEFT)
		return 0;

	if (iCharCode == VK_ESCAPE )
	{
		if (g_nKeyType == 0)
		{
			SetText((char*)"");
			m_bFocused = 0;
			g_pCurrentScene->m_pControlContainer->SetFocusedControl(nullptr);
		}
		return 1;
	}
	if (iCharCode == VK_RETURN)
	{
		if (m_pEventListener != nullptr)
			return m_pEventListener->OnControlEvent(m_dwControlID, 0);

		return 0;
	}
	if (iCharCode == VK_TAB)
	{
		if (m_pEventListener != nullptr)
			return m_pEventListener->OnControlEvent(m_dwControlID, 1);

		return 0;
	}

	SButton* Button = nullptr;
	SButton* Button2 = nullptr;

	int nTextLen = strlen(m_strText);
	if (g_pCurrentScene->GetSceneType() == ESCENE_TYPE::ESCENE_FIELD &&
		iCharCode != VK_BACK &&
		nTextLen <= 1 &&
		(iCharCode == '=' || iCharCode == '-' || iCharCode == '@'))
	{
		if (nTextLen == 0)
		{
			switch (iCharCode)
			{
			case '-':
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_GUILD));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "-");
				break;
			case '=':
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_PARTY));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "=");
				break;
			case '@':
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_KINGDOM));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "@");
				break;
			}

			if (Button != nullptr)
			{
				Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
				Button2->SetText(Button->m_GCPanel.strString);
			}
		}
		else if (iCharCode == '-' && m_strText[0] == '-')
		{
			Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_GUILD2));
			//strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "--");
			Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
			Button2->SetText(Button->m_GCPanel.strString);
		}
		else if (iCharCode == '@' && m_strText[0] == '@')
		{
			Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_CITY));
			strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "@@");
			Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
			Button2->SetText(Button->m_GCPanel.strString);
		}
	}
	if (g_pCurrentScene->GetSceneType() == ESCENE_TYPE::ESCENE_FIELD && iCharCode != VK_BACK)
	{
		char temp[64]{};
		sprintf(temp, "/%s", g_pMessageStringTable[389]);
		int length = strlen(temp);

		if (m_strText[0] == '/' && !strncmp(m_strText, temp, length))
		{
			Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_SHOUT));
			strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "");
			Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
			Button2->SetText(Button->m_GCPanel.strString);
		}
		else if (m_strText[0] == '/' && strncmp(m_strText, temp, length))
		{
			Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_WHISPER));
			strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "");
			Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
			Button2->SetText(Button->m_GCPanel.strString);
		}
	}
	if (iCharCode == VK_BACK)
	{
		if (strlen(m_strComposeText))
			return 1;

		/*Busca*/
		if (g_pCurrentScene->GetSceneType() == ESCENE_TYPE::ESCENE_FIELD)
		{
			auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

			if (pFScene == nullptr)
				return 0;


			if (pFScene->m_pLeilaoStore->m_bVisible && m_bFocused && m_dwControlID == 6002213)
			{
				if (pFScene->m_bCurrentSlotLeilao >= 0 && pFScene->m_dwDelayBusca + 1000 <= g_pApp->m_pTimerManager->GetServerTime())
				{
					pFScene->SendSearchLeilaoRequest(pFScene->m_stAutoTrade.List[pFScene->m_bCurrentSlotLeilao].Categoria, (char*)"");
					pFScene->m_dwDelayBusca = g_pApp->m_pTimerManager->GetServerTime();
				}
			} 
			if (m_dwControlID == 19932)
			{
				// Verifica se a string tem um caractere ou menos e se já passou 1 segundo
				if (strlen(m_GCText.strString) <= 1 && pFScene->m_dwDelayBusca + 1000 <= g_pApp->m_pTimerManager->GetServerTime())
				{
					// Atualiza o tempo do último envio
					pFScene->m_dwDelayBusca = g_pApp->m_pTimerManager->GetServerTime();

					MSG_ReqListaGrupo stParm{};
					stParm.Header.ID = pFScene->m_pMyHuman->m_dwID;
					stParm.Header.Type = 0xCDB1;
					stParm.CurrentPage = 1;

					// Envia a mensagem
					g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));
				}
			}


			if (m_bFocused && m_dwControlID == 478491)
			{
				if (strlen(m_GCText.strString) <= 1 && pFScene->m_dwDelayBusca + 1000 <= g_pApp->m_pTimerManager->GetServerTime())
				{
					if (pFScene->pDropMode == 1)
					{

						MSG_STANDARDPARM2 stParm{};
						stParm.Header.ID = pFScene->m_pMyHuman->m_dwID;
						stParm.Header.Type = 0xA08;
						stParm.Parm1 = 1;
						g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));
						pFScene->m_dwDelayBusca = g_pApp->m_pTimerManager->GetServerTime();
					}
					if (pFScene->pDropMode == 2)
					{

						MSG_STANDARDPARM2 stParm{};
						stParm.Header.ID = pFScene->m_pMyHuman->m_dwID;
						stParm.Header.Type = 0xA08;
						stParm.Parm1 = 1;
						g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));
						pFScene->m_dwDelayBusca = g_pApp->m_pTimerManager->GetServerTime();
					}
				}
			}
		}
		/*Busca do leilao*/

		int nTextLen = strlen(m_strText);
		if (nTextLen == 0)
		{
			if (g_pCurrentScene->GetSceneType() == ESCENE_TYPE::ESCENE_FIELD)
			{
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_NOMAL));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "");
				Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
				Button2->SetText(Button->m_GCPanel.strString);
			}

			return m_pEventListener->OnControlEvent(m_dwControlID, 7);
		}

		LPSTR szPrevText = CharPrev(m_strText, &m_strText[nTextLen]);

		int nLen = szPrevText - m_strText;
		m_strText[nLen] = 0;
		Update();

		if (g_pCurrentScene->GetSceneType() == ESCENE_TYPE::ESCENE_FIELD)
		{
			if (!strcmp(m_strText, "="))
			{
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_PARTY));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "=");
				Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
				Button2->SetText(Button->m_GCPanel.strString);
			}
			else if (!strcmp(m_strText, "-"))
			{
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_GUILD));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "-");
				Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
				Button2->SetText(Button->m_GCPanel.strString);
			}
			else if (!strcmp(m_strText, "--"))
			{
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_GUILD2));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "--");
				Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
				Button2->SetText(Button->m_GCPanel.strString);
			}
			else if (!strcmp(m_strText, "@"))
			{
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_KINGDOM));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "@");
				Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
				Button2->SetText(Button->m_GCPanel.strString);
			}
			else if (!strcmp(m_strText, "@@"))
			{
				Button = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT_CITY));
				strcpy(static_cast<TMFieldScene*>(g_pCurrentScene)->m_cChatType, "@@");
				Button2 = static_cast<SButton*>(g_pCurrentScene->m_pControlContainer->FindControl(B_CHAT_SELECT));
				Button2->SetText(Button->m_GCPanel.strString);
			}
		}
		return 1;
	}
	else if (m_nMaxStringLen > strlen(m_strText))
	{
		if (m_cTempChar)
			m_cTempChar = 0;

		LPSTR lpCurrent = &m_strText[strlen(m_strText)];
		lpCurrent[0] = iCharCode;
		lpCurrent++;
		lpCurrent[0] = 0;
		lpCurrent++;
		Update();

		return 1;
	}
	else
	{
		int nTextLen = strlen(m_strText);
		if (nTextLen == 0)
			return 1;

		if (!IsClearString(m_strText, nTextLen - 1))
		{
			m_strText[nTextLen - 1] = 0;
			m_strComposeText[0] = 0;
		}

		Update();
		m_cTempChar = iCharCode;
		return 1;
	}

	return 0;
}

int SEditableText::OnChangeIME()
{
	// No need for now
	return 0;
}

int SEditableText::OnIMEEvent(char* ipComposeString)
{
	// No need for now
	return 0;
}

int SEditableText::OnKeyDownEvent(unsigned int iKeyCode)
{
	if (m_bEnable == 0)
		return 0;
	if (m_bFocused != 1)
		return 0;

	if (iKeyCode == VK_PRIOR)
		return m_pEventListener->OnControlEvent(m_dwControlID, 2);
	if (iKeyCode == VK_UP)
		return m_pEventListener->OnControlEvent(m_dwControlID, 3);
	if (iKeyCode == VK_NEXT)
		return m_pEventListener->OnControlEvent(m_dwControlID, 4);
	if (iKeyCode == VK_DOWN)
		return m_pEventListener->OnControlEvent(m_dwControlID, 5);
	if (iKeyCode == VK_DELETE)
		return m_pEventListener->OnControlEvent(m_dwControlID, 6);

	return 0;
}

int SEditableText::OnKeyUpEvent(unsigned int iKeyCode)
{
	return m_bFocused == 1;
}

void SEditableText::Update()
{
	int nStringLen = strlen(m_strText);
	int nComposeStringLen = strlen(m_strComposeText);

	if (m_bPasswd == 0)
	{
		strcpy(m_GCText.strString, m_strText);
	}
	else
	{
		for (int nIndex = 0; nIndex < nComposeStringLen + nStringLen; ++nIndex)
			m_GCText.strString[nIndex] = '*';

		m_GCText.strString[nStringLen] = 0;
	}

	m_GCText.pFont->SetText(m_GCText.strString, 0xFFFFFFFF, 0);
}

void SEditableText::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	int nStringLen = strlen(m_strText);
	if (m_bFocused == 1)
	{
		++m_nCursorVisible;
		m_nCursorVisible %= 20;

		if (m_nCursorVisible == 10)
		{
			m_GCText.strString[nStringLen] = '|';
			m_GCText.strString[nStringLen + 1] = 0;
			m_GCText.pFont->SetText(m_GCText.strString, m_GCText.dwColor, 0);
		}
		else if (m_nCursorVisible == 0)
		{
			m_GCText.strString[nStringLen] = 0;
			m_GCText.pFont->SetText(m_GCText.strString, m_GCText.dwColor, 0);
		}
	}
	else if (m_GCText.strString[nStringLen] == '|')
	{
		m_GCText.strString[nStringLen] = 0;
		m_GCText.pFont->SetText(m_GCText.strString, m_GCText.dwColor, 0);
	}
	else
		m_GCText.strString[nStringLen] = 0;

	/*Busca do leilao*/
	if (g_pCurrentScene->GetSceneType() == ESCENE_TYPE::ESCENE_FIELD)
	{
		auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

		auto BuscaleilaoStore = static_cast<SEditableText*>(pFScene->m_pControlContainer->FindControl(6002213));

		auto pGridLeilaoStore = (SGridControl*)pFScene->m_pControlContainer->FindControl(6200011);

		bool Go = (pGridLeilaoStore != nullptr) && (BuscaleilaoStore != nullptr) && (pFScene != nullptr);

		if (Go)
		{ 
			if(pFScene->m_pLeilaoStore->m_bVisible && m_bFocused && m_dwControlID == 6002213)
			{ 
				m_GCText.strString[nStringLen] = 0;
				// Verifica se o texto mudou
				if (strcmp(m_GCText.strString, m_lastSearchText) != 0)
				{
					// Atualiza o texto da última busca
					strcpy(m_lastSearchText, m_GCText.strString);

					// Verifica se a string de pesquisa está vazia
					if (strlen(m_GCText.strString) > 0)
					{
						pFScene->SendSearchLeilaoRequest(pFScene->m_bCurrentTipeLeilao, m_GCText.strString);
					}
					else
					{
						pFScene->SendLeilaoRequest(pFScene->m_bCurrentTipeLeilao, 0);
					}
				}
			}
			else if (m_bFocused && m_dwControlID == 478491)
			{
				m_GCText.strString[nStringLen] = 0;
				 
				if(pFScene->pDropMode == 1)
				{
					// Verifica se o texto mudou
					if (strcmp(m_GCText.strString, m_lastSearchText) != 0)
					{
						// Atualiza o texto da última busca
						strcpy(m_lastSearchText, m_GCText.strString);

						// Verifica se a string de pesquisa está vazia
						if (strlen(m_GCText.strString) > 0)
						{
							pFScene->SendSearchDropList(0, m_GCText.strString);
						}
					}
				}
				else if (pFScene->pDropMode == 2)
				{
					m_GCText.strString[nStringLen] = 0;
					// Verifica se o texto mudou
					if (strcmp(m_GCText.strString, m_lastSearchText) != 0)
					{ 
						// Atualiza o texto da última busca
						strcpy(m_lastSearchText, m_GCText.strString);

						// Verifica se a string de pesquisa está vazia
						if (strlen(m_GCText.strString) > 0)
						{
							pFScene->SendSearchDropList(0, m_GCText.strString);
						}
					}
				}
			}
			else if (m_bFocused && m_dwControlID == 19932)
			{
				m_GCText.strString[nStringLen] = 0;
				 
				// Verifica se o texto mudou
				if (strcmp(m_GCText.strString, m_lastSearchText) != 0)
				{
					// Atualiza o texto da última busca
					strcpy(m_lastSearchText, m_GCText.strString);

					// Verifica se a string de pesquisa está vazia
					if (strlen(m_GCText.strString) > 0)
					{
						pFScene->SendBuscarListaGrupo(0, m_GCText.strString);
					}
				}
			}
		} 
	}



	SText::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);
}

void SEditableText::SetFocused(int bFocused)
{
	m_bFocused = bFocused;
}

int SEditableText::IsIMENative()
{
	return 0;
}

SButton::SButton(int inTextureSetIndex, float inX, float inY, float inWidth, float inHeight, unsigned int idwColor, int bSound, char* istrText)
	: SPanel(inTextureSetIndex, inX, inY, inWidth, inHeight, idwColor, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH)
{
	m_bMouseOver = 0;
	m_bSelected = 0;
	m_cAlwaysAlt = 0;
	m_cBlink = 0;
	m_pAltText = 0;
	m_dwColor = idwColor;
	m_bSound = bSound;
	m_dwOldTime = 0;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_BUTTON;

	if (istrText != nullptr)
	{
		BASE_UnderBarToSpace(istrText);
		if (!strcmp(istrText, " "))
			istrText = nullptr;
	}

	if (istrText != nullptr)
	{
		if (inTextureSetIndex >= -2 && inTextureSetIndex < 0)
		{
			m_GCPanel.pFont = nullptr;
			m_GCPanel.pFont = new TMFont2();

			sprintf(m_GCPanel.strString, "%s", istrText);

			if (m_GCPanel.pFont != nullptr)
				m_GCPanel.pFont->SetText(m_GCPanel.strString, 0xFFFFFFFF, 0);
		}

		m_pAltText = new SText(-1, istrText, 0xFFFFFFFF, inWidth - 10.0f, 0.0f, (float)strlen(istrText) * 8.0f, (float)RenderDevice::m_nFontSize,
			0, 0xFF333333, 1u, 0);
		m_pAltText->SetVisible(0);
		m_pAltText->SetAlwaysOnTop(1);

		if (inTextureSetIndex == -1 || inTextureSetIndex == -2)
		{
			m_nPosX = m_nPosX
				- (float)((float)(RenderDevice::m_nFontSize - 12) * (float)((float)strlen(istrText) * 0.3f));
			m_nPosY = m_nPosY - (float)((float)(RenderDevice::m_nFontSize - 12) * 0.6f);
		}
	}

	m_GrayType = 0;
	Update();
}

SButton::~SButton()
{
	SAFE_DELETE(m_GCPanel.pFont);
	SAFE_DELETE(m_pAltText);
}

void SButton::SetText(char* istrText)
{
	if (m_pAltText != nullptr)
		m_pAltText->SetText(istrText, 0);

	sprintf(m_GCPanel.strString, "%s", istrText);
	m_GCPanel.pFont->SetText(m_GCPanel.strString, 0xFFFFFFFF, 0);
}

int SButton::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (m_bSelectEnable == 0)
		return 0;

	m_bOver = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);

	if (m_bOver)
	{
		ConsolePainelMacro(4);
	}
	if (m_pAltText != nullptr)
	{
		if (m_bOver == 1)
		{
			if (!m_pAltText->IsVisible())
			{
				m_pAltText->SetVisible(1);
				m_pAltText->m_Font.m_dwBgStatus = 1;
			}
			if (m_GCPanel.nTextureSetIndex > -3 && m_GCPanel.nTextureSetIndex < 3)
			{
				m_GCPanel.eRenderType = RENDERCTRLTYPE::RENDER_TEXT_FOCUS;
				if (m_GCPanel.pFont != nullptr)
					m_GCPanel.pFont->m_dwShadeColor = 0x66FFFFFF;
			}
		}
		else
		{
			m_pAltText->m_Font.m_dwBgStatus = 0;
			m_pAltText->SetVisible(0);
			m_GCPanel.eRenderType = RENDERCTRLTYPE::RENDER_IMAGE_STRETCH;
			if (m_GCPanel.pFont != nullptr)
				m_GCPanel.pFont->m_dwShadeColor = 0xFF000000;
		}
	}

	switch (dwFlags)
	{
	case WM_MOUSEMOVE:
		if (m_bMouseOver == 1 && m_pEventListener != nullptr)
			m_pEventListener->OnControlEvent(m_dwControlID, 2);
		break;
	case WM_LBUTTONDOWN:
		if (m_bVisible == 0)
			return 1;

		if (m_bOver == 1)
		{
			m_bPressed = 1;
			m_bFocused = 1;
			Update();
			return 1;
		}
		break;
	case WM_LBUTTONUP:
		if (m_bVisible == 0)
			return 1;

		if (m_bEnable == 1 && m_bOver == 1 && m_bPressed == 1 && m_pEventListener != nullptr)
		{
			unsigned int dwServerTime = g_pTimerManager->GetServerTime();
			if (dwServerTime > m_dwOldTime + 500)
			{
				if (m_bSound == 1 && g_pSoundManager != nullptr)
				{
					if (g_pSoundManager->GetSoundData(53))
						g_pSoundManager->GetSoundData(53)->Play(0, 0);
				}

				Update();
				m_pEventListener->OnControlEvent(m_dwControlID, 0);
				m_dwOldTime = g_pTimerManager->GetServerTime();
				m_bPressed = 0;
			}
			return 1;
		}
		m_bPressed = 0;
		break;
	default:
		return 0;
	}

	if (m_cAlwaysAlt != 0 && m_pAltText->IsVisible())
		m_pAltText->SetVisible(1);

	Update();
	return 0;
}

void SButton::SetSelected(int bSelected)
{
	m_bSelected = bSelected;
}

void SButton::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	if (m_dwControlID == 4628)
		m_GCPanel.dwColor = 0xAA000077;

	/*if (m_GCPanel.nTextureSetIndex == -481)
	{
		m_GCPanel.bVisible = 0;
		std::cout << "\n m_GCPanel.nTextureSetIndex " << m_GCPanel.nTextureSetIndex;
	 }*/

	if (m_GCPanel.nTextureSetIndex == 428)
	{
		auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
		if (pFScene->m_pLeilaoStore->m_bVisible)
		{
			m_GCPanel.bVisible = 0;
		}
		else
		{
			m_GCPanel.bVisible = 1;

		}
		//m_GCPanel.nTextureIndex = 2;
	}
	if (m_cBlink != 0)
	{
		unsigned int dwServerTime = g_pTimerManager->GetServerTime();
		if ((dwServerTime / 750) % 2)
			m_GCPanel.nTextureIndex = 1;
		else
			m_GCPanel.nTextureIndex = 0;
	}

	SPanel::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);
	if (m_pAltText != nullptr &&
		m_pAltText->IsVisible() &&
		(m_GCPanel.nTextureSetIndex > 0 || m_GCPanel.nTextureSetIndex < -2))
	{
		ivParentPos = ivParentPos + TMVector2(m_nPosX, m_nPosY);

		if (((ivParentPos.x + m_pAltText->m_nPosX) + m_pAltText->m_nWidth) > (float)g_pDevice->m_dwScreenWidth)
			ivParentPos.x = ((float)g_pDevice->m_dwScreenWidth - (m_pAltText->m_nPosX * 2.0f)) - m_pAltText->m_nWidth;

		m_pAltText->FrameMove2(pDrawList, ivParentPos, 28, nFlag);
	}
}

void SButton::Update()
{
	if (m_bEnable == 0)
	{
		m_GCPanel.nTextureIndex = 4;
		if (m_GrayType != 0)
			m_GCPanel.dwColor = 0xFF7F7F7F;
	}
	else if (m_bSelected == 1)
	{
		m_GCPanel.nTextureIndex = 3;
	}
	else if (m_bOver == 1)
	{
		if (m_bPressed == 1)
			m_GCPanel.nTextureIndex = 2;
		else
			m_GCPanel.nTextureIndex = 1;
	}
	else
	{
		m_GCPanel.nTextureIndex = 0;
		if (m_GCPanel.pFont != nullptr)
			m_GCPanel.dwColor = 0x1010101;
		else
			m_GCPanel.dwColor = m_dwColor;
	}
}

SButtonBox::SButtonBox(float inPosX, float inPosY, float inWidth, float inHeight, int nStartCount, int nEndCount, int nCurrnetPage, int nPrevPage, int nNextPage, int nStartPage, int nEndPage)
	: SControl(inPosX, inPosY, inWidth, inHeight)
{

}

SButtonBox::~SButtonBox()
{
}

void SButtonBox::SetEventListener(IEventListener* ipEventListener)
{
}

void SButtonBox::SetButtonBox(int nStartCount, int nEndCount, int nCurrnetPage, int nPrevPage, int nNextPage, int nStartPage, int nEndPage)
{
}

SCheckBox::SCheckBox(unsigned int inTextureSetIndex, float inX, float inY, float inWidth, float inHeight, unsigned int dwColor)
	: SPanel(inTextureSetIndex, inX, inY, inWidth, inHeight, dwColor, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH)
{
	m_bValue = 0;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_CHECKBOX;
	m_dwSelectedColor = -16777216;
	m_dwUnSelectedColor = dwColor;
}

SCheckBox::~SCheckBox()
{
}

void SCheckBox::SetValue(int ibValue)
{
	m_bValue = ibValue;
	Update();
}

int SCheckBox::GetValue()
{
	return m_bValue;
}

int SCheckBox::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (!m_bSelectEnable)
		return 0;

	m_bOver = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);

	if (dwFlags == 512)
		return 0;

	if (dwFlags != 513)
		return SPanel::OnMouseEvent(dwFlags, wParam, nX, nY);

	m_bValue = m_bValue == 0;
	m_bFocused = 1;
	Update();

	if (m_pEventListener)
		m_pEventListener->OnControlEvent(m_dwControlID, 0);

	return 1;
}

void SCheckBox::Update()
{
	if (m_bValue == 1)
	{
		m_GCPanel.nTextureIndex = 1;
		m_GCPanel.dwColor = m_dwSelectedColor;
	}
	else
	{
		m_GCPanel.nTextureIndex = 0;
		m_GCPanel.dwColor = m_dwUnSelectedColor;
	}
}

SProgressBar::SProgressBar(int inTextureSetIndex, int inCurrent, int inMax, float inX, float inY, float inWidth, float inHeight, unsigned int idwProgressColor, unsigned int idwColor, unsigned int dwStyle)
	: SPanel(inTextureSetIndex, inX, inY, inWidth, inHeight, idwColor, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH)
{
	m_nCurrent = inCurrent;
	m_nMax = inMax;
	m_GCProgress = GeomControl(RENDERCTRLTYPE::RENDER_IMAGE_STRETCH, inTextureSetIndex, inX, inY, 0.0f, inHeight, 0, idwProgressColor);
	m_GCPanel.eRenderType = RENDERCTRLTYPE::RENDER_IMAGE_STRETCH;
	m_GCPanel.nTextureIndex = 1;
	m_dwStyle = dwStyle;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_PROGRESSBAR;

	if (RenderDevice::m_nBright > 58)
	{
		int nR = WYDCOLOR_RED(idwProgressColor) - (RenderDevice::m_nBright - 40);
		int nG = WYDCOLOR_GREEN(idwProgressColor) - (RenderDevice::m_nBright - 40);
		int nB = WYDCOLOR_BLUE(idwProgressColor) - (RenderDevice::m_nBright - 40);

		if (nR < 0)
			nR = 0;
		if (nG < 0)
			nG = 0;
		if (nB < 0)
			nB = 0;

		m_GCProgress.dwColor = nB | (nG << 8) | idwProgressColor & 0xFF000000 | (nR << 16);
	}

	int nTextureSetIndex = m_GCProgress.nTextureSetIndex;
	if (nTextureSetIndex < -2)
		nTextureSetIndex = -m_GCProgress.nTextureSetIndex;

	auto pUISet = g_pTextureManager->GetUITextureSet(nTextureSetIndex);
	if (pUISet != nullptr)
	{
		m_InitHeight = (float)pUISet->pTextureCoord[m_GCProgress.nTextureIndex].nHeight;
		m_InitStartY = (float)pUISet->pTextureCoord[m_GCProgress.nTextureIndex].nStartY;
		m_InitWidth = (float)pUISet->pTextureCoord[m_GCProgress.nTextureIndex].nWidth;
		m_InitStartX = (float)pUISet->pTextureCoord[m_GCProgress.nTextureIndex].nStartX;
	}

	Update();
}

SProgressBar::~SProgressBar()
{
	ResetBar();

	SControlContainer* pControlContainer = g_pCurrentScene->m_pControlContainer;

	if (pControlContainer != nullptr && m_GCProgress.nLayer >= 0)
		RemoveRenderControlItem(pControlContainer->m_pDrawControl, &m_GCProgress, m_GCProgress.nLayer);
}

void SProgressBar::SetCurrentProgress(int inCurrent)
{
	if (m_nCurrent != inCurrent)
	{
		m_nCurrent = inCurrent;
		if (inCurrent < 0)
			m_nCurrent = 0;

		Update();
	}
}

void SProgressBar::SetMaxProgress(int inMax)
{
	if (m_nMax != inMax)
	{
		m_nMax = inMax;
		Update();
	}
}

int SProgressBar::GetCurrentProgress()
{
	return m_nCurrent;
}

int SProgressBar::GetMaxProgress()
{
	return m_nMax;
}

void SProgressBar::Update()
{
	if (m_nCurrent > m_nMax)
		m_nCurrent = m_nMax;
	if (m_nMax <= 0)
		m_nMax = 1;

	if (m_dwStyle == 1)
		m_nProgressWidth = (float)((float)m_nCurrent * m_nWidth) / (float)m_nMax;
	else if (m_dwStyle == 2)
		m_nProgressWidth = (float)((float)m_nCurrent * m_nWidth) / (float)m_nMax;
	else
		m_nProgressHeight = (float)((float)m_nCurrent * m_nHeight) / (float)m_nMax;
}

void SProgressBar::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	float fWidthRatio = (float)g_pDevice->m_dwScreenWidth / RenderDevice::m_fResWidthBase;
	float fHeightRatio = (float)g_pDevice->m_dwScreenHeight / RenderDevice::m_fResHeightBase;

	SPanel::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);

	if (m_dwStyle == 1)
	{
		m_GCProgress.nPosX = (float)(ivParentPos.x + m_nPosX) + 2.0f;
		m_GCProgress.nPosY = (float)(ivParentPos.y + m_nPosY) + 2.0f;
		m_GCProgress.nWidth = m_nProgressWidth;
		m_GCProgress.nHeight = m_nHeight - 4.0f;
		m_GCProgress.nLayer = inParentLayer;
	}
	else if (m_dwStyle == 2)
	{
		int nTextureSetIndex = m_GCProgress.nTextureSetIndex;
		if (nTextureSetIndex < -2)
			nTextureSetIndex = -m_GCProgress.nTextureSetIndex;

		auto pUISet = g_pTextureManager->GetUITextureSet(nTextureSetIndex);
		if (pUISet != nullptr)
			pUISet->pTextureCoord[m_GCProgress.nTextureIndex].nWidth = (int)(m_nProgressWidth / fWidthRatio);

		m_GCProgress.nPosX = (float)(ivParentPos.x + m_nPosX) + 2.0f;
		m_GCProgress.nPosY = (float)(ivParentPos.y + m_nPosY) + 2.0f;
		m_GCProgress.nWidth = m_nProgressWidth;
		m_GCProgress.nHeight = m_nHeight - 4.0f;
		m_GCProgress.nLayer = inParentLayer;
	}
	else
	{
		int nIndex = m_GCProgress.nTextureSetIndex;
		if (nIndex < -2)
			nIndex = -m_GCProgress.nTextureSetIndex;

		auto pUISet = g_pTextureManager->GetUITextureSet(nIndex);
		if (pUISet != nullptr)
		{
			pUISet->pTextureCoord[m_GCProgress.nTextureIndex].nHeight = (int)(m_nProgressHeight / fHeightRatio);
			pUISet->pTextureCoord[m_GCProgress.nTextureIndex].nStartY = (int)(((m_nHeight - m_nProgressHeight) / fHeightRatio) + m_InitStartY);
		}

		m_GCProgress.nPosX = ivParentPos.x + m_nPosX;
		m_GCProgress.nPosY = ((ivParentPos.y + m_nPosY) + m_nHeight) - m_nProgressHeight;
		m_GCProgress.nWidth = m_nWidth;
		m_GCProgress.nHeight = m_nProgressHeight;
		m_GCProgress.nLayer = inParentLayer;
	}

	AddRenderControlItem(pDrawList, &m_GCProgress, inParentLayer);
}

void SProgressBar::ResetBar()
{
	int nTextureSetIndex = 0;
	if (m_GCProgress.nTextureSetIndex < -2)
		nTextureSetIndex = -m_GCProgress.nTextureSetIndex;
}

SScrollBar::SScrollBar(int inCurrent, int inMax, float inX, float inY, float inWidth, float inHeight, unsigned int dwStyle, unsigned int idwBarColor, unsigned int idwColor, int bChat)
	: SControl(inX, inY, inWidth, inHeight)
{
	m_dwStyle = dwStyle;
	m_nCurrent = inCurrent;
	m_nMax = inMax;
	m_nBtnSize = 17.0f;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_SCROLLBAR;

	if (m_dwStyle == 0)
	{
		m_nBarSize = inWidth;
		float nStartXPos = 0;

		if (bChat)
			nStartXPos = -5;

		m_pUpPanel = new SPanel(423, (float)nStartXPos, 0.0f, 10.0f, m_nBtnSize, 0xAAAA00FF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_pDownPanel = new SPanel(424, (float)nStartXPos, ((m_nHeight - m_nBtnSize) + 7.0f) / 1.0f, 10.0f, m_nBtnSize, 0xAAAA00FF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_nScrollLength = (int)(float)(m_nHeight - (m_nBtnSize * 2.0));
		m_pBar = new SPanel(425, (float)nStartXPos + 1.0f, 0.0f, 10.0f - 2.0f, 17.0f, 0xFFFFFFFF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_pBackground1 = new SPanel(426, (float)nStartXPos, 0.0f, 10.0f, inHeight, 0xFFFFFFFF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_pBackground1->GetGeomControl()->eRenderType = RENDERCTRLTYPE::RENDER_IMAGE_STRETCH;
	}
	else
	{
		m_pUpPanel = new SPanel(-2, 0.0, 0.0, m_nBtnSize, m_nHeight, 0x44444444, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_pDownPanel = new SPanel(-2, m_nWidth - m_nBtnSize, 0.0f, m_nBtnSize, m_nHeight, 0x44444444, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_nScrollLength = (int)(m_nWidth - (m_nBtnSize * 2.0f));
		m_pBar = new SPanel(-2, 0.0f, 0.0f, m_nBarSize, m_nBarSize, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_pBackground1 = new SPanel(-2, 0.0f, 0.0f, m_nWidth, inHeight, 0x77777777, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

		if (m_pBackground1 != nullptr)
			m_pBackground1->GetGeomControl()->nTextureIndex = 0;
	}

	if (m_pUpPanel != nullptr)
	{
		m_pUpPanel->SetControlID(0);
		AddChild(m_pUpPanel);
	}
	if (m_pDownPanel != nullptr)
	{
		m_pDownPanel->SetControlID(1);
		AddChild(m_pDownPanel);
	}
	if (m_pBar != nullptr)
	{
		m_pBar->SetControlID(2);
		AddChild(m_pBar);
	}
	if (m_pBackground1 != nullptr)
	{
		m_pBackground1->SetControlID(3);
		AddChild(m_pBackground1);
	}

	Update();
}

SScrollBar::~SScrollBar()
{
}

void SScrollBar::SetCurrentPos(int inCurrent)
{
	m_nCurrent = inCurrent;
	Update();
}

int SScrollBar::GetCurrentPos()
{
	return m_nCurrent;
}

void SScrollBar::SetMaxValue(int inMax)
{
	m_nMax = inMax;
	Update();
}

int SScrollBar::GetMaxValue()
{
	return m_nMax;
}

void SScrollBar::SetSize(float nWidth, float nHeight)
{
	SControl::SetSize(nWidth, nHeight);
	if (m_dwStyle == 0)
	{
		m_pDownPanel->SetPos(m_pDownPanel->m_nPosX, (float)(m_nHeight - m_nBtnSize) / 1.0f);
		m_nScrollLength = (int)(nHeight - (m_nBtnSize * 2.0f));
		Update();
	}
}

void SScrollBar::Up()
{
	if (m_nCurrent > 5)
	{
		m_nCurrent -= 5;
	}
	else if (m_nCurrent > 0)
	{
		m_nCurrent = 0;
	}

	Update();
}

void SScrollBar::Down()
{
	if (m_nCurrent < m_nMax - 5)
	{
		m_nCurrent += 5;
	}
	else if (m_nCurrent < m_nMax)
	{
		m_nCurrent = m_nMax;
	}

	Update();
}

void SScrollBar::Update()
{
	if (m_nCurrent < 0)
		m_nCurrent = 0;
	if (m_nCurrent > m_nMax)
		m_nCurrent = m_nMax;

	if (m_nMax > 0)
	{
		m_nScrollPos = (int)((((float)m_nScrollLength - m_nBarSize) * (float)m_nCurrent) / (float)m_nMax);
		if (m_dwStyle == 0)
			m_pBar->SetPos(m_pBar->m_nPosX, (float)m_nScrollPos + m_nBtnSize);
		else
			m_pBar->SetPos((float)m_nScrollPos + m_nBtnSize, 0.0f);

		if (m_pEventListener != nullptr)
			m_pEventListener->OnControlEvent(m_dwControlID, m_nCurrent);
	}
}

int SScrollBar::OnControlEvent(DWORD idwControlID, DWORD idwEvent)
{
	return 1;
}

void SScrollBar::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	;
}

int SScrollBar::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (m_bSelectEnable == 0)
		return 0;

	int bInUp = m_pUpPanel->PtInControl((int)((float)nX - m_nPosX), (int)((float)nY - m_nPosY));
	int bInDown = m_pDownPanel->PtInControl((int)((float)nX - m_nPosX), (int)((float)nY - m_nPosY));
	int bInBar = PointInRect(nX, nY, (float)(m_nPosX + m_nWidth) - 15.0f, m_nPosY + m_nBtnSize, 15.0f, m_nHeight - (float)(2.0f * m_nBtnSize));

	if (bInUp && dwFlags == WM_LBUTTONUP)
		Up();
	if (bInDown && dwFlags == WM_LBUTTONUP)
		Down();

	if (m_bOver)
	{
		ConsolePainelMacro(5);
	}
	if (bInBar && dwFlags == WM_MOUSEMOVE && wParam & 1)
	{
		if (m_dwStyle != 0)
			m_nScrollPos = static_cast<int>((float)nX - (float)(m_nPosX + m_nBtnSize));
		else
			m_nScrollPos = static_cast<int>((float)nY - (float)(m_nPosY + m_nBtnSize));

		m_nCurrent = (int)((float)(m_nScrollPos * m_nMax) / (float)((float)m_nScrollLength - m_nBarSize));
		if (m_nCurrent > 0 && m_nCurrent < m_nMax)
			Update();
	}

	return 0;
}

void SScrollBar::upbarSetPos(float x, float y)
{
	m_pUpPanel->SetPos(x, y);
}

void SScrollBar::downbarSetPos(float x, float y)
{
	m_pDownPanel->SetPos(x, y);
}

void SScrollBar::upbarSetsize(float x, float y)
{
	m_pUpPanel->SetRealSize(x, y);
}

void SScrollBar::downbarSetsize(float x, float y)
{
	m_pDownPanel->SetRealSize(x, y);
}

void SScrollBar::upbarSetvisible(bool bSet)
{
	;
}

void SScrollBar::downbarSetvisible(bool bSet)
{
	;
}

void SScrollBar::scrollbarSetvisible(bool bSet)
{
	m_pBar->SetVisible(bSet);
}

void SScrollBar::scrollbarbackSetvisible(bool bSet)
{
	m_pBackground1->SetVisible(bSet);
}

SListBoxItem::SListBoxItem(const char* istrText, unsigned int idwFontColor, float inX, float inY, float inWidth, float inHeight, int ibBorder, unsigned int idwBorderColor, unsigned int dwType, unsigned int dwAlignType)
	: SText(-1,
		istrText,
		idwFontColor,
		inX,
		inY,
		inWidth,
		inHeight,
		ibBorder,
		idwBorderColor,
		dwType,
		dwAlignType)
{
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_LISTBOXITEM;
	m_bBGColor = 0;
	m_dwTime = 0;

}

SListBoxItem::~SListBoxItem()
{
}

void SListBoxItem::FrameMove2(stGeomList* pDrawList, TMVector2 ivItemPos, int inParentLayer, int nFlag)
{
	if (m_bBGColor == 0)
	{
		if (nFlag == 1)
		{
			m_cBorder = 1;
			SetType(2);
		}
		else
		{
			m_cBorder = 0;
			SetType(1);
		}
	}

	SText::FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
}

SListBoxBoardItem::SListBoxBoardItem(char* szIndex, char* szVIndex, char* szTitle, char* szWriter, char* szCount, char* szDate, unsigned int dwColor, int bTitile)
	: SListBoxItem(szVIndex, 0xFFFFFFFF, 0.0f, 0.0f, 500.0f, 12.0f, 0, 0x77777777, 1, 0)
{
	unsigned int dwFontColor = 0xFFFFFFFF;

	if (strcmp(szVIndex, g_pMessageStringTable[176]) && (*szVIndex < '0' || *szVIndex > '9'))
		dwFontColor = 0xFFAAFFAA;

	m_pTitleText = new SText(-2, szTitle, dwFontColor, 44.0f, 0.0f, 260.0f, 16.0f, 1, dwColor, 1, bTitile == 1 ? 1 : 0);
	m_pWriterText = new SText(-2, szWriter, dwFontColor, 305.0f, 0.0f, 79.0f, 16.0f, 1, dwColor, 1, 1);
	m_pCountText = new SText(-2, szCount, dwFontColor, 385.0f, 0.0f, 34.0f, 16.0f, 1, dwColor, 1, 1);
	m_pDateText = new SText(-2, szDate, dwFontColor, 420.0f, 0.0f, 80.0f, 16.0f, 1, dwColor, 1, 1);

	sprintf(m_szIndex, "%s", szIndex);
}

SListBoxBoardItem::~SListBoxBoardItem()
{
	SAFE_DELETE(m_pTitleText);
	SAFE_DELETE(m_pDateText);
	SAFE_DELETE(m_pWriterText);
	SAFE_DELETE(m_pCountText);
}

void SListBoxBoardItem::FrameMove2(stGeomList* pDrawList, TMVector2 ivItemPos, int inParentLayer, int nFlag)
{
	m_pTitleText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_pWriterText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_pCountText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_pDateText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);

	SListBoxItem::FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
}

void SListBoxBoardItem::SetPos(float nPosX, float nPosY)
{
	SControl::SetPos(nPosX, nPosY);
	m_pTitleText->SetPos(nPosX + 44.0f, nPosY);
	m_pWriterText->SetPos(nPosX + 305.0f, nPosY);
	m_pCountText->SetPos(nPosX + 385.0f, nPosY);
	m_pDateText->SetPos(nPosX + 420.0f, nPosY);
}

SListBoxPartyItem::SListBoxPartyItem(char* iStrText, unsigned int idwFontColor, float inX, float inY, float inWidth, float inHeight, unsigned int dwCharID, int nClass, int nLevel, int nHp, int nMaxHp)
	: SListBoxItem(iStrText,
		idwFontColor,
		inX,
		inY,
		inWidth,
		inHeight,
		0,
		0x77777777,
		1,
		0)
{
	m_dwCharID = dwCharID;
	m_nClass = nClass;
	m_nLevel = nLevel;
	m_nState = 0;
	m_pLevelText = 0;
	m_pHpProgress = 0;

	char szLevel[16]{};
	sprintf(szLevel, "%d", m_nLevel + 1);

	m_pLevelText = new SText(-1, szLevel, 0xFFFFFFAA, 74.0f, 3.0f, 8.0f, 12.0f, 0, 0x77777777, 1, 0);
	m_pHpProgress = new SProgressBar(7, nHp, nMaxHp, 0.0f, 15.0f, 110.0f, 8.0f, 0xFFFFFFFF, 0xFFFFFFFF, 1);
	m_pDirPanel = new SPanel(-1, 66.0f, 0.0f, 16.0f, 16.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

	char* pDest = strchr(iStrText, '^');
	if (pDest && !IsClearString2(iStrText, pDest - iStrText))
		pDest = 0;

	if (pDest != nullptr)
	{
		char szMyMob[64]{};
		memset(szMyMob, 0, 64);
		memcpy(szMyMob, iStrText, pDest - iStrText);
		SetText(szMyMob, 0);
	}
	else
		SetText(iStrText, 0);
}

SListBoxPartyItem::~SListBoxPartyItem()
{
	SAFE_DELETE(m_pLevelText);
	SAFE_DELETE(m_pHpProgress);
	SAFE_DELETE(m_pDirPanel);
}

void SListBoxPartyItem::FrameMove2(stGeomList* pDrawList, TMVector2 ivItemPos, int inParentLayer, int nFlag)
{
	m_pLevelText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_pHpProgress->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);

	ivItemPos.x -= 4.0f;
	ivItemPos.y -= 1.0f;

	if (m_nState == 1)
	{
		if ((g_pTimerManager->GetServerTime() % 1000) / 500)
			m_GCText.dwColor = 0xFFFF0000;
		else
			m_GCText.dwColor = 0xFFFFFFFF;

		m_GCText.pFont->SetText(m_GCText.strString, m_GCText.dwColor, 0);
	}

	SListBoxItem::FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
}





SListBoxDropMob::SListBoxDropMob(char* iStrText, unsigned int idwFontColor, float inX, float inY, float inWidth, float inHeight, unsigned int dwCharID, int nClass, int nLevel, int nHp, int nMaxHp)
	: SListBoxItem(iStrText,
		idwFontColor,
		inX  ,
		inY,
		inWidth ,
		inHeight,
		0,
		0x77777777,
		1,
		0)
{
	m_dwCharID = dwCharID;
	m_nClass = nClass;
	m_nLevel = nLevel;
	m_nState = 0;
	m_pLevelText = 0;
	m_pHpProgress = 0;

	char szLevel[16]{};
	sprintf(szLevel, "X:%d Y:%d",2100,2100);

	m_pLevelText = new SText(-1, szLevel, 0xFFFFFFAA, inX , inY, 8.0f, 12.0f, 0, 0x77777777, 1, 0);
	m_pHpProgress = new SProgressBar(7, nHp, nMaxHp, 0.0f, 15.0f, 110.0f, 8.0f, 0xFFFFFFFF, 0xFFFFFFFF, 1);
	m_pDirPanel = new SPanel(-1, 66.0f, 0.0f, 16.0f, 16.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

	char* pDest = strchr(iStrText, '^');
	if (pDest && !IsClearString2(iStrText, pDest - iStrText))
		pDest = 0;

	if (pDest != nullptr)
	{
		char szMyMob[64]{};
		memset(szMyMob, 0, 64);
		memcpy(szMyMob, iStrText, pDest - iStrText);
		SetText(szMyMob, 0);
	}
	else
		SetText(iStrText, 0);
}

SListBoxDropMob::~SListBoxDropMob()
{
	SAFE_DELETE(m_pLevelText);
	SAFE_DELETE(m_pHpProgress);
	SAFE_DELETE(m_pDirPanel);
}

void SListBoxDropMob::FrameMove2(stGeomList* pDrawList, TMVector2 ivItemPos, int inParentLayer, int nFlag)
{
	m_pLevelText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_pHpProgress->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);

	ivItemPos.x += 4.0f;
	ivItemPos.y += 21.0f;

	if (m_nState == 1)
	{
		if ((g_pTimerManager->GetServerTime() % 1000) / 500)
			m_GCText.dwColor = 0xFFFF0000;
		else
			m_GCText.dwColor = 0xFFFFFFFF;

		m_GCText.pFont->SetText(m_GCText.strString, m_GCText.dwColor, 0);
	}

	SListBoxItem::FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
}




SListBoxRankingItem::SListBoxRankingItem(char* iStrText, unsigned int idwFontColor, float inX, float inY, float inWidth, float inHeight, int nKingdom, int nLevel, int nClass, int nEvolution, int nGuild, int nFrags, char* nGuildName)
	: SListBoxItem(iStrText,
		idwFontColor,
		inX,
		inY,
		370.0f,
		inHeight,
		0,
		0x77777777,
		1,
		0)
{

	m_nState = 0;

	char szLevel[16]{};

	if (nLevel > 0)
		sprintf(szLevel, "%d", nLevel);

	char szKing[16]{};
	sprintf(szKing, "%d", nKingdom);

	const char* evolution[5] = { "", "M", "A", "C", "SC" };
	char szEvolution[16]{};
	sprintf(szEvolution, "%s", evolution[nEvolution]);

	char szGuildState[16]{};
	sprintf(szGuildState, "%d", nGuild + 1);

	char szFrags[16]{};
	if (nFrags > -1)
		sprintf(szFrags, "%d", nFrags);

	char szGuildName[16]{};
	sprintf(szGuildName, "%s", nGuildName);

	int ckind = g_pCurrentScene->m_pMyHuman->Is2stClass();
	auto pMobData = &g_pObjectManager->m_stMobData;


	//Level text
	m_pLevelText = new SText(-1, szLevel, Yellow, 163.0f, 6.0f, 8.0f, 12.0f, 0, 0x77777777, 1, 0);

	m_KingdomPanel = new SPanel(-1, 226.0f, 6.0f, 16.0f, 16.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
	m_KingdomPanel->GetGeomControl()->eRenderType = RENDERCTRLTYPE::RENDER_IMAGE_STRETCH;

	if (nKingdom == 0)
	{
		m_KingdomPanel->GetGeomControl()->nTextureSetIndex = 167;
		m_KingdomPanel->GetGeomControl()->nTextureIndex = 4;
	}

	if (nKingdom == 7)
	{
		m_KingdomPanel->GetGeomControl()->nTextureSetIndex = 167;
		m_KingdomPanel->GetGeomControl()->nTextureIndex = 5;
	}
	if (nKingdom == 8)
	{
		m_KingdomPanel->GetGeomControl()->nTextureSetIndex = 167;
		m_KingdomPanel->GetGeomControl()->nTextureIndex = 6;
	}

	if (nKingdom == -1)
	{
		m_KingdomPanel->GetGeomControl()->nTextureSetIndex = -2;
	}

	//classe icone
	m_pClassPanel = new SPanel(-1, 200.0f, 6.0f, 16.0f, 16.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
	//m_pClassPanel->GetGeomControl()->eRenderType = RENDERCTRLTYPE::RENDER_IMAGE_STRETCH;

	if (nClass == 3)
	{
		m_pClassPanel->GetGeomControl()->nTextureSetIndex = 167;
		m_pClassPanel->GetGeomControl()->nTextureIndex = 0;
	}

	if (nClass == 1)
	{
		m_pClassPanel->GetGeomControl()->nTextureSetIndex = 167;
		m_pClassPanel->GetGeomControl()->nTextureIndex = 1;
	}

	if (nClass == 2)
	{
		m_pClassPanel->GetGeomControl()->nTextureSetIndex = 167;
		m_pClassPanel->GetGeomControl()->nTextureIndex = 2;
	}

	if (nClass == 0)
	{
		m_pClassPanel->GetGeomControl()->nTextureSetIndex = 167;
		m_pClassPanel->GetGeomControl()->nTextureIndex = 3;
	}

	if (nClass == -1)
	{
		m_pClassPanel->GetGeomControl()->nTextureSetIndex = -2;
	}

	//guild name
	m_GuildName = new SText(-1, szGuildName, 0xFFFFFFAA, 250.0f, 6.0f, 8.0f, 12.0f, 0, 0x77777777, 1, 0);

	//evolução
	m_pEvolutionText = new SText(-1, szEvolution, 0xFFFFFFAA, 318.0f, 6.0f, 8.0f, 12.0f, 0, 0x77777777, 1, 0);

	m_pFragsText = new SText(-1, szFrags, 0xFFFFFFAA, 365.0f, 6.0f, 8.0f, 12.0f, 0, 0x77777777, 1, 0);


	char* pDest = strchr(iStrText, '^');
	if (pDest && !IsClearString2(iStrText, pDest - iStrText))
		pDest = 0;

	if (pDest != nullptr)
	{
		char szMyMob[64]{};
		memset(szMyMob, 0, 64);
		memcpy(szMyMob, iStrText, pDest - iStrText);
		SetText(szMyMob, 0);
	}
	else
		SetText(iStrText, 0);
}

SListBoxRankingItem::~SListBoxRankingItem()
{
	//SAFE_DELETE(m_pRankPosition);
	SAFE_DELETE(m_pLevelText);
	SAFE_DELETE(m_pClassPanel);
	SAFE_DELETE(m_pEvolutionText);
	SAFE_DELETE(m_pGuildPanel);
	SAFE_DELETE(m_pFragsText);
	SAFE_DELETE(m_GuildName);
	SAFE_DELETE(m_KingdomPanel);


}

void SListBoxRankingItem::FrameMove2(stGeomList* pDrawList, TMVector2 ivItemPos, int inParentLayer, int nFlag)
{
	m_pLevelText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_pEvolutionText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_pFragsText->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_GuildName->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_pClassPanel->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	m_KingdomPanel->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);



	ivItemPos.x -= -16.0f;
	ivItemPos.y -= -4.0f;

	SListBoxItem::FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
}


SListBoxServerItem::SListBoxServerItem(int nTextureSet, char* iStrText, unsigned int idwFontColor, float inX, float inY, float inWidth, float inHeight, int nCount, char cCastle, char cGoldBug, int Num)
	: SListBoxItem(iStrText,
		idwFontColor,
		inX,
		inY,
		inWidth,
		inHeight,
		0,
		0x77777777,
		1,
		0)
{
	m_pBusyProgress = 0;
	m_nCurrent = 0;
	m_cCastle = cCastle;
	m_nCurrent = nCount;
	m_cConnected = 1;
	m_pCrownPanel = 0;
	m_pGoldBugPanel = 0;
	m_pAgePanel = 0;
	m_cGoldBug = cGoldBug;

	float fWidthRatio = RenderDevice::m_fWidthRatio;
	float fHeightRatio = RenderDevice::m_fHeightRatio;

	unsigned int dwCol1 = -1;
	unsigned int dwCol2 = -1;

	if (m_cCastle == 1)
		m_pCrownPanel = new SPanel(151,  100.0f * fWidthRatio, 0.1f * fHeightRatio, 16.0f * fWidthRatio, 16.0f * fHeightRatio, 0xFFFFFFFF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
	if (m_cGoldBug == 1)
		m_pGoldBugPanel = new SPanel(316,  120.0f * fWidthRatio, 1.0f* fHeightRatio, 16.0f * fWidthRatio, 16.0f * fHeightRatio, 0xFFFFFFFF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

	if (nTextureSet <= -1)
	{
		dwCol1 = 0x2FFF0000;
		dwCol2 = 0xFF222222;
	}
	if (nTextureSet <= -2)
	{
		dwCol1 = 0xFF00FF99;
		dwCol2 = 0xFF222222;
	}

	m_pBusyProgress = new SProgressBar(nTextureSet, nCount, 600, inWidth - 58.0f, 6.0f, 20.0f, 6.0f, dwCol1, dwCol2, 1);
}

SListBoxServerItem::~SListBoxServerItem()
{
	SAFE_DELETE(m_pGoldBugPanel);
	SAFE_DELETE(m_pCrownPanel);
	SAFE_DELETE(m_pBusyProgress);
}

void SListBoxServerItem::FrameMove2(stGeomList* pDrawList, TMVector2 ivItemPos, int inParentLayer, int nFlag)
{
	if (m_pCrownPanel != nullptr)
		m_pCrownPanel->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	if (m_pGoldBugPanel != nullptr)
		m_pGoldBugPanel->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
	if (m_pBusyProgress != nullptr)
		m_pBusyProgress->FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);

	SListBoxItem::FrameMove2(pDrawList, ivItemPos, inParentLayer, nFlag);
}

SListBox::SListBox(int inTextureSetIndex, int inMaxCount, int inVisibleCount, float inX, float inY, float inWidth, float inHeight, unsigned int idwColor, RENDERCTRLTYPE eRenderType, int bSelectEnable, int bScrollBar, int bEditable)
	: SPanel(inTextureSetIndex, inX, inY, inWidth, inHeight, idwColor, eRenderType)
{
	m_cScrollBar = bScrollBar;
	m_cEditable = bEditable;
	m_sEditLine = 0;
	m_nMaxCount = inMaxCount;
	m_nVisibleCount = inVisibleCount;
	m_nStartItemIndex = 0;
	m_nSelectedItem = -1;
	m_pEditLine = 0; 
	m_pScrollBar = 0;
	m_nNumItem = 0;

	if (m_nMaxCount > 1000)
		m_nMaxCount = 1000;

	m_bSelectEnable = bSelectEnable;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_LISTBOX;
	m_bRButton = 0;

	if (m_cScrollBar == 1)
	{
		m_pScrollBar = new SScrollBar(0, 0, 0.0, 0.0, (float)12, inHeight - 1.0f, 0, 0, 0xFFAAAAAA, 1);

		if (m_pScrollBar != nullptr)
		{
			m_pScrollBar->SetVisible(1);
			m_pScrollBar->SetControlID(1);
			m_pScrollBar->SetEventListener(this);
			AddChild(m_pScrollBar);
		}
	}
	else if (m_cScrollBar == 2)
	{
		m_pScrollBar = new SScrollBar(0, 0, (float)(inWidth - 12.0f) + 2.0f, 0.0f, 12.0f, inHeight - 1.0f, 0, 0, 0xFFAAAAAA, 0);

		if (m_pScrollBar != nullptr)
		{
			m_pScrollBar->SetVisible(1);
			m_pScrollBar->SetControlID(1);
			m_pScrollBar->SetEventListener(this);
			AddChild(m_pScrollBar);
		}
	}

	if (m_cEditable == 1)
		SetEditable();

	memset(m_pItemList, 0, sizeof(m_pItemList));

	m_fPickWidth = m_nWidth;
	m_fPickHeight = m_nHeight;
	m_dwSetTime = 0;
	m_dwNowTime = 0;
}

SListBox::~SListBox()
{
	Empty();
}

int SListBox::AddItem(SListBoxItem* ipNewItem)
{
	if (m_nNumItem >= m_nMaxCount - m_cEditable)
	{
		SAFE_DELETE(m_pItemList[0]);

		for (int i = 1; i < m_nNumItem; ++i)
			m_pItemList[i - 1] = m_pItemList[i];

		m_pItemList[m_nNumItem - 1] = ipNewItem;

		if (m_dwSetTime != 0)
		{
			m_pItemList[m_nNumItem - 1]->m_dwTime = timeGetTime();
			m_pItemList[m_nNumItem - 1]->m_GCText.bVisible = 1;
		}

		return 1;
	}

	m_pItemList[m_nNumItem] = ipNewItem;
	if (m_dwSetTime)
		m_pItemList[m_nNumItem]->m_dwTime = timeGetTime();

	++m_nNumItem;

	if (m_cScrollBar != 0 && m_pScrollBar != nullptr)
	{
		m_pScrollBar->SetMaxValue(m_nNumItem);
		if (m_nNumItem >= m_nVisibleCount)
		{
			SetStartItemIndex(m_cEditable + m_nNumItem - m_nVisibleCount);
			m_pScrollBar->SetMaxValue(m_nNumItem);
			m_pScrollBar->SetCurrentPos(m_cEditable + m_nNumItem - m_nVisibleCount);

		}
	}

	return 1;
}

int SListBox::DeleteItem(int inItemIndex)
{
	if (m_nNumItem <= inItemIndex || inItemIndex < 0)
		return 0;

	SAFE_DELETE(m_pItemList[inItemIndex]);

	for (int i = inItemIndex + 1; i < m_nNumItem; ++i)
		m_pItemList[i - 1] = m_pItemList[i];

	m_pItemList[m_nNumItem - 1] = 0;
	m_nNumItem--;

	if (m_cScrollBar != 0)
		m_pScrollBar->SetMaxValue(m_nNumItem);

	return 1;
}

int SListBox::DeleteItem(SListBoxItem* ipItem)
{
	int i = 0;
	for (i = 0; ; ++i)
	{
		if (i >= m_nNumItem)
			return 0;

		if (m_pItemList[i] == ipItem)
			break;
	}

	if (m_pItemList[i] != nullptr)
	{
		SAFE_DELETE(m_pItemList[i]);
	}

	for (int j = i + 1; j < m_nNumItem; ++j)
		m_pItemList[j - 1] = m_pItemList[j];

	--m_nNumItem;

	if (m_cScrollBar != 0)
		m_pScrollBar->SetMaxValue(m_nNumItem);

	return 1;
}

SListBoxItem* SListBox::GetItem(int inItemIndex)
{
	if (inItemIndex < 0 || inItemIndex > m_nNumItem)
		return nullptr;

	return m_pItemList[inItemIndex];
}

void SListBox::Empty()
{
	for (int i = 0; i < m_nNumItem; ++i)
	{
		SAFE_DELETE(m_pItemList[i]);
	}

	m_nNumItem = 0;
	if (m_cScrollBar != 0 && m_pScrollBar != nullptr)
		m_pScrollBar->SetMaxValue(m_nNumItem);
}

int SListBox::GetSelectedIndex()
{
	return m_nSelectedItem;
}

void SListBox::SetSelectedIndex(int nIndex)
{
	m_nSelectedItem = nIndex;
}

void SListBox::SetStartItemIndex(int nIndex)
{
	if (nIndex <= m_nNumItem - m_nVisibleCount)
	{
		m_nStartItemIndex = nIndex;
		return;
	}

	m_nStartItemIndex = m_nNumItem - m_nVisibleCount + 1;
	if (m_nStartItemIndex < 0)
		m_nStartItemIndex = 0;
}

void SListBox::SetSize(float nWidth, float nHeight)
{
	SControl::SetSize(nWidth, nHeight);

	m_fPickWidth = m_nWidth;
	m_fPickHeight = m_nHeight;

	if (m_pScrollBar != nullptr)
		m_pScrollBar->SetSize(m_nWidth, nHeight - 1.0f);
}

void SListBox::SetPickSize(float nWidth, float nHeight)
{
	float fWidthRatio = g_UIVer == 2 ? 1.0f : RenderDevice::m_fWidthRatio;
	float fHeightRatio = g_UIVer == 2 ? 1.0f : RenderDevice::m_fHeightRatio;

	m_fPickWidth = nWidth * fWidthRatio;
	m_fPickHeight = nHeight * fHeightRatio;
}

void SListBox::SetEditable()
{
	if (m_pEditLine == nullptr)
	{
		m_cEditable = 1;
		m_sEditLine = 0;

		m_pEditLine = new SEditableText(-2, "", 79, 0, 0xFFFFFFFF, 0.0f, 0.0f, m_nWidth, 16.0f, 0, 0, 1, 0);
		if (m_pEditLine != nullptr)
		{
			m_pEditLine->SetVisible(1);
			m_pEditLine->SetControlID(2);

			m_pEditLine->SetEventListener(this);
			AddChild(m_pEditLine);
		}
	}
}

int SListBox::OnControlEvent(DWORD idwControlID, DWORD idwEvent)
{
	if (idwControlID == 1)
	{
		SetStartItemIndex(idwEvent);
		return 1;
	}

	if (idwControlID != 2)
		return 0;

	if (idwEvent == 0 || idwEvent == 8)
	{
		if (m_sEditLine >= m_nMaxCount - 1)
			return 1;

		char* szText = m_pEditLine->GetText();

		SListBoxItem* pItem = new SListBoxItem(szText, 0xFFFFFFFF, 0.0f, 0.0f, 300.0f, 16.0f, 0, 0x77777777, 1, 0);
		AddItem(pItem);

		m_pEditLine->SetText((char*)"");
		++m_sEditLine;

		if (idwEvent == 8)
			m_pEditLine->OnCharEvent(m_pEditLine->m_cTempChar, 0);
	}
	else if (idwEvent == 7)
	{
		if (m_sEditLine > 0)
		{
			SListBoxItem* ipItem = GetItem(--m_sEditLine);
			if (ipItem == nullptr)
				return 1;

			m_pEditLine->SetText(ipItem->GetText());
			DeleteItem(ipItem);
		}
	}
	else if (idwEvent == 2)
		m_pScrollBar->Up();
	else if (idwEvent == 4)
		m_pScrollBar->Down();

	return 1;
}

void SListBox::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	SPanel::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);

	int nCurrentVisibleCount = 0;
	if (m_nVisibleCount <= m_nNumItem - m_nStartItemIndex)
		nCurrentVisibleCount = m_nVisibleCount;
	else
		nCurrentVisibleCount = m_nNumItem - m_nStartItemIndex;

	int bStr = 0;
	for (int nIndex = 0; nIndex < nCurrentVisibleCount; ++nIndex)
	{
		int nFlag = nIndex + m_nStartItemIndex == m_nSelectedItem ? 1 : 0;
		m_pItemList[nIndex + m_nStartItemIndex]->FrameMove2(pDrawList,
			TMVector2(ivParentPos.x + m_nPosX, ((ivParentPos.y + m_nPosY) + (((float)nIndex * m_nHeight) / (float)m_nVisibleCount))),
			inParentLayer,
			m_bSelectEnable & nFlag);

		if (!bStr)
		{
			SListBoxItem* pItem = m_pItemList[nIndex + m_nStartItemIndex];
			if (pItem != nullptr)
			{
				if (strlen(pItem->GetText()))
					bStr = 1;
			}
		}
	}
	if (m_dwSetTime != 0)
	{
		unsigned int dwTime = timeGetTime();
		for (int i = 0; i < m_nNumItem; ++i)
		{
			if (m_pItemList[i] != nullptr && m_dwSetTime + m_pItemList[i]->m_dwTime < dwTime)
				m_pItemList[i]->m_GCText.bVisible = 0;
		}
	}
	if (m_cEditable == 1 && m_pEditLine != nullptr)
	{
		m_pEditLine->SetPos(0.0f, ((float)(m_sEditLine - m_nStartItemIndex) * m_nHeight) / (float)m_nVisibleCount);

		if (m_sEditLine - m_nStartItemIndex > nCurrentVisibleCount || m_sEditLine - m_nStartItemIndex < 0)
			m_pEditLine->SetVisible(0);
		else
			m_pEditLine->SetVisible(1);
	}
}

int SListBox::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (!m_bSelectEnable && !m_cEditable)
		return 0;
	int bInListBox = 0;
	if (m_cScrollBar)
		bInListBox = PointInRect(nX, nY, m_nPosX, m_nPosY, m_fPickWidth - 14.0f, m_fPickHeight);
	else
		bInListBox = PointInRect(nX, nY, m_nPosX, m_nPosY, m_fPickWidth, m_fPickHeight);

	if (m_bOver)
	{
		ConsolePainelMacro(6);
	}
	if (bInListBox == 1 && dwFlags == WM_LBUTTONUP && m_cEditable == 1 && m_pEditLine != nullptr)
	{
		g_pCurrentScene->m_pControlContainer->SetFocusedControl(m_pEditLine);
	}

	if (bInListBox == 1 && (dwFlags == WM_LBUTTONDOWN || dwFlags == WM_RBUTTONDOWN))
		return 1;
	 
	if (bInListBox == 1 && dwFlags == WM_LBUTTONUP)
	{
		int nLocalIndex = (int)(((float)nY - m_nPosY) / (float)(m_nHeight / (float)m_nVisibleCount));
		if (m_nStartItemIndex + nLocalIndex < m_nNumItem)
		{
			m_nSelectedItem = m_nStartItemIndex + nLocalIndex;
			if (m_pEventListener != nullptr)
			{
				m_bRButton = 0;
				m_pEventListener->OnControlEvent(m_dwControlID, m_nSelectedItem);
			}
		}

		if (m_bSelectEnable == 1)
			return 1;
	}

	if (bInListBox != 1 || dwFlags != WM_RBUTTONUP)
		return 0;

	int nLocalIndex = (int)(((float)nY - m_nPosY) / (float)(m_nHeight / (float)m_nVisibleCount));
	if (m_nStartItemIndex + nLocalIndex < m_nNumItem)
	{
		m_nSelectedItem = m_nStartItemIndex + nLocalIndex;
		if (m_pEventListener != nullptr)
		{
			m_bRButton = 1;
			m_pEventListener->OnControlEvent(m_dwControlID, m_nSelectedItem);
		}
	}

	if (m_bSelectEnable != 1)
		return 0;

	return 1;
}
int SListBoxItem::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	/*m_bOver = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);
	Update();*/
	return 0;
}

void SListBox::SetTextTimer(unsigned int dTime)
{
	m_dwSetTime = dTime;
}






SReconnectPanel::SReconnectPanel(const char* istrMessage, float inX, float inY, float inWidth, float inHeight)
	: SPanel(-2, inX, inY, inWidth, inHeight, 0xAAFFFFFF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH)
{
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_MESSAGEPANEL;
	m_GCPanel.nTextureSetIndex = -2;
	m_GCPanel.dwColor = 0xFFFFFF;
	m_dwControlID = 265537;
	m_pBg = new SPanel(-2, inWidth / 2.0f, inHeight / 2.0f, 500.0f, 150.0f, 0xA1000000, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

	pPingTime = 0;

	m_pSalvagrupo = new SButton(-2, 17.0f, 132.0f, 50.0f, 25.0f, 0, 1, (char*)"Salvar Grupo");
	m_pSalvaPosicao = new SButton(-2, 17.0f, 132.0f, 50.0f, 25.0f, 0, 1, (char*)"Salvar Posicao");
	m_pSalvaCC = new SButton(-2, 17.0f, 132.0f, 50.0f, 25.0f, 0, 1, (char*)"Salvar C.C");
	m_pSalvaNumerica = new SButton(-2, 17.0f, 132.0f, 50.0f, 25.0f, 0, 1, (char*)"Salvar Senha Numerica");
	m_pAtivar = new SButton(-2, 17.0f, 132.0f, 50.0f, 25.0f, 0, 1, (char*)"Auto reconexão:");
	m_pFechar = new SButton(-2, 17.0f, 132.0f, 50.0f, 25.0f, 0, 1, (char*)"Sair");
	m_pTitulo = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, -100.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);

	m_pConectInfo = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, -100.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);
	m_pPingInfo = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, -100.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);
	m_pGrupoInfo = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, -100.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);
	m_pMacroCCInfo = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, -100.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);
	m_pNumericaInfo = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, -100.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);
	m_pPosicaoInfo = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, -100.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);
	m_pStatusInfo = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, -100.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);

	if (m_pTitulo != nullptr)
		AddChild(m_pTitulo);
	if (m_pPingInfo != nullptr)
		AddChild(m_pPingInfo);
	if (m_pConectInfo != nullptr)
		AddChild(m_pConectInfo);




	if (m_pGrupoInfo != nullptr)
		AddChild(m_pGrupoInfo);
	if (m_pMacroCCInfo != nullptr)
		AddChild(m_pMacroCCInfo);
	if (m_pNumericaInfo != nullptr)
		AddChild(m_pNumericaInfo);
	if (m_pPosicaoInfo != nullptr)
		AddChild(m_pPosicaoInfo);
	if (m_pStatusInfo != nullptr)
		AddChild(m_pStatusInfo);

	if (m_pSalvagrupo != nullptr)
	{
		AddChild(m_pSalvagrupo);
		m_pSalvagrupo->SetControlID(2299931);
		m_pSalvagrupo->SetEventListener(this);
	}
	if (m_pSalvaCC != nullptr)
	{
		AddChild(m_pSalvaCC);
		m_pSalvaCC->SetControlID(2299932);
		m_pSalvaCC->SetEventListener(this);
	}
	if (m_pSalvaPosicao != nullptr)
	{
		AddChild(m_pSalvaPosicao);
		m_pSalvaPosicao->SetControlID(2299934);
		m_pSalvaPosicao->SetEventListener(this);
	}
	if (m_pSalvaNumerica != nullptr)
	{
		AddChild(m_pSalvaNumerica);
		m_pSalvaNumerica->SetControlID(2299933);
		m_pSalvaNumerica->SetEventListener(this);
	}
	if (m_pFechar != nullptr)
	{
		AddChild(m_pFechar);
		m_pFechar->SetControlID(2299939);
		m_pFechar->SetEventListener(this);
	}
	if (m_pAtivar != nullptr)
	{
		AddChild(m_pAtivar);
		m_pAtivar->SetControlID(2299942);
		m_pAtivar->SetEventListener(this);
	}

	m_pBg->m_dwControlID = 2299925;
	m_pTitulo->m_dwControlID = 2299930;

	m_pGrupoInfo->m_dwControlID = 2299935;
	m_pMacroCCInfo->m_dwControlID = 2299936;
	m_pNumericaInfo->m_dwControlID = 2299937;
	m_pPosicaoInfo->m_dwControlID = 2299938;
	m_pStatusInfo->m_dwControlID = 2299943;
	m_pConectInfo->m_dwControlID = 2299940;
	m_pPingInfo->m_dwControlID = 2299941;

	if (m_pBg != nullptr)
		AddChild(m_pBg);

	if (m_pBg)
	{
		//m_pBg->m_GCPanel.nTextureSetIndex = 225;
	}

	Init(0);
}
SReconnectPanel::~SReconnectPanel()
{
	SAFE_DELETE(m_pStatusInfo);
	SAFE_DELETE(m_pGrupoInfo);
	SAFE_DELETE(m_pMacroCCInfo);
	SAFE_DELETE(m_pNumericaInfo);
	SAFE_DELETE(m_pPosicaoInfo);
}
void SReconnectPanel::Init(int Indice)
{
	for (int ia = 0; ia < 8; ++ia)
	{
		if (g_pDevice->m_pVertexShader[ia] == nullptr)
		{
			break;
			return;
		}
		if (g_pDevice->m_pVertexDeclaration[ia] == nullptr)
		{
			break;
			return;
		}
	}
	if (m_pBg)
	{

		//std::cout << "\n pReconectData.ReconectStatus " << pReconectData.ReconectStatus;
		if (pReconectData.ReconectStatus)
		{
			ShowReconnect(1);
		}
		else
		{
			ShowReconnect(0);
		}

		GetStatusReconnect();
		char str[128]{};

		g_GameAuto.MountValue = pReconectData.MacroCC[0];
		g_GameAuto.HpValue = pReconectData.MacroCC[1];
		g_GameAuto.AutoType = pReconectData.MacroCC[2];
		g_GameAuto.AutoPos.Type = pReconectData.MacroCC[3];

		if (g_pCurrentScene && g_pCurrentScene->GetSceneType() == ESCENE_TYPE::ESCENE_FIELD)
		{
			auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
			if (pFScene)
			{
				switch (g_GameAuto.AutoType)
				{
				case 0:
					pFScene->m_pMGameAutoBtn->SetTextureSetIndex(458);
					pFScene->m_pMGameAutoBtn->m_pAltText->SetText(g_UIString[229], 0);
					break;
				case 1:
					pFScene->m_pMGameAutoBtn->SetTextureSetIndex(455);
					pFScene->m_pMGameAutoBtn->m_pAltText->SetText(g_UIString[226], 0);
					break;
				case 2:
					pFScene->m_pMGameAutoBtn->SetTextureSetIndex(456);
					pFScene->m_pMGameAutoBtn->m_pAltText->SetText(g_UIString[227], 0);
					break;
				case 3:
					pFScene->m_pMGameAutoBtn->SetTextureSetIndex(459);
					pFScene->m_pMGameAutoBtn->m_pAltText->SetText(g_UIString[230], 0);
					break;
				}
				char szText[128]{};
				if (g_GameAuto.MountValue)
					sprintf_s(szText, g_pMessageStringTable[478], g_GameAuto.MountValue);
				else
					sprintf_s(szText, g_pMessageStringTable[477]);

				if (g_GameAuto.MountValue <= 0)
					g_GameAuto.MountValue = 30;

				if (g_GameAuto.HpValue <= 0)
					g_GameAuto.HpValue = 30;


				sprintf_s(szText, "%d%%", g_GameAuto.MountValue);
				if (pFScene->m_pCCModeMountSte)
				{
					pFScene->m_pCCModeMountSte->SetText(szText, 0);
				}

				sprintf_s(szText, "%d%%", g_GameAuto.HpValue);
				if (pFScene->m_pCCModeMountSte)
					pFScene->m_pCCModeHpSte->SetText(szText, 0);

				if (g_GameAuto.AutoPos.Type == 0)
				{
					if (pFScene->m_pSetType)
					{
						pFScene->m_pSetType->SetTextureSetIndex(463);
						pFScene->m_pSetType->m_pAltText->SetText(g_UIString[232], 0);
					}
				}
				else if (g_GameAuto.AutoPos.Type == 1)
				{
					if (pFScene->m_pSetType)
					{
						pFScene->m_pSetType->SetTextureSetIndex(464);

						pFScene->m_pSetType->m_pAltText->SetText(g_UIString[233], 0);
					}
				}
				else if (g_GameAuto.AutoPos.Type == 2)
				{
					if (pFScene->m_pSetType)
					{
						pFScene->m_pSetType->SetTextureSetIndex(465);
						pFScene->m_pSetType->m_pAltText->SetText((char*)"", 0);
					}
				}
				 

			}
		}

		if (strlen(pReconectData.keypass) >= 4)
		{
			m_pNumericaInfo->SetText((char*)"******", 0);
		}
		else
		{
			m_pNumericaInfo->SetText((char*)"??????", 0);
		}

		m_pConectInfo->SetText((char*)"Estatus da conexão:", 0);
		if (strlen(pReconectData.GrupoLeader))
		{
			sprintf(str, "Líder:%s", pReconectData.GrupoLeader);
		}
		else
		{
			snprintf(str, sizeof str, "Não salvo");
		}

		m_pGrupoInfo->SetText((char*)str, 0);

		memset(&str, 0, sizeof str);
		bool MacroCC = false;
		for (size_t i = 0; i < 4; i++)
		{
			if (pReconectData.MacroCC[i])
			{
				MacroCC = true;
				break;
			}
		}
		if (MacroCC == true)
		{
			
			g_GameAuto.MountValue = pReconectData.MacroCC[0];
			g_GameAuto.HpValue = pReconectData.MacroCC[1];
			g_GameAuto.AutoType = pReconectData.MacroCC[2];
			g_GameAuto.AutoPos.Type = pReconectData.MacroCC[3];
			SetMacroCC("Salvo");
		}
		else
		{
			SetMacroCC("Não salvo");
		}
		memset(&str, 0, sizeof str);

		bool isPos = (pReconectData.PosX > 0 && pReconectData.PosX < 4096 && pReconectData.PosY > 0 && pReconectData.PosY < 4096);
		if (isPos == true)
		{
			snprintf(str, sizeof str, "X: %d - Y:%d", pReconectData.PosX, pReconectData.PosY);
			m_pPosicaoInfo->SetText((char*)str, 0);
		}
		else
		{
			m_pPosicaoInfo->SetText((char*)"Não salvo", 0);
		}

	}
}
int SReconnectPanel::GetStatusReconnect()
{
	if (pReconectData.ReconectStatus)
	{
		m_pStatusInfo->SetTextColor(0xFF00FF00);
		m_pStatusInfo->SetText((char*)"Ativado", 0);
	}
	else
	{
		m_pStatusInfo->SetTextColor(0xFFFF0000);
		m_pStatusInfo->SetText((char*)"Desativado", 0);
	}
	return pReconectData.ReconectStatus;
}
int SReconnectPanel::OnControlEvent(DWORD idwControlID, DWORD idwEvent)
{
	if (g_pCurrentScene == nullptr)
		return 0;

	//printf("\n 0x%X - idwControlID %d", (int)g_pCurrentScene->GetSceneType(), idwControlID);
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	auto pCScene = static_cast<TMSelectCharScene*>(g_pCurrentScene);

	if (idwControlID && m_bVisible && idwControlID != 2299932)
	{
		CheckServerConnection();
	}
	 
	// Handle control events here	 
	if (idwControlID == 544612)
	{
		STRUCT_SELCHAR* pSelChar = &g_pObjectManager->m_stSelCharData;
		int nSlot = pReconectData.LastPerson;
		 

		if (nSlot >= 0 && nSlot < 4)
		{
			if (pCScene->m_pHuman[nSlot] && pSelChar->MobName[nSlot][0])
			{

				pCScene->m_pBtnLogin->SetEnable(0);
				pCScene->m_pBtnCancel->SetEnable(0);
				pCScene->m_pBtnDelete->SetEnable(0);
				MSG_CharacterLogin stCharacterLogin{};
				stCharacterLogin.Header.ID = g_pObjectManager->m_dwCharID;
				stCharacterLogin.Header.Type = MSG_CharacterLogin_Opcode;
				stCharacterLogin.Slot = nSlot;
				g_pSocketManager->SendOneMessage((char*)&stCharacterLogin, sizeof(stCharacterLogin));
			}
		}
		else
		{
			pCScene->m_pMessagePanel->SetMessage(g_pMessageStringTable[14], 2000);
			pCScene->m_pMessagePanel->SetVisible(1, 1);
		}
		return 1;
	}
	switch (idwControlID)
	{
	case 2299942:
	{
		auto pMobData = &g_pObjectManager->m_stMobData; 
		pFScene->pReconectTime = g_pTimerManager->GetServerTime();
		pReconectData.ReconectStatus = pReconectData.ReconectStatus == false;

		if (pReconectData.ReconectStatus)
		{
			strcpy(pReconectData.GrupoLeader, pMobData->MobName);
			pReconectStep = 1;
			pFScene->m_pMessagePanel->SetMessage((char*)"Auto reconexão ativada!", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
		}
		else
		{
			pFScene->m_pMessagePanel->SetMessage((char*)"Auto reconexão desativada!", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
		}


		GetStatusReconnect();
		break;
	}

	case 360000u:
	{
		pFScene->pReconectTime = g_pTimerManager->GetServerTime();
		pReconectData.PosX = (int)pFScene->m_pMyHuman->m_vecPosition.x;
		pReconectData.PosY = (int)pFScene->m_pMyHuman->m_vecPosition.y;
		std::cout << "\n Posição x atualizada " << pReconectData.PosX;
		std::cout << "\n Posição y atualizada " << pReconectData.PosY;
		pFScene->m_pMessagePanel->SetMessage("Posição salva!", 1000);
		pFScene->m_pMessagePanel->SetVisible(1, 1);
		break;
	}
	case 350000u:
	{
		pFScene->pReconectTime = g_pTimerManager->GetServerTime();
		short posX = pReconectData.PosX;
		short posY = pReconectData.PosY;

		pReconectData.RecoveryPos = false;

		if (posX <= 0 || posY <= 0)
		{
			pReconectData.PosX = (int)pFScene->m_pMyHuman->m_vecPosition.x;
			pReconectData.PosY = (int)pFScene->m_pMyHuman->m_vecPosition.y;
			pFScene->m_pMessagePanel->SetMessage("Posição não salva! Recuperação de posição cancelada.", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
			pReconectStep = -1;
			SetVisible(0);
			return 0;
		}

		struct	MSG_Reconect
		{
			MSG_STANDARD Header;
			int  Indice;
			int  Indice2;
		};

		MSG_Reconect st{};
		st.Header.ID = g_pObjectManager->m_dwCharID;
		st.Header.Type = 0xAF5;
		st.Indice = posX;
		st.Indice2 = posY;
		g_pSocketManager->SendOneMessage((char*)&st, sizeof(st));
		break;
	}
	case 2299931u:
	{
		pFScene->pReconectTime = g_pTimerManager->GetServerTime();


		char strs[56]{};

		pReconectData.GrupoRecovery = pReconectData.GrupoRecovery == 0;


		int LiderLen = strlen(pReconectData.GrupoLeader);

		if (pReconectData.GrupoRecovery)
		{
			pFScene->m_pMessagePanel->SetMessage("Auto recuperação de grupo enviada.", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
		}
		else
		{
			pFScene->m_pMessagePanel->SetMessage("Auto recuperação de grupo desativado.", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
		}
		break;
	}
	case 22999313u:
	{

		pFScene->pReconectTime = g_pTimerManager->GetServerTime();


		char strs[56]{};



		int LiderLen = strlen(pReconectData.GrupoLeader);

		auto pMobData = &g_pObjectManager->m_stMobData;


		if (LiderLen >= 4)//CHECAR SE EXISTE NOME DE ALGUM LIDER SALVO
		{

			if (!strcmp(pReconectData.GrupoLeader, pMobData->MobName))//eu sou o lider
			{
				EnviarPacote(1);
			}
			else // não sou lider e quero recuperar meu grupo
			{
				EnviarPacote(2);
			}
		}
		else
		{ 
			memset(&pReconectData.GrupoLeader, 0, sizeof(pReconectData.GrupoLeader));
			strncpy(pReconectData.GrupoLeader, pMobData->MobName, sizeof(pReconectData.GrupoLeader) - 1);
			EnviarPacote(1);
		}

		UpdateGrupo();
		pReconectData.GrupoRecovery = false;
		SetVisible(0);
		break;
	}
	case 2299932:
	{
		if (g_pCurrentScene->GetSceneType() != ESCENE_TYPE::ESCENE_FIELD)
		{
			pFScene->m_pMessagePanel->SetMessage((char*)"Ação não permitida", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
			return 0;
		}

		pReconectData.MacroCC[0] = g_GameAuto.MountValue;
		pReconectData.MacroCC[1] = g_GameAuto.HpValue;
		pReconectData.MacroCC[2] = g_GameAuto.AutoType;
		pReconectData.MacroCC[3] = g_GameAuto.AutoPos.Type;

		pFScene->m_pMessagePanel->SetMessage("Macro CC gravado com sucesso.", 2000);
		pFScene->m_pMessagePanel->SetVisible(1, 1);
		SetMacroCC("Salvo");
		// Handle save CC button click
		break;
	}
	case 2299933:
	{ 
		break;
	}
	case 2299934:
	{
		if (g_pCurrentScene->GetSceneType() != ESCENE_TYPE::ESCENE_FIELD)
		{
			pFScene->m_pMessagePanel->SetMessage((char*)"Ação não permitida", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
			return 0;
		}
		pFScene->pReconectTime = g_pTimerManager->GetServerTime();


		char strs[56]{};

		pReconectData.RecoveryPos = pReconectData.RecoveryPos == 0;

		if (pReconectData.RecoveryPos)
		{
			pReconectData.PosX = (int)pFScene->m_pMyHuman->m_vecPosition.x;
			pReconectData.PosY = (int)pFScene->m_pMyHuman->m_vecPosition.y;

			pFScene->m_pMessagePanel->SetMessage("Auto recuperação de posição ativado.", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
		}
		else
		{
			pFScene->m_pMessagePanel->SetMessage("Auto recuperação de posição desativado.", 2000);
			pFScene->m_pMessagePanel->SetVisible(1, 1);
		}
		sprintf(strs, "X: %d - Y:%d", pReconectData.PosX, pReconectData.PosY);
		m_pPosicaoInfo->SetText((char*)strs, 0);

		break;
	}
	case 2299939:
	{
		// Handle close button click
		SetVisible(0);
		break;
	}
	default:
		return 0;
		break;
	}
	return 0;
}
void SReconnectPanel::SetMacroCC(const char* istrMessage)
{
	m_pMacroCCInfo->SetText((char*)istrMessage, 0);
}

void SReconnectPanel::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	m_bVisible = 0;
	if (pPingTime == 0)
	{
		pPingTime = g_pTimerManager->GetServerTime();
	}
	if (!Initialize || Initialize)
	{
		/*if (Initialize)
			return;*/

		switch (RenderDevice::m_fResBase)
		{
		case 1:
		{
			m_pTitulo->SetPos(BASE_ScreenResize(30.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-250.0f * RenderDevice::m_fHeightRatio));
			m_pGrupoInfo->SetPos(BASE_ScreenResize(-135.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-200.0f * RenderDevice::m_fHeightRatio));
			m_pMacroCCInfo->SetPos(BASE_ScreenResize(-35.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-200.0f * RenderDevice::m_fHeightRatio));
			m_pNumericaInfo->SetPos(BASE_ScreenResize(55.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-200.0f * RenderDevice::m_fHeightRatio));
			m_pPosicaoInfo->SetPos(BASE_ScreenResize(170.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-200.0f * RenderDevice::m_fHeightRatio));
			m_pSalvagrupo->SetPos(BASE_ScreenResize(220.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(100.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaCC->SetPos(BASE_ScreenResize(320.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(100.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaNumerica->SetPos(BASE_ScreenResize(420.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(100.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaPosicao->SetPos(BASE_ScreenResize(520.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(100.0f * RenderDevice::m_fHeightRatio));
			m_pFechar->SetPos(BASE_ScreenResize(520.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(15.0f * RenderDevice::m_fHeightRatio));
			m_pBg->SetPos(BASE_ScreenResize(150.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(15.0f * RenderDevice::m_fHeightRatio));
			m_pConectInfo->SetPos(BASE_ScreenResize(-170.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-270.0f * RenderDevice::m_fHeightRatio));
			m_pPingInfo->SetPos(BASE_ScreenResize(-75.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-270.0f * RenderDevice::m_fHeightRatio));
			m_pStatusInfo->SetPos(BASE_ScreenResize(-90.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-250.0f * RenderDevice::m_fHeightRatio));
			m_pAtivar->SetPos(BASE_ScreenResize(175.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(35.0f * RenderDevice::m_fHeightRatio));


			break;
		}

		case 2:
		{

			//m_pTitulo->SetPos(BASE_ScreenResize(30.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-150.0f * RenderDevice::m_fHeightRatio));
			m_pBg->SetPos(BASE_ScreenResize(300.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(35.0f * RenderDevice::m_fHeightRatio));
			m_pFechar->SetPos(BASE_ScreenResize(680.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(35.0f * RenderDevice::m_fHeightRatio));

			m_pAtivar->SetPos(BASE_ScreenResize(320.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(53.0f * RenderDevice::m_fHeightRatio));
			m_pStatusInfo->SetPos(BASE_ScreenResize(-80.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-315.0f * RenderDevice::m_fHeightRatio));

			m_pPingInfo->SetPos(BASE_ScreenResize(-55.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-335.0f * RenderDevice::m_fHeightRatio));
			m_pConectInfo->SetPos(BASE_ScreenResize(-135.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-335.0f * RenderDevice::m_fHeightRatio));

			m_pSalvaCC->SetPos(BASE_ScreenResize(420.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(120.0f * RenderDevice::m_fHeightRatio));
			m_pSalvagrupo->SetPos(BASE_ScreenResize(320.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(120.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaNumerica->SetPos(BASE_ScreenResize(520.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(120.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaPosicao->SetPos(BASE_ScreenResize(620.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(120.0f * RenderDevice::m_fHeightRatio));






			m_pGrupoInfo->SetPos(BASE_ScreenResize(-150.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-260.0f * RenderDevice::m_fHeightRatio));
			m_pMacroCCInfo->SetPos(BASE_ScreenResize(-50.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-260.0f * RenderDevice::m_fHeightRatio));
			m_pNumericaInfo->SetPos(BASE_ScreenResize(50.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-260.0f * RenderDevice::m_fHeightRatio));
			m_pPosicaoInfo->SetPos(BASE_ScreenResize(150.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-260.0f * RenderDevice::m_fHeightRatio));



			break;
		}

		case 3:
		{
			m_nPosY = -105.0f;
			//m_pTitulo->SetPos(BASE_ScreenResize(30.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-150.0f * RenderDevice::m_fHeightRatio));
			m_pBg->SetPos(BASE_ScreenResize(455.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(70.0f * RenderDevice::m_fHeightRatio));
			m_pFechar->SetPos(BASE_ScreenResize(770.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(70.0f * RenderDevice::m_fHeightRatio));

			m_pAtivar->SetPos(BASE_ScreenResize(465.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(80.0f * RenderDevice::m_fHeightRatio));
			m_pStatusInfo->SetPos(BASE_ScreenResize(-70.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-230.0f * RenderDevice::m_fHeightRatio));

			m_pPingInfo->SetPos(BASE_ScreenResize(-50.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-240.0f * RenderDevice::m_fHeightRatio));
			m_pConectInfo->SetPos(BASE_ScreenResize(-125.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-240.0f * RenderDevice::m_fHeightRatio));

			m_pSalvagrupo->SetPos(BASE_ScreenResize(480.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(150.0f * RenderDevice::m_fHeightRatio));
			m_pGrupoInfo->SetPos(BASE_ScreenResize(-120.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-180.0f * RenderDevice::m_fHeightRatio));

			m_pSalvaCC->SetPos(BASE_ScreenResize(570.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(150.0f * RenderDevice::m_fHeightRatio));
			m_pMacroCCInfo->SetPos(BASE_ScreenResize(-35.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-180.0f * RenderDevice::m_fHeightRatio));

			m_pSalvaNumerica->SetPos(BASE_ScreenResize(660.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(150.0f * RenderDevice::m_fHeightRatio));
			m_pNumericaInfo->SetPos(BASE_ScreenResize(60.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-180.0f * RenderDevice::m_fHeightRatio));

			m_pSalvaPosicao->SetPos(BASE_ScreenResize(750.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(150.0f * RenderDevice::m_fHeightRatio));
			m_pPosicaoInfo->SetPos(BASE_ScreenResize(145.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-180.0f * RenderDevice::m_fHeightRatio));



			break;
		}

		case 4:
		{

			//m_pTitulo->SetPos(BASE_ScreenResize(30.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-150.0f * RenderDevice::m_fHeightRatio));
			m_pBg->SetPos(BASE_ScreenResize(480.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(35.0f * RenderDevice::m_fHeightRatio));
			m_pFechar->SetPos(BASE_ScreenResize(840.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(35.0f * RenderDevice::m_fHeightRatio));

			m_pAtivar->SetPos(BASE_ScreenResize(495.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(53.0f * RenderDevice::m_fHeightRatio));
			m_pStatusInfo->SetPos(BASE_ScreenResize(-80.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-322.0f * RenderDevice::m_fHeightRatio));

			m_pPingInfo->SetPos(BASE_ScreenResize(-55.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-335.0f * RenderDevice::m_fHeightRatio));
			m_pConectInfo->SetPos(BASE_ScreenResize(-135.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-335.0f * RenderDevice::m_fHeightRatio));

			m_pSalvagrupo->SetPos(BASE_ScreenResize(500.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(120.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaCC->SetPos(BASE_ScreenResize(620.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(120.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaNumerica->SetPos(BASE_ScreenResize(720.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(120.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaPosicao->SetPos(BASE_ScreenResize(820.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(120.0f * RenderDevice::m_fHeightRatio));






			m_pGrupoInfo->SetPos(BASE_ScreenResize(-150.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-265.0f * RenderDevice::m_fHeightRatio));
			m_pMacroCCInfo->SetPos(BASE_ScreenResize(-25.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-265.0f * RenderDevice::m_fHeightRatio));
			m_pNumericaInfo->SetPos(BASE_ScreenResize(75.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-265.0f * RenderDevice::m_fHeightRatio));
			m_pPosicaoInfo->SetPos(BASE_ScreenResize(175.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-265.0f * RenderDevice::m_fHeightRatio));



			break;
		}

		case 5:
		{

			//m_pTitulo->SetPos(BASE_ScreenResize(30.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-150.0f * RenderDevice::m_fHeightRatio));
			m_pBg->SetPos(BASE_ScreenResize(780.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(75.0f * RenderDevice::m_fHeightRatio));
			m_pFechar->SetPos(BASE_ScreenResize(1090.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(75.0f * RenderDevice::m_fHeightRatio));

			m_pAtivar->SetPos(BASE_ScreenResize(790.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(85.0f * RenderDevice::m_fHeightRatio));
			m_pStatusInfo->SetPos(BASE_ScreenResize(-70.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-444.0f * RenderDevice::m_fHeightRatio));

			m_pPingInfo->SetPos(BASE_ScreenResize(-40.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-458.0f * RenderDevice::m_fHeightRatio));
			m_pConectInfo->SetPos(BASE_ScreenResize(-120.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-458.0f * RenderDevice::m_fHeightRatio));

			m_pSalvagrupo->SetPos(BASE_ScreenResize(800.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(150.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaCC->SetPos(BASE_ScreenResize(895.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(150.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaNumerica->SetPos(BASE_ScreenResize(980.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(150.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaPosicao->SetPos(BASE_ScreenResize(1080.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(150.0f * RenderDevice::m_fHeightRatio));






			m_pGrupoInfo->SetPos(BASE_ScreenResize(-125.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-395.0f * RenderDevice::m_fHeightRatio));
			m_pMacroCCInfo->SetPos(BASE_ScreenResize(-25.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-395.0f * RenderDevice::m_fHeightRatio));
			m_pNumericaInfo->SetPos(BASE_ScreenResize(75.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-395.0f * RenderDevice::m_fHeightRatio));
			m_pPosicaoInfo->SetPos(BASE_ScreenResize(155.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-395.0f * RenderDevice::m_fHeightRatio));



			break;
		}
		default:
		{
			m_pTitulo->SetPos(BASE_ScreenResize(30.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-250.0f * RenderDevice::m_fHeightRatio));
			m_pGrupoInfo->SetPos(BASE_ScreenResize(-135.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-200.0f * RenderDevice::m_fHeightRatio));
			m_pMacroCCInfo->SetPos(BASE_ScreenResize(-35.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-200.0f * RenderDevice::m_fHeightRatio));
			m_pNumericaInfo->SetPos(BASE_ScreenResize(55.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-200.0f * RenderDevice::m_fHeightRatio));
			m_pPosicaoInfo->SetPos(BASE_ScreenResize(170.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-200.0f * RenderDevice::m_fHeightRatio));
			m_pSalvagrupo->SetPos(BASE_ScreenResize(220.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(100.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaCC->SetPos(BASE_ScreenResize(320.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(100.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaNumerica->SetPos(BASE_ScreenResize(420.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(100.0f * RenderDevice::m_fHeightRatio));
			m_pSalvaPosicao->SetPos(BASE_ScreenResize(520.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(100.0f * RenderDevice::m_fHeightRatio));
			m_pFechar->SetPos(BASE_ScreenResize(520.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(15.0f * RenderDevice::m_fHeightRatio));
			m_pBg->SetPos(BASE_ScreenResize(150.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(15.0f * RenderDevice::m_fHeightRatio));
			m_pConectInfo->SetPos(BASE_ScreenResize(-170.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-270.0f * RenderDevice::m_fHeightRatio));
			m_pPingInfo->SetPos(BASE_ScreenResize(-75.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-270.0f * RenderDevice::m_fHeightRatio));
			m_pStatusInfo->SetPos(BASE_ScreenResize(-90.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(-250.0f * RenderDevice::m_fHeightRatio));
			m_pAtivar->SetPos(BASE_ScreenResize(175.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(35.0f * RenderDevice::m_fHeightRatio));


			break;
		}
		}

		Initialize = 1;
	}
	SPanel::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);
}
void SReconnectPanel::UpdateMacroCC(int mode)
{
	if (g_pCurrentScene == nullptr)
		return;

	if (pReconectData.ReconectStatus == false)
		return;
	
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

	if (mode == 1)
	{
		if (pReconectData.MacroCC[0])
			g_GameAuto.MountValue = pReconectData.MacroCC[0];
		if (pReconectData.MacroCC[1])
			g_GameAuto.HpValue = pReconectData.MacroCC[1];
		if (pReconectData.MacroCC[2])
			g_GameAuto.AutoType = pReconectData.MacroCC[2];
		if (pReconectData.MacroCC[3])
			g_GameAuto.AutoPos.Type = pReconectData.MacroCC[3];

		if (g_GameAuto.AutoType && pFScene->m_pccmode && pFScene->m_pMGameAutoBtn)
		{
			if (g_GameAuto.AutoPos.Type == 0)
			{
				pFScene->m_pSetType->SetTextureSetIndex(463);
				pFScene->m_pSetType->m_pAltText->SetText(g_UIString[232], 0);
			}
			else if (g_GameAuto.AutoPos.Type == 1)
			{
				pFScene->m_pSetType->SetTextureSetIndex(464);

				pFScene->m_pSetType->m_pAltText->SetText(g_UIString[233], 0);
			}
			else if (g_GameAuto.AutoPos.Type == 2)
			{
				pFScene->m_pSetType->SetTextureSetIndex(465);
				pFScene->m_pSetType->m_pAltText->SetText((char*)"", 0);
			}
			 

			int slotchar = pReconectData.LastPerson;
			int isPosX = pReconectData.PosX;
			int isPosY = pReconectData.PosY;

			if (isPosX && isPosY)
			{
				pFScene->m_AutoStartPointX = isPosX;
				pFScene->m_AutoStartPointY = isPosY;
			}
		}
	}
	else if (mode == 0)
	{
		pReconectData.MacroCC[0] = g_GameAuto.MountValue;
		pReconectData.MacroCC[1] = g_GameAuto.HpValue;
		pReconectData.MacroCC[2] = g_GameAuto.AutoType;
		pReconectData.MacroCC[3] = g_GameAuto.AutoPos.Type;
	}
}
void SReconnectPanel::ShowReconnect(int bVisible)
{
	if (g_pCurrentScene == nullptr)
		return;

	if (bVisible)
	{
		GetStatusReconnect();
	}
	SPanel::SetVisible(0);
}
void SReconnectPanel::CheckServerConnection()
{
	if (g_pCurrentScene == nullptr)
		return;


	if (pPingTime + 1000 > CurrentTime)
	{ 
		return;
	}

	pPingTime = CurrentTime;



	int ip = inet_addr(g_pApp->m_szServerIP);

	// Medindo o ping
	int ping = g_pSocketManager->MeasurePing(g_pApp->m_szServerIP, TM_CONNECTION_PORT, ip);
	ping /= 4;
	if (ping == -1) {
		std::cout << "Erro ao medir o ping." << std::endl;

		m_pPingInfo->SetTextColor(0xFFFF0000);
		m_pPingInfo->SetText((char*)"Falha na conexão", 0);
		return;
	}

	const char* connectionCategory;
	int textColor;

	if (ping < 25) {
		connectionCategory = "excelente";
		textColor = 0xFF00FF00; // Verde
	}
	else if (ping < 50) {
		connectionCategory = "ótima";
		textColor = 0xFF0000FF; // Azul
	}
	else if (ping < 75) {
		connectionCategory = "boa";
		textColor = 0xFF00FFFF; // Vermelho
	}
	else {
		connectionCategory = "ruim";
		textColor = 0xFFFF0000; // Vermelho
	}

	char connectionText[256];
	snprintf(connectionText, sizeof(connectionText), "%s (%d ms)", connectionCategory, ping);
	m_pPingInfo->SetText(connectionText, 0);
	m_pPingInfo->SetTextColor(textColor);

}
void SReconnectPanel::EnviarPacote(int mode)
{

	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	if (g_pCurrentScene == nullptr)
		return;

	if (g_pCurrentScene->GetSceneType() != ESCENE_TYPE::ESCENE_FIELD)
		return;
	std::cout << "\n mode " << mode;
	if (mode == 1)
	{
		MSG_MessageWhisper stWhisper{};

		stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
		stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

		sprintf_s(stWhisper.MobName, "passgrupo");

		SendOneMessage((char*)&stWhisper, sizeof(stWhisper));
		pFScene->SendListaGrupo();

	}
	if (mode == 2)
	{
		MSG_MessageWhisper stWhisper{};

		stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
		stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

		sprintf_s(stWhisper.MobName, "mgetin");
		sprintf_s(stWhisper.String, "%s", pReconectData.GrupoLeader);
		sprintf_s(stWhisper.String1, "%s", pReconectData.GrupoPass);
		g_pSocketManager->SendOneMessage((char*)&stWhisper, sizeof(stWhisper));

	}


}
void SReconnectPanel::UpdateGrupo()
{

	if (g_pCurrentScene == nullptr)
		return;

	char str[128]{};
	if (strlen(pReconectData.GrupoLeader))
	{
		sprintf(str, "Líder:%s", pReconectData.GrupoLeader);
	}
	else
	{
		snprintf(str, sizeof str, "Não salvo");
	}

	m_pGrupoInfo->SetText((char*)str, 0);
}









SMessageBox::SMessageBox(const char* istrMessage, char ibyMessageBoxType, float inX, float inY)
	: SPanel(-2, inX, inY, 256.0f, 172.0f, 0x1010101, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH)
{
	m_dwArg = 0;
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_MESSAGEBOX;
	m_dwControlID = 4617;
	m_dwMessage = -1;
	SetCenterPos(m_dwControlID, inX, inY, 256.0f * RenderDevice::m_fWidthRatio, 172.0f * RenderDevice::m_fWidthRatio);

	m_pPanel1 = new SPanel(501, 0.0, 0.0, 256.0f, 172.0f, 0x77777777, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
	m_pMessage = new SText(-1, istrMessage, 0xFFFFFFFF, 20.0f, 36.0f, (float)strlen(istrMessage) * 6.0f, 52.0f, -1, 1, 0, 0);
	m_pMessage2 = new SText(-1, istrMessage, 0xFFFFFFFF, 20.0f, 52.0f, (float)strlen(istrMessage) * 6.0f, 52.0f, -1, 1, 0, 0);
	m_pCaption = new SText(-1, g_pMessageStringTable[237], 0xFFFFFFFF, 96.0f, 9.0f,
		(float)strlen(g_pMessageStringTable[237]) * 6.0f, 22.0f, -1, 1, 0, 0);

	m_bPickable = 1;

	if (m_pCaption != nullptr)
		AddChild(m_pCaption);
	if (m_pMessage != nullptr)
		AddChild(m_pMessage);
	if (m_pMessage2 != nullptr)
		AddChild(m_pMessage2);

	m_pMessage->SetText((char*)"", 0);
	m_pMessage2->SetText((char*)"", 0);
	m_byMessageBoxType = ibyMessageBoxType;

	if (ibyMessageBoxType == TMC_MESSAGEBOX_MESSAGE)
	{
		m_pOKButton = new SButton(-2, 17.0f, 132.0f, 103.0f, 21.0f, 0, 1, g_pMessageStringTable[238]);

		if (m_pOKButton != nullptr)
		{
			m_pOKButton->SetControlID(1);
			m_pOKButton->SetEventListener(this);
			AddChild(m_pOKButton);
		}

		m_pCancelButton = new SButton(-2, 137.0f, 132.0f, 103.0f, 21.0f, 0, 1, g_pMessageStringTable[239]);
		if (m_pCancelButton != nullptr)
		{
			m_pCancelButton->SetControlID(2);
			m_pCancelButton->SetEventListener(this);
			AddChild(m_pCancelButton);
		}
	}
	else if (ibyMessageBoxType == TMC_MESSAGEBOX_OK)
	{
		m_pPanelBtn1 = new SPanel(165, 70.0f, 76.0f, 88.0f, 23.0f, 0x77777777, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		m_pOKButton = new SButton(-2, 70.0f, 76.0f, 88.0f, 23.0f, 0, 1, g_pMessageStringTable[238]);

		if (m_pOKButton != nullptr)
		{
			m_pOKButton->SetControlID(1);
			m_pOKButton->SetEventListener(this);
			AddChild(m_pOKButton);
		}
	}

	if (m_pPanel1 != nullptr)
		AddChild(m_pPanel1);
}

SMessageBox::~SMessageBox()
{
}

int SMessageBox::OnControlEvent(DWORD idwControlID, DWORD idwEvent)
{

	if (idwControlID == 20589)
	{

		return 1;
	}

	if (idwControlID == m_pOKButton->GetControlID() &&
		!idwEvent &&
		m_pEventListener != nullptr)
	{
		SetVisible(0);

		m_pEventListener->OnControlEvent(m_dwControlID, 0);
		return 1;
	}
	else if (m_pCancelButton != nullptr &&
		idwControlID == m_pCancelButton->GetControlID() &&
		!idwEvent &&
		m_pEventListener != nullptr)
	{
		SetVisible(0);

		m_pEventListener->OnControlEvent(m_dwControlID, 1);
		return 1;
	}

	return 0;
}

void SMessageBox::SetMessage(char* istrMessage, unsigned int dwMessageValue, char* istrMessage2)
{
	m_dwMessage = dwMessageValue;

	if (strlen(istrMessage) > 34 && !istrMessage2)
	{
		LPSTR pPrev = CharPrev(istrMessage, &istrMessage[34]);
		int nLen = pPrev - istrMessage;

		char szMessage[128]{};
		strncpy(szMessage, istrMessage, nLen);

		m_pMessage->SetText(szMessage, 0);
		m_pMessage2->SetText(&istrMessage[nLen], 0);
	}
	else
	{
		m_pMessage->SetText(istrMessage, 0);
		m_pMessage2->SetText(istrMessage2, 0);
	}
}

void SMessageBox::SetMessage(unsigned int dwMessageValue)
{
	m_dwMessage = dwMessageValue;
}

unsigned int SMessageBox::GetMessageA()
{
	return m_dwMessage;
}

int SMessageBox::OnCharEvent(char iCharCode, int lParam)
{
	if (m_pEventListener == nullptr)
		return 0;

	if (iCharCode == VK_RETURN || iCharCode == 'Y' || iCharCode == 'y')
	{
		SetVisible(0);
		m_pEventListener->OnControlEvent(m_dwControlID, 0);
		return 1;
	}

	if (iCharCode != 'N' && iCharCode != 'n')
		return 0;

	SetVisible(0);
	m_pEventListener->OnControlEvent(m_dwControlID, 1);
	return 1;
}

void SMessageBox::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	SControl::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);
}

void SMessageBox::SetVisible(int bVisible)
{
	SControl::SetVisible(bVisible);

	if (g_pCurrentScene != nullptr)
	{
		if (bVisible == 1)
		{
			SControlContainer* Ctrl = g_pCurrentScene->GetCtrlContainer();
			if (Ctrl != nullptr)
				Ctrl->SetFocusedControl(this);
		}
		else
		{
			SControlContainer* Ctrl = g_pCurrentScene->GetCtrlContainer();
			if (Ctrl != nullptr)
				Ctrl->SetFocusedControl(nullptr);
		}
	}
}

int SMessageBox::OnMouseEvent(unsigned int dwFlags, unsigned int wParam, int nX, int nY)
{
	if (m_bSelectEnable == 0)
		return 0;

	m_bOver = PointInRect(nX, nY, m_nPosX, m_nPosY, m_nWidth, m_nHeight);

	if (dwFlags != WM_LBUTTONDOWN || m_bOver != 1)
		return SPanel::OnMouseEvent(dwFlags, wParam, nX, nY);

	if (g_pCurrentScene == nullptr)
		return 1;

	TMScene* pScene = g_pCurrentScene;
	pScene->GetCtrlContainer()->SetFocusedControl(this);

	return SPanel::OnMouseEvent(0x201, wParam, nX, nY);
}

SMessagePanel::SMessagePanel(const char* istrMessage, float inX, float inY, float inWidth, float inHeight, unsigned int dwTime)
	: SPanel(-45, inX, inY, inWidth, inHeight, 0xAAFFFFFF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH)
{
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_MESSAGEPANEL;
	m_GCPanel.nTextureSetIndex = -789;
	m_GCPanel.dwColor = 0xAA000000;
	m_dwControlID = 5638;

	m_pText = new SText(-2, istrMessage, 0xFFFFFFFF, 30.0f, 1.0f, (float)(inWidth - 2.0f) - 50.0f, inHeight - 2.0f, 1, 0, 1, 1);
	m_pText2 = new SText(-2, "", 0xFFFFFFFF, 50.0f, 24.0f, (float)(inWidth - 2.0) - 50.0f, 14.0f, 1, 0, 1, 1);
	m_pPanelL = new SPanel(-2, -11.0f, 0, 0, inHeight, 0xAA000000, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
	m_pPanelR = new SPanel(-2, inWidth, 0, 0, inHeight, 0xAA000000, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

	if (m_pPanelL != nullptr)
		AddChild(m_pPanelL);
	if (m_pPanelR != nullptr)
		AddChild(m_pPanelR);
	if (m_pText != nullptr)
		AddChild(m_pText);

	m_dwLifeTime = dwTime;
	m_dwOldServerTime = g_pTimerManager->GetServerTime();
}

SMessagePanel::~SMessagePanel()
{
	SAFE_DELETE(m_pText2);
}

void SMessagePanel::SetMessage(const char* istrMessage, unsigned int dwTime)
{
	m_dwLifeTime = dwTime;
	m_dwOldServerTime = g_pTimerManager->GetServerTime();
	m_pText->SetText((char*)istrMessage, 0);
}

void SMessagePanel::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	unsigned int dwServerTime = g_pTimerManager->GetServerTime();
	if (m_dwLifeTime && dwServerTime - m_dwOldServerTime > m_dwLifeTime)
		SetVisible(0, 1);

	SPanel::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);
}

void SMessagePanel::SetVisible(int bVisible, int bSound)
{
	SPanel::SetVisible(bVisible);

	auto soundManager = g_pSoundManager;
	if (bVisible == 1 && bSound == 1 && soundManager != nullptr)
	{
		auto soundData = soundManager->GetSoundData(33);
		if (soundData)
			soundData->Play();
	}
}

SReelPanel::SReelPanel(unsigned int inTextureSetIndex, float inX, float inY, float inSizeX, float inSizeY, float inPitch)
	: SPanel(-2, inX, inY, 186.0f, 180.0f, 0x1010101, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH)
{
}

SReelPanel::~SReelPanel()
{
}

void SReelPanel::SetRoll(bool bRoll, int StopPos1, int StopPos2, int StopPos3, unsigned int StopTime)
{
}

void SReelPanel::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
}

void SReelPanel::SetVisible(int bVisible)
{
}

void SReelPanel::SetResult(char cResult)
{
}

void SReelPanel::UpDateJackpot()
{
}

int PointInRect(int inPosX, int inPosY, float ifX, float ifY, float ifWidth, float ifHeight)
{
	return (float)inPosX >= ifX
		&& (float)inPosY >= ifY
		&& (float)(ifX + ifWidth) > (float)inPosX
		&& (float)(ifY + ifHeight) > (float)inPosY;
}

void RemoveRenderControlItem(stGeomList* pDrawList, GeomControl* pGeomControl, int nLayer)
{
	if (nLayer >= MAX_DRAW_CONTROL)
		return;

	GeomControl* pCurrent = pDrawList[nLayer].pHeadGeom;

	if (pCurrent == nullptr)
		return;

	if (pCurrent == pGeomControl)
	{
		pDrawList[nLayer].pHeadGeom = pCurrent->m_pNextGeom;
		return;
	}

	int nCount{ 0 };
	while (pCurrent != nullptr && pCurrent->m_pNextGeom != nullptr)
	{
		if (pCurrent->m_pNextGeom == pGeomControl)
		{
			pCurrent->m_pNextGeom = pCurrent->m_pNextGeom->m_pNextGeom;
			return;
		}

		pCurrent = pCurrent->m_pNextGeom;
		if (++nCount > g_pDebugMaxCount)
			g_pDebugMaxCount = nCount;

		if (nCount > MAX_DRAW_CONTROL)
			return;
	}
}

int AddRenderControlItem(stGeomList* pDrawList, GeomControl* pGeomControl, int nLayer)
{
	if (nLayer >= MAX_DRAW_CONTROL)
		return 0;

	if (pGeomControl)
		pGeomControl->nLayer = nLayer;

	if (pDrawList[nLayer].pHeadGeom)
		pDrawList[nLayer].pTailGeom->m_pNextGeom = pGeomControl;
	else
		pDrawList[nLayer].pHeadGeom = pGeomControl;

	pDrawList[nLayer].pTailGeom = pGeomControl;
	return 1;
}


