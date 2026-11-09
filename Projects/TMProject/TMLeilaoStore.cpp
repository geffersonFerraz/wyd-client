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
#include <WinInet.h>

int TMFieldScene::m_bAutotrade = 0;
int TMFieldScene::mHideLeilao = 0;
int TMFieldScene::m_bCurrentSlotLeilao = 0;
int TMFieldScene::m_bCurrentTipeLeilao = -1;
int TMFieldScene::m_nCurrentSlobankLeilao;
int TMFieldScene::m_nLeilaoStoreCurrentPage = 0; // Página atual
int TMFieldScene::m_nLeilaoStoreTotalPages = 1;  // Total de páginas
 
std::string SControl::FormatPriceInKKK(long long price) {
	std::string result;

	if (price >= 1000000000) { // Bilhões (1KKK ou mais)
		result = std::to_string(price / 1000000000) + "KKK";
	}
	else if (price >= 1000000) { // Milhões (1KK até 999KK)
		result = std::to_string(price / 1000000) + "KK";
	}
	else if (price >= 1000) { // Milhares (1K até 999K)
		result = std::to_string(price / 1000) + "K";
	}
	else {
		result = std::to_string(price); // Menos de 1000
	}

	return result;
}


int TMFieldScene::InitializeLeilaoStore()
{
	m_pLojinhaTimeText = new SText(-2,
		"00 : 00",
		0xFF00FF00,
		(((float)g_pDevice->m_dwScreenWidth / RenderDevice::m_fWidthRatio) / 2.0f) - 42.0f,
		30.0f,
		200.0f,
		16.0f,
		0,
		0x77777777,
		1,
		0);

	if (m_pLojinhaTimeText)
	{
		m_pLojinhaTimeText->m_Font.m_fSize = 1.0f;
		m_pLojinhaTimeText->SetVisible(0);
		m_pControlContainer->AddItem(m_pLojinhaTimeText);
	}
	m_pBlocoX48 = new CapeLeilaoStore("", 160.0f, 80.0f, 560.0f * RenderDevice::m_fWidthRatio, 420.0f * RenderDevice::m_fHeightRatio);
	if (m_pBlocoX48)
	{
		m_pBlocoX48->m_GCPanel.nTextureSetIndex = 759;
		m_pBlocoX48->m_bSelectEnable = 0;
		m_pBlocoX48->SetControlID(56389901);

		m_pBlocoX48->SetVisible(0, 0);
		m_pControlContainer->AddItem(static_cast<SControl*>(m_pBlocoX48));

	}

	m_pLeilaoStore = (SPanel*)m_pControlContainer->FindControl(6000011);
	m_pLeilaoStore->m_bVisible = 0;
	m_pGridLeilaoStore = (SGridControl*)m_pControlContainer->FindControl(6200011);

	auto gGridSlotValuePane = (SGridControl*)m_pControlContainer->FindControl(565225);

	if (m_pGridLeilaoStore)
	{
		m_pGridLeilaoStore->m_bDrawGrid = 1;
		m_pGridLeilaoStore->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
		m_pGridLeilaoStore->Empty();
	}
	if (gGridSlotValuePane)
	{
		gGridSlotValuePane->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
		gGridSlotValuePane->Empty();
	}

	m_pAutoTradeValuePanel = (SPanel*)m_pControlContainer->FindControl(565885);
	if (m_pAutoTradeValuePanel)
	{
		m_pAutoTradeValuePanel->SetVisible(0);
		m_pAutoTradeValuePanel->SetPos(((float)g_pDevice->m_dwScreenWidth * 0.5f) - (m_pAutoTradeValuePanel->m_nWidth * 0.8f),
			((float)g_pDevice->m_dwScreenHeight * 0.5f) - (m_pAutoTradeValuePanel->m_nHeight * 1.5f));
	}

	return 1;
}

int TMFieldScene::LeilaoONEsc(int bvisible)
{


	return 1;
}  

