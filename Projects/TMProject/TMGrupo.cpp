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


int TMFieldScene::m_bAutoGrupo = 0;
int TMFieldScene::m_nListaGrupoCurrentPage = 1; // Página atual
int TMFieldScene::m_nListaGrupoTotalPages = 1;  // Total de páginas  
char TMFieldScene::LeaderName[16];
char TMFieldScene::LeaderPass[6];

int TMFieldScene::InitializeListaGrupo()
{  
	m_pListaGrupo = (SPanel*)m_pControlContainer->FindControl(19930);
	m_pListaGrupo->SetVisible(0);
	memset(LeaderName, 0, sizeof LeaderName);
	memset(LeaderPass, 0, sizeof LeaderPass);

	/*m_pAutoTradeValuePanel = (SPanel*)m_pControlContainer->FindControl(565885);
	if (m_pAutoTradeValuePanel)
	{
		m_pAutoTradeValuePanel->SetVisible(0);
		m_pAutoTradeValuePanel->SetPos(((float)g_pDevice->m_dwScreenWidth * 0.5f) - (m_pAutoTradeValuePanel->m_nWidth * 0.8f),
			((float)g_pDevice->m_dwScreenHeight * 0.5f) - (m_pAutoTradeValuePanel->m_nHeight * 1.5f));
	}*/

	return 1;
} 
//ok
void TMFieldScene::UpdateElementesListaGrupo() {
 
}  
//ok
void TMFieldScene::SendReqListaGrupoEntrar()//aqui compra o item
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
//ok
int TMFieldScene::SendListaGrupoSair(int itemType, int alvo)
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
int TMFieldScene::SendBuscarListaGrupo(int itemType, char* ItemName)
{
	memset(&_HudControl.gListgrup, 0, sizeof MSG_ListaGrupo);

	auto Lideres = (SListBox*)m_pControlContainer->FindControl(19931);
	if (Lideres)
		Lideres->Empty();
	auto StatusPT = (SListBox*)m_pControlContainer->FindControl(19951);
	if (StatusPT)
		StatusPT->Empty();
	auto qtddd = (SListBox*)m_pControlContainer->FindControl(19961);
	if (qtddd)
		qtddd->Empty();
	pMsgSignalChar req{};
	req.Header.ID = g_pObjectManager->m_dwCharID;
	req.Header.Type = 0xCDB6;
	req.Value = m_nListaGrupoCurrentPage;
	if (strlen(ItemName) < sizeof(req.Nick)) {
		strcpy(req.Nick, ItemName);
	}
	else {
		// Trate o caso de erro aqui, por exemplo, truncando ou retornando uma mensagem de erro.
		strncpy(req.Nick, ItemName, sizeof(req.Nick) - 1);
		req.Nick[sizeof(req.Nick) - 1] = '\0'; // Garante que o último caractere é nulo.
	}

	SendOneMessage((char*)&req, sizeof(req)); 
	return 1;
}