int TMFieldScene::LeilaoButtonAction(int idwControlID)
{
	 
	if (m_pLeilaoStore && (!m_pLeilaoStore->m_bVisible) && (idwControlID != 6000063u))
		return 0;

	auto LabelTituloPanel = static_cast<SText*>(m_pControlContainer->FindControl(6810069));
	 
	if (idwControlID == 6000060u) // categoria donate
	{
		LabelTituloPanel->SetText((char*)"Negociados por Donate", 0);
		for (size_t i = 0; i < 48; i++)
		{
			m_pBlocoX48->SetReset(i);
		}
		m_pGridLeilaoStore->Empty();
		m_pGridLeilaoStore->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
		m_bCurrentTipeLeilao = 1;
		m_nLeilaoStoreCurrentPage = 1;
		AltLeilaoAction(0);
		return SendLeilaoRequest(m_bCurrentTipeLeilao, 0);
	}
	if (idwControlID == 6000061)// categoria gold
	{
		LabelTituloPanel->SetText((char*)"Negociados por Gold", 0);
		for (size_t i = 0; i < 48; i++)
		{
			m_pBlocoX48->SetReset(i);
		}
		m_pGridLeilaoStore->Empty();
		m_pGridLeilaoStore->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
		m_bCurrentTipeLeilao = 2;
		m_nLeilaoStoreCurrentPage = 1;
		AltLeilaoAction(0);
		return SendLeilaoRequest(m_bCurrentTipeLeilao, 0);
	}
	if (idwControlID == 6000062u)// categoria barras
	{
		LabelTituloPanel->SetText((char*)"Negociados por Barras(1b)", 0);
		for (size_t i = 0; i < 48; i++)
		{
			m_pBlocoX48->SetReset(i);
		}
		m_pGridLeilaoStore->Empty();
		m_pGridLeilaoStore->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
		m_bCurrentTipeLeilao = 3;
		AltLeilaoAction(0);
		return SendLeilaoRequest(m_bCurrentTipeLeilao, 0);
	}
	if (idwControlID == 6000063u)// categoria todos itens
	{
		LabelTituloPanel->SetText((char*)"Itens em negociação", 0);
		for (size_t i = 0; i < 48; i++)
		{
			m_pBlocoX48->SetReset(i);
		}
		m_pGridLeilaoStore->Empty();
		m_pGridLeilaoStore->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
		m_bCurrentTipeLeilao = 0;
		m_nLeilaoStoreCurrentPage = 1;
		AltLeilaoAction(0);
		return SendLeilaoRequest(m_bCurrentTipeLeilao, 0);
	}
	if (idwControlID == 6000064u)// categoria meus itens
	{
		LabelTituloPanel->SetText((char*)"Clique sobre o item para retira-lo do leilão", 0);
		for (size_t i = 0; i < 48; i++)
		{
			m_pBlocoX48->SetReset(i);
		}
		m_pGridLeilaoStore->Empty();
		m_pGridLeilaoStore->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
		m_bCurrentTipeLeilao = 0;
		m_nLeilaoStoreCurrentPage = 1;
		m_nLeilaoStoreTotalPages = 1;

		AltLeilaoAction(-1);
		return SendLeilaoRequest(-1, 1);
	}
	if (idwControlID == 6002212u)//get edit busca leilao
	{

		auto BuscaleilaoStore = static_cast<SEditableText*>(m_pControlContainer->FindControl(6002213));

		if (BuscaleilaoStore)
		{
			m_pControlContainer->SetFocusedControl(BuscaleilaoStore);
		}
		return 1;
	}
	if (idwControlID == 6002213)
	{

		SendSearchLeilaoRequest(0, (char*)"");
		m_dwDelayBusca = g_pApp->m_pTimerManager->GetServerTime();

		m_pControlContainer->SetFocusedControl(0);
		return 1;
	}
	if (idwControlID == 65794)
	{
		if (m_pLeilaoStore)
		{
			OnControlEvent(6000063u, 0);
		}
		return 1;
	}
	if (idwControlID == 6000163) //close leilao store
	{
		SetVisibleLeilaoAction(0, 0);
		return 0;
	}
	if (idwControlID == 6002211u) // 65794u
	{
		if (!m_pMyHuman->IsInTown())
		{
			m_pMessagePanel->SetMessage((char*)"Somente nas cidades.", 10000);
			m_pMessagePanel->SetVisible(1, 1);
			return 1;
		}

		m_pGridLeilaoStore->Empty();

		if (m_bAutotrade)
		{
			MSG_MessageWhisper stWhisper{};

			stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
			stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

			sprintf_s(stWhisper.MobName, "fecharloja");

			SendOneMessage((char*)&stWhisper, sizeof(stWhisper));
			for (size_t i = 0; i < 48; i++)
			{
				m_pBlocoX48->SetReset(i);
			}
			return 1;
		}

		AltLeilaoAction(-1);
		VisibleInputTradeName();
		return 1;
	}
	if (idwControlID == 665811)
	{
		if (!m_pMyHuman->IsInTown())
			return 1;

		m_pGridLeilaoStore->Empty();

		MSG_MessageWhisper stWhisper{};

		stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
		stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

		sprintf_s(stWhisper.MobName, "hideleilao");

		SendOneMessage((char*)&stWhisper, sizeof(stWhisper));

		UpdateElementesLeilaoStore();
		return 1;
	}

	if (idwControlID >= 565811 && idwControlID <= 565813)
	{
		m_bCurrentTipeLeilao = (idwControlID - 565811) + 1;

		auto Generico1 = static_cast<SButton*>(m_pControlContainer->FindControl(565811));
		auto Generico2 = static_cast<SButton*>(m_pControlContainer->FindControl(565812));
		auto Generico3 = static_cast<SButton*>(m_pControlContainer->FindControl(565813));

		// OFF 280
		// ON 246 

		std::string message;
		switch (m_bCurrentTipeLeilao)
		{
		case 1:
		{
			if (Generico1)
			{
				Generico1->m_GCPanel.nTextureSetIndex = 246;
			}
			if (Generico2)
			{
				Generico2->m_GCPanel.nTextureSetIndex = 280;
			}
			if (Generico3)
			{
				Generico3->m_GCPanel.nTextureSetIndex = 280;
			}
			message = "Item agora disponível para compra com donate (cash)!";
			break;
		}
		case 2:
		{
			if (Generico1)
			{
				Generico1->m_GCPanel.nTextureSetIndex = 280;
			}
			if (Generico2)
			{
				Generico2->m_GCPanel.nTextureSetIndex = 246;
			}
			if (Generico3)
			{
				Generico3->m_GCPanel.nTextureSetIndex = 280;
			}
			message = "Item agora disponível para compra com ouro!";
			break;
		}
		case 3:
		{
			if (Generico1)
			{
				Generico1->m_GCPanel.nTextureSetIndex = 280;
			}
			if (Generico2)
			{
				Generico2->m_GCPanel.nTextureSetIndex = 280;
			}
			if (Generico3)
			{
				Generico3->m_GCPanel.nTextureSetIndex = 246;
			}
			message = "Item agora disponível para compra com barras de ouro!";
			break;
		}
		default:
			break;
		}

		if (!message.empty())
		{
			m_pMessagePanel->SetMessage(message.c_str(), 10000);
			m_pMessagePanel->SetVisible(1, 1);
		}

		m_stAutoTrade.List[m_bCurrentSlotLeilao].Categoria = m_bCurrentTipeLeilao;
		return 1;
	}
	if (idwControlID == 6010061)//idwControlID do Botão ID
	{
		LeilaoStoreNextPage();
		return 1;
	}
	if (idwControlID == 6010060)//idwControlID do Botão ID
	{
		LeilaoStorePreviousPage();
		return 1;
	}
	if (idwControlID == 565887)
	{
		m_pAutoTradeValuePanel->m_bVisible = 0;
		return 1;
	}
	if (idwControlID == 565886)
	{
		auto pInputText = static_cast<SEditableText*>(m_pControlContainer->FindControl(565889));

		char* inputText = pInputText->GetText();

		int inputTextLen = strlen(inputText);
		 
		if (m_bCurrentTipeLeilao == -1)
		{

			m_pMessagePanel->SetMessage((char*)"Selecione qual tipo de venda para o item", 5000);
			m_pMessagePanel->SetVisible(1, 1);
			return 1;
		}

		if (strcmp(inputText, "all"))
		{
			bool bFind = false;

			for (int n = 0; n < inputTextLen; ++n)
			{
				if (inputText[n] < '0' || inputText[n] > '9')
				{
					bFind = true;
					break;
				}
			}

			for (int n = 0; n < inputTextLen; ++n)
			{
				if (inputText[n] == '%')
					inputText[n] = '!';
			}

			long long nInputValue = _atoi64(pInputText->GetText());

			if (strlen(pInputText->GetText()) <= 0)
			{
				m_pControlContainer->SetFocusedControl(pInputText);
				return 1;
			}


			if (m_nCoinMsgType != 12 &&
				m_nCoinMsgType != 11 &&
				m_nCoinMsgType != 8 &&
				m_nCoinMsgType != 3 &&
				m_nCoinMsgType != 6)
			{
				if (bFind == true || nInputValue < 0 || nInputValue > g_pObjectManager->m_stMobData.Coin && !m_nCoinMsgType)
				{
					m_pMessagePanel->SetMessage(g_pMessageStringTable[34], 1000);
					m_pMessagePanel->SetVisible(1, 1);
					m_pControlContainer->SetFocusedControl(pInputText);
					m_pChatSelectPanel->SetVisible(0);
					return 1;
				}
			}

			if (bFind == true && m_nCoinMsgType == 12)
			{
				m_pMessagePanel->SetMessage(g_pMessageStringTable[409], 1000);
				m_pMessagePanel->SetVisible(1, 1);
				m_pControlContainer->SetFocusedControl(pInputText);
				return 1;
			}

			if (m_nCoinMsgType == 4 && nInputValue > 1000000000000000LL)
			{
				char istrMessage[128]{};

				sprintf_s(istrMessage, "Limite para definição de valores é de até %lld", 1000000000000000LL);

				m_pMessagePanel->SetMessage(istrMessage, 1000);
				m_pMessagePanel->SetVisible(1, 1);

				m_pControlContainer->SetFocusedControl(pInputText);
				return 1;
			}
			if (m_nCurrentSlobankLeilao >= 0 && m_bCurrentSlotLeilao >= 0)
			{

				auto pGridLeilaoStore = m_pGridLeilaoStore;

				if (!pGridLeilaoStore) {
					 
					return 1;
				}

				SGridControl* pParent = pGridLeilaoStore;
				SGridControlItem* pAutoTradeItem = pParent->GetAtItem(0, 0);

				SGridControl* pCargoGrid = m_pCargoGridList[m_nCurrentSlobankLeilao / 40];
				SGridControlItem* pCargoItem = pCargoGrid->GetAtItem(m_nCurrentSlobankLeilao % 40 % 5, m_nCurrentSlobankLeilao % 40 / 5);
				 
				pAddItemAutoTrade SendPacket = {};
				if (!pCargoItem) {
					std::cout << "Item no Cargo não encontrado!" << std::endl;
					return 1;
				}

				auto pItem = new STRUCT_ITEM;

				memcpy(pItem, pCargoItem->m_pItem, sizeof(STRUCT_ITEM));

				pCargoItem->m_GCObj.dwColor = 0xFFFF0000;

				SendPacket.Header.ID = g_pObjectManager->m_dwCharID;
				SendPacket.Header.Type = 0x397C;

				SendPacket.SlotBank = m_nCurrentSlobankLeilao;
				SendPacket.itemPrice = nInputValue;
				SendPacket.itemType = static_cast<int>(m_bCurrentTipeLeilao);

				memcpy(&SendPacket.Name, m_stAutoTrade.List[m_bCurrentSlotLeilao].LojinhaName, sizeof(m_stAutoTrade.List[m_bCurrentSlotLeilao].LojinhaName));

				memcpy(&SendPacket.Item, pItem, sizeof(STRUCT_ITEM));

				pParent->m_nTradeMoney = nInputValue;
				SendOneMessage((char*)&SendPacket, sizeof(pAddItemAutoTrade));
				m_nCurrentSlobankLeilao = -1;
				m_stAutoTrade.List[m_bCurrentSlotLeilao].Categoria = -1;
			}


			m_pControlContainer->SetFocusedControl(0);
			m_pAutoTradeValuePanel->SetVisible(0);

			pInputText->SetText((char*)"");

			if (g_nKeyType == 1)
				m_pControlContainer->SetFocusedControl(m_pEditChat);
		}
		return 1;
	}
	/*BOTOES DO SISTEMA DE LEILÃO*/
}
//ok
int TMFieldScene::OnKeyVisibleLeilao(char iCharCode, int lParam)
{
	if (iCharCode != 'b' && iCharCode != 'B')
		return 0;

	if (m_pLeilaoStore)
	{
		OnControlEvent(6000063u, 0);
	}

	return 1;
}

//ok
void TMFieldScene::LeilaoStoreNextPage() {

	if (m_nLeilaoStoreCurrentPage < m_nLeilaoStoreTotalPages)
	{
		m_nLeilaoStoreCurrentPage++;
	}

	if (m_pInvenPanel->m_bVisible)
	{
		SendLeilaoRequest(m_bCurrentTipeLeilao, 0);
	}
	else
	{
		SendLeilaoRequest(m_bCurrentTipeLeilao, 1);
	}
}

//ok
void TMFieldScene::LeilaoStorePreviousPage() {

	m_nLeilaoStoreCurrentPage--;
	if (m_nLeilaoStoreCurrentPage <= -1)
		m_nLeilaoStoreCurrentPage = 0;
	if (m_pInvenPanel->m_bVisible)
	{
		SendLeilaoRequest(m_bCurrentTipeLeilao, 0);
	}
	else
	{
		SendLeilaoRequest(m_bCurrentTipeLeilao, 1);
	}
}

//ok
int TMFieldScene::SendLeilaoOp(int itemType, int alvo)
{

	char szMessage[128]{};
	sprintf(szMessage, "Acesso confirmado com sucesso. Anúncios da loja %s", m_stAutoTrade.List[m_bCurrentSlotLeilao].LojinhaName);


	m_pMessagePanel->SetMessage(szMessage, 5000);
	m_pMessagePanel->SetVisible(1, 1);

	MSG_STANDARDPARM2 stParm2{};
	stParm2.Header.Type = 0xAF2;
	stParm2.Header.ID = m_pMyHuman->m_dwID;
	stParm2.Parm1 = alvo;
	stParm2.Parm2 = itemType;

	SendOneMessage((char*)&stParm2, sizeof(stParm2));
	m_pPGTPanel->SetVisible(0);
	m_pPGTOver = 0;
	return 1;
}

//ok
int TMFieldScene::SendLeilaoDelItem(int SlotLeilaoStore, int alvo)
{
	m_pGridLeilaoStore->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
	MSG_STANDARDPARM2 stParm2{};
	stParm2.Header.Type = 0xBAF2;
	stParm2.Header.ID = m_pMyHuman->m_dwID;
	stParm2.Parm1 = alvo;
	stParm2.Parm2 = SlotLeilaoStore;
	SendOneMessage((char*)&stParm2, sizeof(stParm2));
	m_pPGTPanel->SetVisible(0);
	m_pPGTOver = 0;
	return 1;
}
int TMFieldScene::SendSearchLeilaoRequest(int itemType, char* ItemName)
{
	STRUCT_BUSCA req{};
	req.Header.ID = g_pObjectManager->m_dwCharID;
	req.Header.Type = 0xAFC;
	req.itemType = itemType;
	req.currentpage = m_nLeilaoStoreCurrentPage;
	req.totalpage = m_nLeilaoStoreTotalPages;

	if (strlen(ItemName) < sizeof(req.Name)) {
		strcpy(req.Name, ItemName);
	}
	else {
		// Trate o caso de erro aqui, por exemplo, truncando ou retornando uma mensagem de erro.
		strncpy(req.Name, ItemName, sizeof(req.Name) - 1);
		req.Name[sizeof(req.Name) - 1] = '\0'; // Garante que o último caractere é nulo.
	}
	 
	SendOneMessage((char*)&req, sizeof(req));
	return 1;
} 
//ok
int TMFieldScene::SendLeilaoRequest(int itemType, int action)
{
	// m_pGridLeilaoStore->Empty();
	MSG_GetLeilaoStores req{};
	req.Header.ID = g_pObjectManager->m_dwCharID;
	req.Header.Type = 0xAF1;
	req.itemType = itemType;
	req.currentpage = m_nLeilaoStoreCurrentPage;
	req.totalpage = m_nLeilaoStoreTotalPages;
	SendOneMessage((char*)&req, sizeof(req));
	return 1;
}
//ok
void TMFieldScene::UpdateElementesLeilaoStore() {

	if (m_pLeilaoStore->m_bVisible == 0)
		return;

	int slotBank = 0;

	unsigned int enableItem = 0xFFFFFFFF;//inicia defalut 
	 










#pragma region Labels
	auto LabelCurrentPage = static_cast<SText*>(m_pControlContainer->FindControl(6010069));

	char strSexy[56]{};

	auto currentpage = m_nLeilaoStoreCurrentPage;

	if (currentpage <= 0)
	{
		currentpage = 1;
	}

	if (LabelCurrentPage)
	{
		snprintf(strSexy, sizeof(strSexy), "%d/%d", currentpage, m_nLeilaoStoreTotalPages);
		LabelCurrentPage->SetText((char*)strSexy, 0);
	}

	auto LabelHideLeilao = static_cast<SText*>(m_pControlContainer->FindControl(665810));

	if (LabelHideLeilao)
	{
		auto BtnHideLeilao = static_cast<SButton*>(m_pControlContainer->FindControl(665811));

		if (mHideLeilao && BtnHideLeilao)
		{
			BtnHideLeilao->m_GCPanel.nTextureSetIndex = 280;//280
			LabelHideLeilao->SetText((char*)"Não quero ver meus Anúncios", 0);
		}
		else
		{
			BtnHideLeilao->m_GCPanel.nTextureSetIndex = 246;//246
			LabelHideLeilao->SetText((char*)"Quero ver meus Anúncios", 0);
		}
	}

#pragma endregion 

#pragma region BTNS


	auto oBtnAbrirLoja = (SButton*)m_pControlContainer->FindControl(6002211);

	if (m_bAutotrade)
	{
		oBtnAbrirLoja->SetText((char*)"Fechar Lojinha");
	}
	else
	{
		oBtnAbrirLoja->SetText((char*)"Abrir Lojinha");
	}

#pragma endregion

#pragma region Slots Grid Status and Label price

	for (int nCargoIndex = 0; nCargoIndex < 160; ++nCargoIndex)
	{
		// Verifica se o item no índice atual é válido
		if (g_pObjectManager->m_stItemCargo[nCargoIndex].sIndex)
		{
			int Page = nCargoIndex / 40; // Determina a página do item
			if (Page < 4) // Verifica se a página está dentro do intervalo válido
			{
				// Calcula as coordenadas x e y na página
				int mod40 = nCargoIndex % 40;
				int x = mod40 % 5;
				int y = mod40 / 5;

				// Obtém o item da grade
				auto bb = m_pCargoGridList[Page]->GetItem(x, y);

				// Verifica se o item é válido
				if (!bb)
					continue; // Pular itens que não são válidos

				// Atualiza a cor do item para vermelho se o item já está no leilão
				bb->m_GCObj.dwColor = 0xFFFFFFFF;

			}
		}
	}

	for (int i = 0; i < MAX_SLOT_LEILAO_AUTOTRADE; ++i)
	{
		if (m_stAutoTrade.List[i].OwnerId <= 0)
			continue;

		if (m_stAutoTrade.List[i].Item.sIndex <= 0)
			continue;

		//pGridLeilaoStore
		int mod50 = i;
		int x2 = mod50 % 8;
		int y2 = mod50 / 8;

		auto bb2 = m_pGridLeilaoStore->GetItem(x2, y2);
		if (!bb2)
			continue;
		 

		if (m_pMyHuman->m_dwID == m_stAutoTrade.List[i].OwnerId)
		{
			if (m_stAutoTrade.List[i].Item.sIndex == m_stAutoTrade.List[i].Item.sIndex)
			{
				enableItem = 0xFFFF0000;
			}
			else
			{
				enableItem = 0xFFFFFFFF;
			}
		}
		else
		{
			enableItem = 0xFFFFFFFF;
		}

		bb2->m_GCObj.dwColor = enableItem;


		if (g_pObjectManager->m_stItemCargo[m_stAutoTrade.List[i].SlotBank].sIndex)
		{
			if (m_pMyHuman->m_dwID != m_stAutoTrade.List[i].OwnerId &&
				g_pObjectManager->m_stItemCargo[m_stAutoTrade.List[i].SlotBank].sIndex != m_stAutoTrade.List[i].Item.sIndex)
			{
				enableItem = 0xFFFFFFFF;
			}
			if (m_pMyHuman->m_dwID == m_stAutoTrade.List[i].OwnerId &&
				g_pObjectManager->m_stItemCargo[m_stAutoTrade.List[i].SlotBank].sIndex == m_stAutoTrade.List[i].Item.sIndex)
			{
				enableItem = 0xFFFF0000;
			}

			int Page = m_stAutoTrade.List[i].SlotBank / 40; // Determina a página do item
			if (Page < 4) // Verifica se a página está dentro do intervalo válido
			{
				int mod40 = m_stAutoTrade.List[i].SlotBank % 40;
				int x = mod40 % 5;
				int y = mod40 / 5;

				auto bb = this->m_pCargoGridList[Page]->GetItem(x, y);
				if (!bb)
					continue;

				bb->m_GCObj.dwColor = enableItem;
			}
		}


		char szText[128] = "";
		if (m_stAutoTrade.List[i].Categoria == 1)
		{
			sprintf(szText, "%d Donate", m_stAutoTrade.List[i].Price);
		}
		else if (m_stAutoTrade.List[i].Categoria == 2)
		{
			sprintf(szText, "%lld Gold", m_stAutoTrade.List[i].Price);
		}
		else if (m_stAutoTrade.List[i].Categoria == 3)
		{
			sprintf(szText, "%d Bis", m_stAutoTrade.List[i].Price);
		}
	}
#pragma endregion


}
//ok
void TMFieldScene::AltLeilaoAction(int action)
{



	if (action == -1)
	{
		SetVisibleLeilaoAction(1, 1);
		for (int nCargoIndex = 0; nCargoIndex < 160; ++nCargoIndex)
		{

			if (g_pObjectManager->m_stItemCargo[nCargoIndex].sIndex)
			{
				int Page = nCargoIndex / 40;
				if (Page < 4)
				{

					int mod40 = nCargoIndex % 40;
					int x = mod40 % 5;
					int y = mod40 / 5;


					auto bb = m_pCargoGridList[Page]->GetItem(x, y);


					if (!bb)
						continue;

					if (bb->m_GCObj.dwColor != 0xFFFF0000)
						continue;

					bb->m_GCObj.dwColor = 0xFFFFFFFF;

				}
			}
		}
		m_pCargoPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);
		m_pCargoPanel1->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);

	}
	else
	{
		SetVisibleLeilaoAction(1, 0);
	}
}