int TMFieldScene::ListaGrupoButtonAction(int idwControlID, int idwEvent)
{
	if (idwControlID == 19935)//idwControlID do Botão ID
	{
		ListaGrupoPreviousPage();
		return 1;
	}
	if (idwControlID == 19933)//lupa
	{
		memset(&_HudControl.gListgrup, 0, sizeof MSG_ListaGrupo);

		auto Lideres = (SListBox*)m_pControlContainer->FindControl(19931);
		if (Lideres)
			Lideres->Empty();
		auto StatusPT = (SListBox*)m_pControlContainer->FindControl(19951);
		if (StatusPT)
			StatusPT->Empty();
		auto qtddd = (SListBox*)m_pControlContainer->FindControl(19961);
		if (qtddd)
			qtddd->Empty();
		m_nListaGrupoCurrentPage = 1;
		 
		SendListaGrupo();
		m_pControlContainer->SetFocusedControl(0);
		return 1;
	}
	
	if (idwControlID == 19934)
	{
		ListaGrupoNextPage();
		return 1;
	}
	if (idwControlID == 19944)
	{
		MSG_MessageWhisper stWhisper{};

		stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
		stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

		sprintf_s(stWhisper.MobName, "passgrupo");

		SendOneMessage((char*)&stWhisper, sizeof(stWhisper));
		SendListaGrupo();
		return 1;
	}
	if (idwControlID == 19945)
	{
		MSG_MessageWhisper stWhisper{};

		stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
		stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

		sprintf_s(stWhisper.MobName, "atvevoc");

		SendOneMessage((char*)&stWhisper, sizeof(stWhisper));
		SendListaGrupo();
		return 1;
	}
	if (idwControlID == 19947)
	{
		MSG_MessageWhisper stWhisper{};

		stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
		stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

		sprintf_s(stWhisper.MobName, "atvfll");

		SendOneMessage((char*)&stWhisper, sizeof(stWhisper));
		SendListaGrupo();
		return 1;
	}
	 

	if (idwControlID == 19931) {
		auto bbTex = (SListBox*)m_pControlContainer->FindControl(idwControlID);
		char strSexy[56] = {};
		unsigned int Statusgrupo;

		if (bbTex) {
			const char* itemText = bbTex->m_pItemList[idwEvent]->GetText();
			if (itemText != nullptr) {

				strncpy(LeaderName, itemText, sizeof(LeaderName) - 1);

				auto pEdit = (SEditableText*)g_pCurrentScene->m_pControlContainer->FindControl(65889);
				pEdit->SetText((char*)"");
				// Verificação para encontrar o líder correspondente
				bool leaderFound = false;
				size_t itemTextLength = strlen(itemText);
				for (size_t i = 0; i < _HudControl.gListgrup.LeaderCount; i++) {
					if (!strncmp(_HudControl.gListgrup.Leaders[i].Lider, itemText, itemTextLength)) {
						Statusgrupo = _HudControl.gListgrup.Leaders[i].Status;
						leaderFound = true;

						if (Statusgrupo)//Livre
							strncpy(LeaderPass, _HudControl.gListgrup.Leaders[i].Pass, sizeof(LeaderPass) - 1);
						else
							strncpy(LeaderPass, "", sizeof(LeaderPass) - 1);

						break;
					}
				}

				if (leaderFound) {

					if (Statusgrupo)//Livre
					{
						if (!m_pMessageBox->IsVisible()) {
							snprintf(strSexy, sizeof(strSexy), "confirme para entrar no grupo %s", itemText);



							m_pMessageBox->SetMessage(strSexy, 19931, 0);
							m_pMessageBox->SetVisible(1);
						}
					}
					else//Privado
					{
						m_pInputGoldPanel->SetVisible(1);
						auto pText = (SText*)m_pControlContainer->FindControl(65888);
						if (pText)
						{
							snprintf(strSexy, sizeof(strSexy), "insira a senha do grupo %s", itemText);
							m_nCoinMsgType = 19931;
							pText->SetText(strSexy, 0); 
							m_pControlContainer->SetFocusedControl((SControl*)pEdit);
						}
					}






				}
			}
		}
	}
	
	if (idwControlID == 19946)
	{
		MSG_MessageWhisper stWhisper{};

		stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
		stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

		sprintf_s(stWhisper.MobName, "atvgrupo");

		SendOneMessage((char*)&stWhisper, sizeof(stWhisper));
		SendListaGrupo();
		return 1;
	}
	
}
//ok
int TMFieldScene::SendListaGrupo()
{
	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

	// Estrutura de mensagem para solicitação de ranking
	struct MSG_ReqListaGrupo {
		MSG_STANDARD Header;
		int CurrentPage; // Campo para a página atual
	};
	MSG_ReqListaGrupo stParm{};
	stParm.Header.ID = pFScene->m_pMyHuman->m_dwID;
	stParm.Header.Type = 0xCDB1;
	stParm.CurrentPage = m_nListaGrupoCurrentPage;
	g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));
	return 1;
}//ok
void TMFieldScene::ListaGrupoNextPage() {

	if (m_nListaGrupoCurrentPage < m_nListaGrupoTotalPages)
	{
		m_nListaGrupoCurrentPage++;
	}
	std::cout << "[ListaGrupoNextPage] m_nListaGrupoCurrentPage " << m_nListaGrupoCurrentPage << std::endl;

	SendListaGrupo();
}
//ok
void TMFieldScene::ListaGrupoPreviousPage() {

	m_nListaGrupoCurrentPage--;
	if (m_nListaGrupoCurrentPage <= -1)
		m_nListaGrupoCurrentPage = 0;
	if (m_nListaGrupoCurrentPage <= 0)
		m_nListaGrupoCurrentPage = 1;

	std::cout << "[ListaGrupoPreviousPage] m_nListaGrupoCurrentPage " << m_nListaGrupoCurrentPage << std::endl;
	SendListaGrupo();
}
int TMFieldScene::OnReceivingGrupo(MSG_STANDARD* pStd) {


	auto pack = (MSG_ListaGrupo*)pStd;
	memcpy(&_HudControl.gListgrup, pack, sizeof MSG_ListaGrupo);
	if (pack == nullptr) {
		std::cout << "[OnReceivingGrupo] Falha ao interpretar a mensagem de grupo." << std::endl;
		return 0;
	}
	//m_nListaGrupoCurrentPage = pack->SceneIndex;
	std::cout << "[OnReceiveListaGrupo] Número de líderes recebidos: " << _HudControl.gListgrup.LeaderCount << std::endl;

	if (_HudControl.gListgrup.LeaderCount <= 0) {
		std::cout << "[OnReceiveListaGrupo] Nenhum líder recebido." << std::endl;
		return 1;
	}
	m_nListaGrupoTotalPages = _HudControl.gListgrup.SceneIndex;

	char strSexy[56] = {};
	char strSexy2[128] = {};

	auto currentpage = (SText*)m_pControlContainer->FindControl(19936);		 
	snprintf(strSexy, sizeof(strSexy), "%d", m_nListaGrupoCurrentPage);
	currentpage->SetText((char*)strSexy, 0);

	auto passgrupo = (SText*)m_pControlContainer->FindControl(19942);
	 

	std::cout << "[OnReceiveListaGrupo] m_nListaGrupoTotalPages recebidos: " << m_nListaGrupoTotalPages << std::endl;
	
	 
	 
	auto qtddd = (SListBox*)m_pControlContainer->FindControl(19961);
	if (qtddd) {
		qtddd->Empty();


		for (int i = 0; i < _HudControl.gListgrup.LeaderCount; i++) {
			char strSexy[128];
			 
			snprintf(strSexy, sizeof(strSexy), "%d/12",
				_HudControl.gListgrup.Leaders[i].Member);

			auto labelname = new SListBoxItem("", 0xFFFFFFFF, 73.0f * RenderDevice::m_fHeightRatio, 2.0f * RenderDevice::m_fHeightRatio, 157.0f * RenderDevice::m_fHeightRatio, 8.0f * RenderDevice::m_fWidthRatio, 1, 0x15FFFFFF, 1, 0);
			labelname->SetText(strSexy, 0);

			labelname->SetTextColor(0xFFFFFFFF);
			labelname->m_dwAlignType = 3;
			qtddd->m_bSelectEnable = 0;
			labelname->m_GCText.m_fHeight = 1.0f;
			qtddd->AddItem(labelname);



		}

		if (qtddd->m_nNumItem > 0) {
			qtddd->m_nSelectedItem = 0;
		}
	} 
	auto StatusPT = (SListBox*)m_pControlContainer->FindControl(19951);
	if (StatusPT) {
		StatusPT->Empty();


		for (int i = 0; i < _HudControl.gListgrup.LeaderCount; i++) {
			char strSexy[128];

			const char* statusText = (_HudControl.gListgrup.Leaders[i].Status == 1) ? "Livre" : "Privado";

			// Formata a string com o nome do líder, número de membros e o status
			snprintf(strSexy, sizeof(strSexy), "%s",
				statusText);

			auto labelname = new SListBoxItem("", 0xFFFFFFFF, 115.0f * RenderDevice::m_fHeightRatio, 2.0f * RenderDevice::m_fHeightRatio, 157.0f * RenderDevice::m_fHeightRatio, 8.0f * RenderDevice::m_fWidthRatio, 1, 0x15FFFFFF, 1, 0);
			labelname->SetText(strSexy, 0);

			labelname->SetTextColor(0xFFFFFFFF);
			labelname->m_dwAlignType = 3;
			StatusPT->m_bSelectEnable = 0;
			labelname->m_GCText.m_fHeight = 1.0f;
			StatusPT->AddItem(labelname);
		}

		if (StatusPT->m_nNumItem > 0) {
			StatusPT->m_nSelectedItem = 0;
		}
	}

	auto Lideres = (SListBox*)m_pControlContainer->FindControl(19931);
	if (Lideres) {

		Lideres->Empty();
		Lideres->m_nSelectedItem = 0;


		for (int i = 0; i < _HudControl.gListgrup.LeaderCount; i++) {

			// Formata a string com o nome do líder, número de membros e o status
			snprintf(strSexy, sizeof(strSexy), "%s",
				_HudControl.gListgrup.Leaders[i].Lider);
			 
			auto labelname = new SListBoxItem("", 0xFFFFFFFF, 2.0f * RenderDevice::m_fHeightRatio, 2.0f * RenderDevice::m_fHeightRatio, 154.0f * RenderDevice::m_fHeightRatio, 8.0f * RenderDevice::m_fWidthRatio, 1, 0x15FFFFFF, 1, 0);
			labelname->SetText(strSexy, 0);

			labelname->SetTextColor(0xFFFFFFFF);
			labelname->m_dwAlignType = 3;
			Lideres->m_bSelectEnable = 1;
			labelname->m_GCText.m_fHeight = 1.0f;
			Lideres->AddItem(labelname);
		}

	}

	if (_HudControl.gListgrup.Update) {
		std::cout << "[OnReceiveListaGrupo] Atualizando a interface dos grupos..." << std::endl;
	}

	return 1;
}
//ok
int TMFieldScene::OnKeyVisibleListaGrupo(char iCharCode, int lParam)
{
	if (iCharCode != 'j' && iCharCode != 'J')
		return 0;

	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);

	if (m_pListaGrupo)
	{
		m_pListaGrupo->SetVisible(m_pListaGrupo->m_bVisible == 0);
	}
;


	if (m_pListaGrupo->m_bVisible)
	{
		memset(&_HudControl.gListgrup, 0, sizeof MSG_ListaGrupo);

		auto Lideres = (SListBox*)m_pControlContainer->FindControl(19931);
		if (Lideres)
			Lideres->Empty();
		auto StatusPT = (SListBox*)m_pControlContainer->FindControl(19951);
		if (StatusPT)
			StatusPT->Empty();
		auto qtddd = (SListBox*)m_pControlContainer->FindControl(19961);
		if (qtddd)
			qtddd->Empty();

		MSG_ReqListaGrupo stParm{};
		stParm.Header.ID = pFScene->m_pMyHuman->m_dwID;
		stParm.Header.Type = 0xCDB1;
		stParm.CurrentPage = 1;
		g_pSocketManager->SendOneMessage((char*)&stParm, sizeof(stParm));
	}
	return 1;
}




  