//ok 
// 
void TMFieldScene::SetVisibleLeilaoAction(int bShow, int bCargo)
{
	SGridControl::m_sLastMouseOverIndex = -1;
	m_pBlocoX48->SetVisible(bShow, 1);



	if (!bShow)
	{
		for (size_t i = 0; i < 48; i++)
		{
			m_pBlocoX48->SetReset(i);
		}
	}
	if (m_pLeilaoStore)
	{
		auto pMyCargoCoin = static_cast<SText*>(m_pControlContainer->FindControl(TMT_ATRADE_COIN));
		auto pMyCargoCoinB = static_cast<SButton*>(m_pControlContainer->FindControl(TMB_ATRADE_COIN));



		auto pGridLeilaoStore = m_pGridLeilaoStore;

		if (!pGridLeilaoStore)
			return;

		m_pLeilaoStore->SetVisible(bShow);
		m_pBlocoX48->m_nPosX = m_pLeilaoStore->m_nPosX;
		m_pBlocoX48->m_nPosY = m_pLeilaoStore->m_nPosY;
		m_pBlocoX48->m_nWidth = m_pLeilaoStore->m_nWidth;
		m_pBlocoX48->m_nHeight = m_pLeilaoStore->m_nHeight;
		 
		m_dwDelayBusca = g_pApp->m_pTimerManager->GetServerTime();

		if (bShow)
		{

			if (m_pChatPanel)
			{
				m_pChatPanel->m_bVisible = 0;
				m_pChatList->m_bVisible = 0;
				m_pChatListnotice->m_bVisible = 0;
				m_pChatBack->m_bVisible = 0;
				m_pChatGeneral->m_bVisible = 0;
				m_pChatParty->m_bVisible = 0;
				m_pChatWhisper->m_bVisible = 0;
				m_pChatGuild->m_bVisible = 0;
				m_pChatGeneral_C->m_bVisible = 0;
				m_pChatParty_C->m_bVisible = 0;
				m_pChatWhisper_C->m_bVisible = 0;
				m_pChatGuild_C->m_bVisible = 0;
				m_pKingdomText->m_bVisible = 0;
				m_pKingDomGuild->m_bVisible = 0;
				m_pKingDomGuild_C->m_bVisible = 0;
			}
		}
		else
		{
			if (m_pChatPanel)
			{
				m_pChatPanel->m_bVisible = 1;
				m_pChatList->m_bVisible = 1;
				m_pChatListnotice->m_bVisible = 1;
				m_pChatBack->m_bVisible = 1;
				m_pChatGeneral->m_bVisible = 1;
				m_pChatParty->m_bVisible = 1;
				m_pChatWhisper->m_bVisible = 1;
				m_pChatGuild->m_bVisible = 1;
				m_pChatGeneral_C->m_bVisible = 1;
				m_pChatParty_C->m_bVisible = 1;
				m_pChatWhisper_C->m_bVisible = 1;
				m_pChatGuild_C->m_bVisible = 1;
				m_pKingdomText->m_bVisible = 1;
				m_pKingDomGuild->m_bVisible = 1;
				m_pKingDomGuild_C->m_bVisible = 1;
			}

		}
		if (bShow == 1)
		{


			g_pCursor->DetachItem();
			m_pSystemPanel->SetVisible(0);
			m_pCPanel->SetVisible(0);
			m_pSkillPanel->SetVisible(0);
			m_pSkillMPanel->SetVisible(0);



			if (bCargo == 1)
			{
				m_pCargoPanel->SetVisible(1);
				m_pCargoPanel1->SetVisible(1);
				m_pInvenPanel->SetVisible(0);
				m_pShopPanel->SetVisible(0);

				m_pInvenPanel->SetVisible(0);
				m_pInvenPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);
				m_pInvenPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);
				m_pCargoPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);
				m_pCargoPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);

				if (pMyCargoCoin)
					pMyCargoCoin->SetVisible(1);

				if (pMyCargoCoinB)
					pMyCargoCoinB->SetVisible(1);
			}
			else
			{
				m_pCargoPanel->SetVisible(0);
				m_pCargoPanel1->SetVisible(0);

				if (pMyCargoCoin)
					pMyCargoCoin->SetVisible(0);

				if (pMyCargoCoinB)
					pMyCargoCoinB->SetVisible(0);

				m_pGridInvList[0]->m_eGridType = TMEGRIDTYPE::GRID_TRADEINV2;
				m_pGridInvList[1]->m_eGridType = TMEGRIDTYPE::GRID_TRADEINV2;
				m_pGridInvList[2]->m_eGridType = TMEGRIDTYPE::GRID_TRADEINV2;
				m_pGridInvList[3]->m_eGridType = TMEGRIDTYPE::GRID_TRADEINV2;
				m_pInvenPanel->SetVisible(1);

				m_pInvenPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);
				m_pInvenPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);
				m_pCargoPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);
				m_pCargoPanel->SetPos(RenderDevice::m_fWidthRatio * 575.0f, RenderDevice::m_fHeightRatio * 1.0f);

				SetEquipGridState(0);
			}
			m_pCargoGridList[0]->m_eGridType = TMEGRIDTYPE::GRID_TRADEINV2;
			m_pCargoGridList[1]->m_eGridType = TMEGRIDTYPE::GRID_TRADEINV2;
			m_pCargoGridList[2]->m_eGridType = TMEGRIDTYPE::GRID_TRADEINV2;
			m_pCargoGridList[3]->m_eGridType = TMEGRIDTYPE::GRID_TRADEINV2;

		}
		else
		{
			// OFF 280
			// ON 246 


			sprintf(m_pMyHuman->m_TradeDesc, "");
			m_pMyHuman->m_pAutoTradeDesc->SetText(m_pMyHuman->m_TradeDesc, 0);

			if (m_pInvenPanel->IsVisible() == 1)
			{
				m_pGridInvList[0]->m_eGridType = TMEGRIDTYPE::GRID_DEFAULT;
				m_pGridInvList[1]->m_eGridType = TMEGRIDTYPE::GRID_DEFAULT;
				m_pGridInvList[2]->m_eGridType = TMEGRIDTYPE::GRID_DEFAULT;
				m_pGridInvList[3]->m_eGridType = TMEGRIDTYPE::GRID_DEFAULT;
				SetEquipGridState(1);
			}
			m_pInvenPanel->SetVisible(0);
			m_pCargoPanel->SetVisible(0);
			m_pCargoPanel1->SetVisible(0);
			g_pDevice->m_nWidthShift = 0;
			m_pCargoGridList[0]->m_eGridType = TMEGRIDTYPE::GRID_CARGO;
			m_pCargoGridList[1]->m_eGridType = TMEGRIDTYPE::GRID_CARGO;
			m_pCargoGridList[2]->m_eGridType = TMEGRIDTYPE::GRID_CARGO;
			m_pCargoGridList[3]->m_eGridType = TMEGRIDTYPE::GRID_CARGO;

			m_pControlContainer->SetFocusedControl(0);
			memset(&m_stAutoTrade, 0, sizeof(m_stAutoTrade));
			SetGridState();
		}

		 
	}
}

//ok
void TMFieldScene::SendReqLeilaoBuy()//aqui compra o item
{

	m_pGridLeilaoStore->m_eGridType = TMEGRIDTYPE::GRID_LEILAOSTORE;
	pStartNegotiation sendPacket = {};

	// Preenchendo o pacote
	sendPacket.Header.ID = g_pObjectManager->m_dwCharID;
	sendPacket.Header.Type = MSG_ReqBuy_Opcode;
	sendPacket.targetPlayerId = m_stAutoTrade.List[m_bCurrentSlotLeilao].OwnerId;
	sendPacket.itemSlot = m_bCurrentSlotLeilao;
	sendPacket.itemPrice = static_cast<long long>(m_stAutoTrade.List[m_bCurrentSlotLeilao].Price);
	sendPacket.itemType = static_cast<int>(m_stAutoTrade.List[m_bCurrentSlotLeilao].Categoria);
	sendPacket.itemTax = static_cast<int>(m_stAutoTrade.List[m_bCurrentSlotLeilao].TaxRate);
	sendPacket.SlotBank = static_cast<int>(m_stAutoTrade.List[m_bCurrentSlotLeilao].SlotBank);
	memcpy(&sendPacket.item, &m_stAutoTrade.List[m_bCurrentSlotLeilao].Item, sizeof(STRUCT_ITEM));
	 
	// Definindo o tamanho do pacote
	sendPacket.Header.Size = sizeof(pStartNegotiation);

	printf("Pacote de início de negociação preparado.\n");
	SendOneMessage((char*)&sendPacket, sizeof(pStartNegotiation));
}

int TMFieldScene::OnReceivingLeilaoStores(MSG_STANDARD* pStd)
{
	int slotBank = 0;
	unsigned int enableItem = 0xFFFFFFFF; // Inicia default
	auto pGridLeilaoStore = m_pGridLeilaoStore;
	auto LabelCurrentPage = static_cast<SText*>(m_pControlContainer->FindControl(6010069));
	char strSexy[128]{};
	memset(&m_stAutoTrade, 0, sizeof(m_stAutoTrade));

	auto currentpage = m_nLeilaoStoreCurrentPage;
	if (currentpage <= 0)
	{
		currentpage = 1;
	}

	if (LabelCurrentPage)
	{
		snprintf(strSexy, sizeof(strSexy), "%d/%d", currentpage, m_nLeilaoStoreTotalPages);
		LabelCurrentPage->SetText(strSexy, 0);
	}
	auto pAutoTrade = reinterpret_cast<MSG_LeilaoStore*>(pStd);

	// Limpa o grid somente uma vez antes de adicionar todos os itens
	pGridLeilaoStore->Empty();

	for (int nCargoIndex = 0; nCargoIndex < 160; ++nCargoIndex)
	{
		if (g_pObjectManager->m_stItemCargo[nCargoIndex].sIndex)
		{
			int Page = nCargoIndex / 40;
			if (Page < 4)
			{
				int mod40 = nCargoIndex % 40;
				int x = mod40 % 5;
				int y = mod40 / 5;

				auto bb = m_pCargoGridList[Page]->GetItem(x, y);

				if (!bb)
					continue;

				if (bb->m_GCObj.dwColor != 0xFFFF0000)
					continue;

				bb->m_GCObj.dwColor = 0xFFFFFFFF;
			}
		}
	}
	// Primeiro, reseta todos os painéis
	for (int i = 0; i < 48; ++i) {
		m_pBlocoX48->SetReset(i); 
	}

	for (int i = 0; i < MAX_SLOT_LEILAO_AUTOTRADE; ++i)
	{
		// Verifica se o item é válido
		if (pAutoTrade->List[i].OwnerId <= 0)
		{
			continue;
		}
		 
		 
		if (pAutoTrade->List[i].OwnerId != g_pObjectManager->m_dwCharID)
		{ 
		 m_pBlocoX48->SetColorBgzin(i, 0x25FFFF00);
				 
		}
		else
		{
			m_pBlocoX48->SetColorBgzin(i, 0x55FF0000);

		}








		// Copia as informações do item recebido para a estrutura local
		memcpy(&m_stAutoTrade.List[i], &pAutoTrade->List[i], sizeof(LeilaoStoreAux));

		// Se o item não for válido (sIndex <= 0 indica que não há item no slot)
		if (pAutoTrade->List[i].Item.sIndex <= 0)
		{
			m_pBlocoX48->SetReset(i); // Reseta o painel associado a esse índice
			continue;
		}

		// Configura o grid para exibir os itens
		pGridLeilaoStore->m_nTradeMoney = pAutoTrade->List[i].Price;

		// Atualiza informações de paginação
		if (pAutoTrade->CurrentPage > 0)
			m_nLeilaoStoreCurrentPage = pAutoTrade->CurrentPage;

		if (pAutoTrade->TotalPages > 0)
			m_nLeilaoStoreTotalPages = pAutoTrade->TotalPages;

		// Adiciona o item ao grid na posição correta
		if (pAutoTrade->List[i].Item.sIndex > 0) {
			STRUCT_ITEM* pItemRecovery = new STRUCT_ITEM;
			memcpy(pItemRecovery, &m_stAutoTrade.List[i].Item, sizeof(STRUCT_ITEM));

			// Calcula a posição no grid
			auto nx = i % 8;
			auto ny = i / 8;

			// Adiciona o item ao grid
			pGridLeilaoStore->AddItem(new SGridControlItem(nullptr, pItemRecovery, 0.0f, 0.0f), nx, ny);

			// Verifica a categoria e define o índice da textura
			int category = pAutoTrade->List[i].Categoria;
			if (category == 1)
			{
				m_pBlocoX48->SetName(i, "Cash");
				m_pBlocoX48->SetSizePanels(i, 23.0f, 15.0f);
				m_pBlocoX48->SetTextureSetIndexForPanel(i, 763); // Donate
			}
			else if (category == 2)
			{

				m_pBlocoX48->SetName(i, "Gold");
				m_pBlocoX48->SetSizePanels(i, 23.0f, 23.0f);
				m_pBlocoX48->SetTextureSetIndexForPanel(i, 764); // Gold
			}
			else if (category == 3)
			{
				m_pBlocoX48->SetName(i, "Bi's");
				m_pBlocoX48->SetSizePanels(i, 23.0f, 23.0f);
				m_pBlocoX48->SetTextureSetIndexForPanel(i, 765); // Bis
			}
			long long nPrice = m_stAutoTrade.List[i].Price;

			int nTipy = m_stAutoTrade.List[i].Categoria;

			std::string formattedPrice = m_pBlocoX48->FormatPriceInKKK(nPrice);

			if (nTipy == 1) {
				sprintf(strSexy, "%s", formattedPrice.c_str());
			}
			else if (nTipy == 2) {
				sprintf(strSexy, "%s", formattedPrice.c_str());
			}
			else if (nTipy == 3) {
				sprintf(strSexy, "%s", formattedPrice.c_str());
			}
			m_pBlocoX48->SetPrice(i, strSexy);
			m_pBlocoX48->SetPriceColor(i, 0xFFFFFFA0);
			m_pBlocoX48->SetNameColor(i, 0xFFFFFFA0);

			m_pBlocoX48->SetVisibleForPanel(i, 1);
		}
	}

	UpdateElementesLeilaoStore();

	return 1;
}
int TMFieldScene::OnPacketUpdateSaldoLeilao(st_SaldoLeilao* pStd)
{
	auto Pack = (st_SaldoLeilao*)pStd;

	// Formatar Gold com vírgulas como separador de milhares
	std::string result = std::to_string(Pack->Gold);
	int insertPosition = result.length() - 3;
	while (insertPosition > 0)
	{
		result.insert(insertPosition, ",");
		insertPosition -= 3;
	}
	std::string formattedGold = result;

	// Atualizar o texto do controle de Gold
	auto InfoGoldLeilaoStore = (SText*)m_pControlContainer->FindControl(6300069);
	if (InfoGoldLeilaoStore)
	{
		char szPrice[128];
		snprintf(szPrice, sizeof(szPrice), "%s", formattedGold.c_str());
		InfoGoldLeilaoStore->SetText(szPrice, 0);
		InfoGoldLeilaoStore->SetTextColor(0xFFFFFFA0);
	}

	// Atualizar o texto do controle de Donate
	auto InfoCashLeilaoStore = (SText*)m_pControlContainer->FindControl(6000069);
	if (InfoCashLeilaoStore)
	{
		char strSexy[128];
		snprintf(strSexy, sizeof(strSexy), "%d", Pack->Donate);
		InfoCashLeilaoStore->SetText(strSexy, 0);
	}

	// Atualizar o texto do controle de Bis
	auto InfoBisLeilaoStore = (SText*)m_pControlContainer->FindControl(6400069);
	if (InfoBisLeilaoStore)
	{
		char strSexy[128];
		if (Pack->Bis > 0)
		{
			snprintf(strSexy, sizeof(strSexy), "%d", Pack->Bis);
		}
		else
		{
			snprintf(strSexy, sizeof(strSexy), "0");
		}
		InfoBisLeilaoStore->SetText(strSexy, 0);
	}

	return 0;
}


CapeLeilaoStore::CapeLeilaoStore(const char* istrMessage, float inX, float inY, float inWidth, float inHeight)
	: SPanel(-45, inX, inY, inWidth, inHeight, 0xAA0000FF, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH) // Painel principal azul
{
	m_eCtrlType = CONTROL_TYPE::CTRL_TYPE_MESSAGEPANEL;

	m_GCPanel.nTextureSetIndex = 759;
	m_dwControlID = 56389901;
	resindex = RenderDevice::m_fResBase;







	float posX = 0;
	float posY = 0;
	for (size_t i = 0; i < 48; i++)
	{

		posX = inX + (i % 8) * (m_panelSizeX + m_spacingX);
		posY = inY + (i / 8) * (m_panelSizeY + m_spacingX);

		if (resindex == 1)
		{
			m_pText[i] = new SText(-2, istrMessage, 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);
			m_pText2[i] = new SText(-2, "", 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);

			m_pPanels[i] = new SPanel(-2, posX, posY, m_panelSizeX, m_panelSizeY, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

			m_pBgzin[i] = new SPanel(-2, posX, posY, 65.0f, 40.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		}
		else if (resindex == 2)
		{
			m_pText[i] = new SText(-2, istrMessage, 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);
			m_pText2[i] = new SText(-2, "", 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);

			m_pPanels[i] = new SPanel(-2, posX, posY, m_panelSizeX, m_panelSizeY, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

			m_pBgzin[i] = new SPanel(-2, posX, posY, 65.0f, 40.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		}
		else if (resindex == 3)
		{
			m_pText[i] = new SText(-2, istrMessage, 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);
			m_pText2[i] = new SText(-2, "", 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);

			m_pPanels[i] = new SPanel(-2, posX, posY, m_panelSizeX, m_panelSizeY, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

			m_pBgzin[i] = new SPanel(-2, posX, posY, 65.0f, 44.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		}

		else if (resindex == 4)
		{
			m_pText[i] = new SText(-2, istrMessage, 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);
			m_pText2[i] = new SText(-2, "", 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);

			m_pPanels[i] = new SPanel(-2, posX, posY, m_panelSizeX, m_panelSizeY, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

			m_pBgzin[i] = new SPanel(-2, posX, posY, 65.0f, 42.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		}

		else if (resindex == 5)
		{
			m_pText[i] = new SText(-2, istrMessage, 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);
			m_pText2[i] = new SText(-2, "", 0xFFFFFFFF, 0, 0, m_panelSizeX - 2.0f, m_panelSizeY - 2.0f, 1, 0, 1, 1);

			m_pPanels[i] = new SPanel(-2, posX, posY, m_panelSizeX, m_panelSizeY, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);

			m_pBgzin[i] = new SPanel(-2, posX, posY, 65.0f, 40.0f, 0, RENDERCTRLTYPE::RENDER_IMAGE_STRETCH);
		}

		m_GCPanel.nPosX = posX;
		m_GCPanel.nPosY = posY;

		if (m_pText[i] != nullptr)
			AddChild(m_pText[i]);

		if (m_pText2[i] != nullptr)
			AddChild(m_pText2[i]);

		if (m_pPanels[i] != nullptr)
			AddChild(m_pPanels[i]);

		if (m_pBgzin[i] != nullptr)
			AddChild(m_pBgzin[i]);
	}
}
CapeLeilaoStore::~CapeLeilaoStore()
{
	for (size_t i = 0; i < 48; i++)
	{
		SAFE_DELETE(m_pText[i]);
		SAFE_DELETE(m_pText2[i]);
	}
}
void CapeLeilaoStore::SetPrice(int panelIndex, const char* istrMessage)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pText[panelIndex])
		{
			m_pText[panelIndex]->SetText((char*)istrMessage, 0);
		}
	}
}
void CapeLeilaoStore::SetPricePos(int panelIndex, float x, float y)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pText[panelIndex])
		{
			m_pText[panelIndex]->m_nPosX = x;
			m_pText[panelIndex]->m_nPosY = y;
		}
	}
}
void CapeLeilaoStore::SetNamePos(int panelIndex, float x, float y)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pText2[panelIndex])
		{
			m_pText2[panelIndex]->SetPos(x, y);
		}
	}
}
void CapeLeilaoStore::SetName(int panelIndex, const char* istrMessage)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pText2[panelIndex])
		{
			m_pText2[panelIndex]->SetText((char*)istrMessage, 0);
		}
	}
}
void CapeLeilaoStore::SetPriceColor(int panelIndex, unsigned int dwFontColor)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pText[panelIndex])
		{
			m_pText[panelIndex]->SetTextColor(dwFontColor);
		}
	}
}
void CapeLeilaoStore::SetNameColor(int panelIndex, unsigned int dwFontColor)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pText2[panelIndex])
		{
			m_pText2[panelIndex]->SetTextColor(dwFontColor);
		}
	}
}
void CapeLeilaoStore::SetTextureSetIndexForPanel(int panelIndex, int textureIndex)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pPanels[panelIndex])
		{
			m_pPanels[panelIndex]->SetTextureSetIndex(textureIndex);
		}
	}
}
void CapeLeilaoStore::SetReset(int panelIndex)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pPanels[panelIndex])
		{
			SetVisibleForPanel(panelIndex, 0); 
			SetName(panelIndex, "");
			SetPrice(panelIndex, "");
			SetTextureSetIndexForPanel(panelIndex, -1); // reset
		}
	}
}
void CapeLeilaoStore::SetVisibleForPanel(int panelIndex, int indice)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pPanels[panelIndex])
		{
			m_pPanels[panelIndex]->SetVisible(indice);
			m_pText[panelIndex]->SetVisible(indice);
			m_pText2[panelIndex]->SetVisible(indice);
			m_pBgzin[panelIndex]->SetVisible(indice);
		}
	}
}
void CapeLeilaoStore::SetMovePanels(int panelIndex, float horizontalSpacing, float verticalSpacing)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pPanels[panelIndex])
		{
			m_pPanels[panelIndex]->SetPos(horizontalSpacing * RenderDevice::m_fWidthRatio, verticalSpacing * RenderDevice::m_fHeightRatio);
		}
	}
}
void CapeLeilaoStore::SetSizePanels(int panelIndex, float horizontalSpacing, float verticalSpacing)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pPanels[panelIndex])
		{
			m_pPanels[panelIndex]->SetSize(horizontalSpacing * RenderDevice::m_fWidthRatio, verticalSpacing * RenderDevice::m_fHeightRatio);
		}
	}
}
void CapeLeilaoStore::SetSizeBgzin(int panelIndex, float horizontalSpacing, float verticalSpacing)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pBgzin[panelIndex])
		{
			m_pBgzin[panelIndex]->SetSize(horizontalSpacing * RenderDevice::m_fWidthRatio, verticalSpacing * RenderDevice::m_fHeightRatio);
		}
	}
}
void CapeLeilaoStore::SetPanelSpacingForPanels(float horizontalSpacing, float verticalSpacing)
{
	m_spacingX = horizontalSpacing;
	m_spacingY = verticalSpacing;

	for (size_t i = 0; i < 48; i++)
	{
		float adjustedPosX = (i % 8) * (m_panelSizeX + m_spacingX); // Espaçamento horizontal
		float adjustedPosY = (i / 8) * (m_panelSizeY + m_spacingY); // Espaçamento vertical

		m_pPanels[i]->SetPos(adjustedPosX + 1.0f * RenderDevice::m_fWidthRatio, adjustedPosY * RenderDevice::m_fHeightRatio);
	}
}
void CapeLeilaoStore::SetPanelSpacingForText(float horizontalSpacing, float verticalSpacing)
{
	m_spacingX = horizontalSpacing;
	m_spacingY = verticalSpacing;

	for (size_t i = 0; i < 48; i++)
	{
		float adjustedPosX = (i % 8) * (m_panelSizeX + m_spacingX); // Espaçamento horizontal
		float adjustedPosY = (i / 8) * (m_panelSizeY + m_spacingY); // Espaçamento vertical

		m_pText[i]->SetPos(adjustedPosX + 7.0f * RenderDevice::m_fWidthRatio, adjustedPosY + 23.0f * RenderDevice::m_fHeightRatio);
		m_pText2[i]->SetPos(adjustedPosX + 4.0f * RenderDevice::m_fWidthRatio, adjustedPosY + 13.0f * RenderDevice::m_fHeightRatio);
	}
}
void CapeLeilaoStore::SetColorBgzin(int panelIndex, unsigned int idwColor)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pBgzin[panelIndex])
		{
			m_pBgzin[panelIndex]->m_GCPanel.dwColor = idwColor;
		}
	}
}
void CapeLeilaoStore::SetMoveBgzin(float horizontalSpacing, float verticalSpacing)
{
	for (size_t i = 0; i < 48; i++)
	{
		m_pBgzin[i]->SetPos(horizontalSpacing * RenderDevice::m_fWidthRatio, verticalSpacing * RenderDevice::m_fHeightRatio);
	}
}
void CapeLeilaoStore::SetMovePrice(int panelIndex, float horizontalSpacing, float verticalSpacing)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pText[panelIndex])
		{
			m_pText[panelIndex]->SetPos(horizontalSpacing * RenderDevice::m_fWidthRatio, verticalSpacing * RenderDevice::m_fHeightRatio);
		}
	}
}
void CapeLeilaoStore::SetMoveName(int panelIndex, float horizontalSpacing, float verticalSpacing)
{
	if (panelIndex >= 0 && panelIndex < 48)
	{
		if (m_pText2[panelIndex])
		{
			m_pText2[panelIndex]->SetPos(horizontalSpacing * RenderDevice::m_fWidthRatio, verticalSpacing * RenderDevice::m_fHeightRatio);
		}
	}
}
void CapeLeilaoStore::SetPanelSpacingForBgzin(float horizontalSpacing, float verticalSpacing)
{
	m_spacingX = horizontalSpacing;
	m_spacingY = verticalSpacing;

	for (size_t i = 0; i < 48; i++)
	{
		float adjustedPosX = (i % 8) * (m_panelSizeX + m_spacingX); // Espaçamento horizontal
		float adjustedPosY = (i / 8) * (m_panelSizeY + m_spacingY); // Espaçamento vertical

		m_pBgzin[i]->SetPos(adjustedPosX - 40.0f * RenderDevice::m_fWidthRatio, adjustedPosY - 5.0f * RenderDevice::m_fHeightRatio);
	}
}
void CapeLeilaoStore::SetPanelSpacingForMainPanel(float panelPosX, float panelPosY)
{
	m_GCPanel.nPosX = (float)g_pDevice->m_dwScreenWidth - m_GCPanel.nWidth - panelPosX * RenderDevice::m_fWidthRatio;
	m_GCPanel.nPosY = panelPosY * RenderDevice::m_fHeightRatio;
}
void CapeLeilaoStore::FrameMove2(stGeomList* pDrawList, TMVector2 ivParentPos, int inParentLayer, int nFlag)
{
	unsigned int dwServerTime = g_pTimerManager->GetServerTime();

	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

	auto res = RenderDevice::m_fResBase;
	 

	SPanel::FrameMove2(pDrawList, ivParentPos, inParentLayer, nFlag);

	if (pFScene->m_pAffectDesc)
	{
		for (int i = 0; i < 32; ++i)
		{
			if (m_bVisible)
			{
				pFScene->m_pAffectDesc->SetVisible(0);
				pFScene->m_pAffectIcon[i]->SetVisible(0);
				pFScene->m_pAffect[i]->SetVisible(0);
			}
			else
			{
				pFScene->m_pAffectDesc->SetVisible(1);

				pFScene->m_pAffectIcon[i]->SetVisible(1);
				pFScene->m_pAffect[i]->SetVisible(1);

			}
		}
	}


	for (size_t i = 0; i < 48; i++) 
	{  
		if (res == 1)
		{ 
			SetPos(BASE_ScreenResize(63.0f), BASE_ScreenResize(86.5f));
			SetPanelSpacingForMainPanel(235.0f, 2.0f);
			float m_spacingX = BASE_ScreenResize(6.5f);
			float m_spacingY = BASE_ScreenResize(40.0f);
			m_pBgzin[i]->SetSize(BASE_ScreenResize(64.50f), BASE_ScreenResize(39.5f));
			float adjustedPosX = (i % 8) * (59.8f + m_spacingX); // Espaçamento horizontal
			float adjustedPosY = (i / 8) * (7.0f + m_spacingY); // Espaçamento vertical
			m_pBgzin[i]->SetPos(BASE_ScreenResize(adjustedPosX - 45.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(adjustedPosY - 5.0f * RenderDevice::m_fHeightRatio));
			
			m_spacingX = BASE_ScreenResize(6.5f);
			m_spacingY = BASE_ScreenResize(40.0f);
			m_pText[i]->SetSize(BASE_ScreenResize(65.0f), BASE_ScreenResize(40.0f));
			adjustedPosX = (i % 8) * (60.0f + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (7.0f + m_spacingY); // Espaçamento vertical
			m_pText2[i]->SetPos(adjustedPosX - BASE_ScreenResize(12.0f) * RenderDevice::m_fWidthRatio, adjustedPosY + BASE_ScreenResize(11.5) * RenderDevice::m_fHeightRatio);
			m_pText[i]->SetPos(BASE_ScreenResize(adjustedPosX - BASE_ScreenResize(25.0f) * RenderDevice::m_fWidthRatio), BASE_ScreenResize(adjustedPosY - 1.0f * RenderDevice::m_fHeightRatio));
			
			adjustedPosX = (i % 8) * (BASE_ScreenResize(59.8f) + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (BASE_ScreenResize(6.50f) + m_spacingY); // Espaçamento vertical
			m_pPanels[i]->SetSize(BASE_ScreenResize(25.0f), BASE_ScreenResize(20.0f));
			m_pPanels[i]->SetPos(adjustedPosX - BASE_ScreenResize(6.0f) * RenderDevice::m_fWidthRatio, adjustedPosY * RenderDevice::m_fHeightRatio);

		}
		if (res == 2)
		{ 
			SetPos(BASE_ScreenResize(63.0f), BASE_ScreenResize(86.5f));
			SetPanelSpacingForMainPanel(320.0f, 2.0f);

			float m_spacingX = BASE_ScreenResize(6.5f);
			float m_spacingY = BASE_ScreenResize(40.0f);

			m_pBgzin[i]->SetSize(BASE_ScreenResize(65.0f), BASE_ScreenResize(39.5f));

			float adjustedPosX = (i % 8) * (59.0f + m_spacingX); // Espaçamento horizontal
			float adjustedPosY = (i / 8) * (0.350f + m_spacingY); // Espaçamento vertical

			m_pBgzin[i]->SetPos(BASE_ScreenResize(adjustedPosX - 40.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(adjustedPosY - 5.0f * RenderDevice::m_fHeightRatio));

			m_spacingX = BASE_ScreenResize(5.3f) * RenderDevice::m_fWidthRatio;
			m_spacingY = BASE_ScreenResize(35.0f) * RenderDevice::m_fHeightRatio;

			m_pText[i]->SetSize(BASE_ScreenResize(68.0f), BASE_ScreenResize(40.0f));

			adjustedPosX = (i % 8) * (71.0f + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (1.0f + m_spacingY); // Espaçamento vertical

			m_pText2[i]->SetPos((i % 8) * (69.50f + m_spacingX) - BASE_ScreenResize(8.0f) * RenderDevice::m_fWidthRatio, (i / 8) * (7.0f + m_spacingY) + BASE_ScreenResize(0.15) * RenderDevice::m_fHeightRatio);
			m_pText[i]->SetPos(BASE_ScreenResize((i % 8) * (59.50f + m_spacingX) - BASE_ScreenResize(20.0f) * RenderDevice::m_fWidthRatio), BASE_ScreenResize(adjustedPosY + 3.0f * RenderDevice::m_fHeightRatio));

			adjustedPosX = (i % 8) * (BASE_ScreenResize(59.8f) + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (BASE_ScreenResize(0.0010f) + m_spacingY); // Espaçamento vertical

			m_pPanels[i]->SetSize(BASE_ScreenResize(20.0f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(15.0f) * RenderDevice::m_fHeightRatio);
			m_pPanels[i]->SetPos(adjustedPosX - BASE_ScreenResize(4.0f) * RenderDevice::m_fWidthRatio, adjustedPosY * RenderDevice::m_fHeightRatio);

		}

		if (res == 3)
		{ 
			pFScene->m_pGridLeilaoStore->m_GCGrid->nWidth = 66.30f;
			pFScene->m_pGridLeilaoStore->m_GCGrid->nHeight = 50.0f;
			SetPos(BASE_ScreenResize(70.0f), BASE_ScreenResize(95.5f));
			SetPanelSpacingForMainPanel(385.0f, 2.0f);

			float m_spacingX = BASE_ScreenResize(5.5f);
			float m_spacingY = BASE_ScreenResize(37.0f);

			m_pBgzin[i]->SetSize(BASE_ScreenResize(48.0f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(30.5f) * RenderDevice::m_fHeightRatio);

			float adjustedPosX = (i % 8) * (58.80f + m_spacingX); // Espaçamento horizontal
			float adjustedPosY = (i / 8) * (0.350f + m_spacingY); // Espaçamento vertical

			m_pBgzin[i]->SetPos(BASE_ScreenResize(adjustedPosX - 40.0f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(adjustedPosY - 5.0f * RenderDevice::m_fHeightRatio));

			m_spacingX = BASE_ScreenResize(4.8f) * RenderDevice::m_fWidthRatio;
			m_spacingY = BASE_ScreenResize(27.50f) * RenderDevice::m_fHeightRatio;

			m_pText[i]->SetSize(BASE_ScreenResize(68.0f), BASE_ScreenResize(40.0f));

			adjustedPosX = (i % 8) * (70.0f + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (1.0f + m_spacingY); // Espaçamento vertical

			m_pText2[i]->SetPos((i % 8) * (89.15) - BASE_ScreenResize(13.20f) * RenderDevice::m_fWidthRatio, (i / 8) * (1.0f + 68.0f) - BASE_ScreenResize(3.015) * RenderDevice::m_fHeightRatio);
			m_pText[i]->SetPos(BASE_ScreenResize((i % 8) * (57.50f + m_spacingX) - BASE_ScreenResize(20.0f) * RenderDevice::m_fWidthRatio), BASE_ScreenResize(adjustedPosY + 3.0f * RenderDevice::m_fHeightRatio));

			adjustedPosX = (i % 8) * (BASE_ScreenResize(59.51f) + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (BASE_ScreenResize(0.1010f) + m_spacingY); // Espaçamento vertical

			m_pPanels[i]->SetSize(BASE_ScreenResize(15.0f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(10.0f) * RenderDevice::m_fHeightRatio);
			m_pPanels[i]->SetPos(adjustedPosX - BASE_ScreenResize(8.0f) * RenderDevice::m_fWidthRatio, adjustedPosY * RenderDevice::m_fHeightRatio);

		}

		if (res == 4)
		{
			pFScene->m_pGridLeilaoStore->m_GCGrid->nWidth = 52.30f * RenderDevice::m_fWidthRatio;
			pFScene->m_pGridLeilaoStore->m_GCGrid->nHeight = 37.0f * RenderDevice::m_fHeightRatio;
			SetPos(BASE_ScreenResize(70.0f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(95.5f) * RenderDevice::m_fHeightRatio);
			SetPanelSpacingForMainPanel(400.0f * RenderDevice::m_fWidthRatio, 2.0f * RenderDevice::m_fHeightRatio);

			float m_spacingX = BASE_ScreenResize(5.5f) * RenderDevice::m_fWidthRatio;
			float m_spacingY = BASE_ScreenResize(28.0f) * RenderDevice::m_fHeightRatio;

			m_pBgzin[i]->SetSize(BASE_ScreenResize(53.0f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(30.5f) * RenderDevice::m_fHeightRatio);

			float adjustedPosX = (i % 8) * (57.8f + m_spacingX); // Espaçamento horizontal
			float adjustedPosY = (i / 8) * (1.8350f + m_spacingY); // Espaçamento vertical

			m_pBgzin[i]->SetPos(BASE_ScreenResize(adjustedPosX - 57.50f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(adjustedPosY - 29.50f * RenderDevice::m_fHeightRatio));

			m_spacingX = BASE_ScreenResize(4.8f) * RenderDevice::m_fWidthRatio;
			m_spacingY = BASE_ScreenResize(27.50f) * RenderDevice::m_fHeightRatio;

			m_pText[i]->SetSize(BASE_ScreenResize(68.0f), BASE_ScreenResize(40.0f));

			adjustedPosX = (i % 8) * (70.0f + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (1.0f + m_spacingY); // Espaçamento vertical

			m_pText2[i]->SetPos((i % 8) * (84.15) - BASE_ScreenResize(30.20f) * RenderDevice::m_fWidthRatio, (i / 8) * (1.0f + 59.0f) - BASE_ScreenResize(30.015) * RenderDevice::m_fHeightRatio);
			m_pText[i]->SetPos(BASE_ScreenResize((i % 8) * (58.9f + m_spacingX) - BASE_ScreenResize(32.20f) * RenderDevice::m_fWidthRatio), BASE_ScreenResize((i / 8)* (2.3f + m_spacingY) - 22.015 * RenderDevice::m_fHeightRatio));

			adjustedPosX = (i % 8) * (BASE_ScreenResize(59.91f) + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (BASE_ScreenResize(12.1010f) + m_spacingY); // Espaçamento vertical

			m_pPanels[i]->SetSize(BASE_ScreenResize(15.0f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(10.0f) * RenderDevice::m_fHeightRatio);
			m_pPanels[i]->SetPos(adjustedPosX - BASE_ScreenResize(23.9750f) * RenderDevice::m_fWidthRatio, adjustedPosY - BASE_ScreenResize(27.9750f) * RenderDevice::m_fHeightRatio);

		}

		if (res == 5)
		{
			pFScene->m_pGridLeilaoStore->m_GCGrid->nWidth = 48.0f * RenderDevice::m_fWidthRatio;
			pFScene->m_pGridLeilaoStore->m_GCGrid->nHeight = 33.8f * RenderDevice::m_fHeightRatio;
			SetPos(BASE_ScreenResize(70.0f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(95.5f) * RenderDevice::m_fHeightRatio);
			SetPanelSpacingForMainPanel(595.0f * RenderDevice::m_fWidthRatio, 2.0f * RenderDevice::m_fHeightRatio);

			float m_spacingX = BASE_ScreenResize(5.5f) * RenderDevice::m_fWidthRatio;
			float m_spacingY = BASE_ScreenResize(24.0f) * RenderDevice::m_fHeightRatio;

			m_pBgzin[i]->SetSize(BASE_ScreenResize(46.65f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(30.5f) * RenderDevice::m_fHeightRatio);

			float adjustedPosX = (i % 8) * (55.50f + m_spacingX); // Espaçamento horizontal
			float adjustedPosY = (i / 8) * (0.0350f + m_spacingY); // Espaçamento vertical

			m_pBgzin[i]->SetPos(BASE_ScreenResize(adjustedPosX - 56.90f * RenderDevice::m_fWidthRatio), BASE_ScreenResize(adjustedPosY - 36.50f * RenderDevice::m_fHeightRatio));

			m_spacingX = BASE_ScreenResize(4.8f) * RenderDevice::m_fWidthRatio;
			m_spacingY = BASE_ScreenResize(27.50f) * RenderDevice::m_fHeightRatio;

			m_pText[i]->SetSize(BASE_ScreenResize(68.0f), BASE_ScreenResize(40.0f));

			adjustedPosX = (i % 8) * (70.0f + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (1.0f + m_spacingY); // Espaçamento vertical

			m_pText2[i]->SetPos((i % 8) * (92.15) - BASE_ScreenResize(30.20f) * RenderDevice::m_fWidthRatio, (i / 8) * (1.0f + 63.0f) - BASE_ScreenResize(30.015) * RenderDevice::m_fHeightRatio);
			m_pText[i]->SetPos((i % 8)* (93.15) - BASE_ScreenResize(42.50f) * RenderDevice::m_fWidthRatio, (i / 8)* (1.0f + 64.0f) - BASE_ScreenResize(25.015) * RenderDevice::m_fHeightRatio);
			 
			adjustedPosX = (i % 8) * (BASE_ScreenResize(59.91f) + m_spacingX); // Espaçamento horizontal
			adjustedPosY = (i / 8) * (BASE_ScreenResize(8.1010f) + m_spacingY); // Espaçamento vertical

			m_pPanels[i]->SetSize(BASE_ScreenResize(15.0f) * RenderDevice::m_fWidthRatio, BASE_ScreenResize(10.0f) * RenderDevice::m_fHeightRatio);
			m_pPanels[i]->SetPos(adjustedPosX - BASE_ScreenResize(27.9750f) * RenderDevice::m_fWidthRatio, adjustedPosY - BASE_ScreenResize(33.9750f) * RenderDevice::m_fHeightRatio);

		}
		m_pPanels[i]->FrameMove(dwServerTime);
		m_pBgzin[i]->FrameMove(dwServerTime);
	}
}
void CapeLeilaoStore::SetVisible(int bVisible, int bSound)
{
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	if (pFScene->m_pLeilaoStore && pFScene->m_pLeilaoStore->m_bVisible && !bVisible)
	{
		pFScene->m_pLeilaoStore->m_bVisible = 0; 
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