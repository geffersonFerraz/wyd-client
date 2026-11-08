#include "pch.h"
#include "RouletteSystem.h"
#include "TMGlobal.h"
#include "TMFieldScene.h"
#include "SGrid.h"
#include "SControl.h"
#include "SControlContainer.h"
#include "TMUtil.h"

static unsigned int sRouletteTextColors[5] = { 0xFFFFFFFF, 0xFF05edda, 0xFFfe9b00, 0xFF950ab2, 0xFFfe0004 };

stItemPack g_LuckBox[100];   
stItemPack g_KeyBox[100];
stItemPack g_PackBox[100];

#define M_PI_2     1.57079632679489661923
#define EF_INFORMATION     203
#define DECAYICON     503
bool isDebug = false;

struct stTemp
{
	char Name[64];
	int Icon;

	struct
	{
		int Color;
		char Text[64];
	} Desc[10];
};

int BASE_GetItemIcon(STRUCT_ITEM* item)
{
	int curIcon = g_itemicon[item->sIndex] - 1;

	int packId = BASE_GetItemAbility(item, EF_INFORMATION);
	auto itemId = item->sIndex;
	int icon = -1;

    if (itemId == 5380)
        icon = g_PackBox[packId].Icon;
    else if (itemId == 5381)
        icon = g_LuckBox[packId].Icon;
    else if (itemId == 5382)
        icon = g_KeyBox[packId].Icon;

	if (icon >= 0)
		curIcon = icon - 1;

	return curIcon;
}

char* BASE_GetItemName(STRUCT_ITEM* item)
{
	auto itemId = item->sIndex;

	static char str[96]{};
	memset(str, 0, sizeof(str));

	int packId = BASE_GetItemAbility(item, EF_INFORMATION);

	if (itemId == 5380)
	{
		strcpy(str, g_PackBox[packId].Name); 
		return str;
	}
	else if (itemId == 5381)
	{
		strcpy(str, g_LuckBox[packId].Name); 
		return str;
	}
	else if (itemId == 5382)
	{
		strcpy(str, g_KeyBox[packId].Name);
		return str;
	}


	return g_pItemList[itemId].Name;
}
     
bool ReadItemPack()
{
	struct stTemp
	{
		char Name[64];
		int Icon;

		struct
		{
			int Color;
			char Text[64];
		} Desc[10];
	};

	// Usando std::array
	std::array<stTemp, 100> p; // Array de 10 elementos usando std::array

	for (int KeyID = 0; KeyID < 10; KeyID++)
	{
		snprintf(p[KeyID].Name, sizeof(p[KeyID].Name), "");  
		p[KeyID].Icon = 0;
		snprintf(p[KeyID].Desc[KeyID].Text, sizeof(p[KeyID].Desc[KeyID].Text), ""); 
		p[KeyID].Desc[KeyID].Color = 0;
	}

	// Copia a memória de p para g_PackBox
	memcpy(&g_PackBox, p.data(), sizeof(stTemp) * 10);  

	return false;
}

bool ReadItemKey()
{
	// Array temporário para armazenar os dados dos itens
	std::array<stTemp, 100> p;

	// Preenchendo os dados manualmente
	snprintf(p[0].Name, sizeof(p[0].Name), "Chave_Good_Luck_I");
	p[0].Icon = 1431;
	p[0].Desc[0] = { -1, "_" };
	p[0].Desc[1] = { -65281, "[Item Premium]" };
	p[0].Desc[2] = { -1, "_" };
	p[0].Desc[3] = { -1, "Use este item para abrir a Caixa Good Luck" };
	p[0].Desc[4] = { -1, "e obter recompensas variadas." };
	p[0].Desc[5] = { 0, "" };
	p[0].Desc[6] = { 0, "" };
	p[0].Desc[7] = { 0, "" };
	p[0].Desc[8] = { 0, "" };
	p[0].Desc[9] = { 0, "" };

	snprintf(p[1].Name, sizeof(p[1].Name), "Chave_Good_Luck_II");
	p[1].Icon = 1432;
	p[1].Desc[0] = { -1, "_" };
	p[1].Desc[1] = { -65281, "[Item Premium]" };
	p[1].Desc[2] = { -1, "_" };
	p[1].Desc[3] = { -1, "Use este item para abrir a Caixa Good Luck" };
	p[1].Desc[4] = { -1, "e obter aleatoriamente" };
	p[1].Desc[5] = { -1, "uma gema do nv. 3." };
	p[1].Desc[6] = { 0, "" };
	p[1].Desc[7] = { 0, "" };
	p[1].Desc[8] = { 0, "" };
	p[1].Desc[9] = { 0, "" };

	snprintf(p[2].Name, sizeof(p[2].Name), "Chave_Good_Luck_III");
	p[2].Icon = 1432;
	p[2].Desc[0] = { -1, "_" };
	p[2].Desc[1] = { -65281, "[Item Premium]" };
	p[2].Desc[2] = { -1, "_" };
	p[2].Desc[3] = { -1, "Use este item para abrir a Caixa Good Luck" };
	p[2].Desc[4] = { -1, "e obter aleatoriamente" };
	p[2].Desc[5] = { -1, "uma gema do nv. 4." };
	p[2].Desc[6] = { 0, "" };
	p[2].Desc[7] = { 0, "" };
	p[2].Desc[8] = { 0, "" };
	p[2].Desc[9] = { 0, "" };

	snprintf(p[3].Name, sizeof(p[3].Name), "Chave_Good_Luck_IV");
	p[3].Icon = 1432;
	p[3].Desc[0] = { -1, "_" };
	p[3].Desc[1] = { -65281, "[Item Premium]" };
	p[3].Desc[2] = { -1, "_" };
	p[3].Desc[3] = { -1, "Use este item para abrir a Caixa Good Luck" };
	p[3].Desc[4] = { -1, "e obter aleatoriamente" };
	p[3].Desc[5] = { -1, "uma gema do nv. 5." };
	p[3].Desc[6] = { 0, "" };
	p[3].Desc[7] = { 0, "" };
	p[3].Desc[8] = { 0, "" };
	p[3].Desc[9] = { 0, "" };

	snprintf(p[4].Name, sizeof(p[4].Name), "Chave_Good_Luck_V");
	p[4].Icon = 1432;
	p[4].Desc[0] = { -1, "_" };
	p[4].Desc[1] = { -65281, "[Item Premium]" };
	p[4].Desc[2] = { -1, "_" };
	p[4].Desc[3] = { -1, "Use este item para abrir a Caixa Good Luck" };
	p[4].Desc[4] = { -1, "e obter aleatoriamente" };
	p[4].Desc[5] = { -1, "uma gema do nv. 6." };
	p[4].Desc[6] = { 0, "" };
	p[4].Desc[7] = { 0, "" };
	p[4].Desc[8] = { 0, "" };
	p[4].Desc[9] = { 0, "" };

	snprintf(p[5].Name, sizeof(p[5].Name), "Chave_do_Bau_de_Natal");
	p[5].Icon = 1436;
	p[5].Desc[0] = { -1, "_" };
	p[5].Desc[1] = { -65281, "[Item Premium de Evento]" };
	p[5].Desc[2] = { -1, "_" };
	p[5].Desc[3] = { -1, "Use esta chave para abrir o Bau de Natal" };
	p[5].Desc[4] = { -1, "e obter aleatoriamente um item natalino." };
	p[5].Desc[5] = { 0, "" };
	p[5].Desc[6] = { 0, "" };
	p[5].Desc[7] = { 0, "" };
	p[5].Desc[8] = { 0, "" };
	p[5].Desc[9] = { 0, "" };

	snprintf(p[6].Name, sizeof(p[6].Name), "Chave_do_Bau_das_Maravilhas");
	p[6].Icon = 947;
	p[6].Desc[0] = { -1, "_" };
	p[6].Desc[1] = { -65281, "[Item Premium de Evento]" };
	p[6].Desc[2] = { -1, "_" };
	p[6].Desc[3] = { -1, "Use esta chave para abrir o Bau das Maravilhas" };
	p[6].Desc[4] = { -1, "e obter aleatoriamente um item." };
	p[6].Desc[5] = { -1, "_" };
	p[6].Desc[6] = { 0, "" };
	p[6].Desc[7] = { 0, "" };
	p[6].Desc[8] = { 0, "" };
	p[6].Desc[9] = { 0, "" };

	snprintf(p[7].Name, sizeof(p[7].Name), "Chave_do_Bau_de_Janeiro");
	p[7].Icon = 1447;
	p[7].Desc[0] = { -1, "_" };
	p[7].Desc[1] = { -65281, "[Item Premium de Evento]" };
	p[7].Desc[2] = { -1, "_" };
	p[7].Desc[3] = { -1, "Use esta chave para abrir o Bau de Janeiro" };
	p[7].Desc[4] = { -1, "e obter aleatoriamente um item." };
	p[7].Desc[5] = { -1, "_" };
	p[7].Desc[6] = { 0, "" };
	p[7].Desc[7] = { 0, "" };
	p[7].Desc[8] = { 0, "" };
	p[7].Desc[9] = { 0, "" };

	snprintf(p[8].Name, sizeof(p[8].Name), "Chave_do_Bau_de_Carnaval");
	p[8].Icon = 1449;
	p[8].Desc[0] = { -1, "_" };
	p[8].Desc[1] = { -65281, "[Item Premium de Evento]" };
	p[8].Desc[2] = { -1, "_" };
	p[8].Desc[3] = { -1, "Use esta chave para abrir o Bau de Carnaval" };
	p[8].Desc[4] = { -1, "e obter aleatoriamente um item." };
	p[8].Desc[5] = { -1, "_" };
	p[8].Desc[6] = { 0, "" };
	p[8].Desc[7] = { 0, "" };
	p[8].Desc[8] = { 0, "" };
	p[8].Desc[9] = { 0, "" };

	snprintf(p[9].Name, sizeof(p[9].Name), "Chave_do_Bau_de_Páscoa");
	p[9].Icon = 1450;
	p[9].Desc[0] = { -1, "_" };
	p[9].Desc[1] = { -65281, "[Item Premium de Evento]" };
	p[9].Desc[2] = { -1, "_" };
	p[9].Desc[3] = { -1, "Use esta chave para abrir o Bau de Páscoa" };
	p[9].Desc[4] = { -1, "e obter aleatoriamente um item." };
	p[9].Desc[5] = { -1, "_" };
	p[9].Desc[6] = { 0, "" };
	p[9].Desc[7] = { 0, "" };
	p[9].Desc[8] = { 0, "" };
	p[9].Desc[9] = { 0, "" };

	// Copiando os dados preenchidos para o array global
	memcpy(g_KeyBox, p.data(), sizeof(g_KeyBox));

	return true;
}

bool ReadItemRoll()
{  
	// Usando std::array
	std::array<stTemp, 100> p; // Array de 10 elementos usando std::array

	// Preenchendo os dados diretamente
	snprintf(p[0].Name, sizeof(p[0].Name), "Caixa_Good_Luck_I");
	p[0].Icon = 1441;
	p[0].Desc[0] = { -1, "_" };
	p[0].Desc[1] = { -65281, "[Item Premium]" };
	p[0].Desc[2] = { -1, "_" };
	p[0].Desc[3] = { -1, "Use este item para abrir a roleta e" };
	p[0].Desc[4] = { -1, "testar sua sorte. Cada uso consome" };
	p[0].Desc[5] = { -1, "uma unidade deste item." };
	p[0].Desc[6] = { 0, "" };
	p[0].Desc[7] = { 0, "" };
	p[0].Desc[8] = { 0, "" };
	p[0].Desc[9] = { 0, "" };

	snprintf(p[1].Name, sizeof(p[1].Name), "Caixa_Good_Luck_II");
	p[1].Icon = 1442;
	p[1].Desc[0] = { -1, "_" };
	p[1].Desc[1] = { -65281, "[Item Premium]" };
	p[1].Desc[2] = { -1, "_" };
	p[1].Desc[3] = { -1, "Use este item para abrir a roleta e" };
	p[1].Desc[4] = { -1, "testar sua sorte. Cada uso consome" };
	p[1].Desc[5] = { -1, "uma unidade deste item." };
	p[1].Desc[6] = { 0, "" };
	p[1].Desc[7] = { 0, "" };
	p[1].Desc[8] = { 0, "" };
	p[1].Desc[9] = { 0, "" };

	snprintf(p[2].Name, sizeof(p[2].Name), "Caixa_Good_Luck_III");
	p[2].Icon = 1442;
	p[2].Desc[0] = { -1, "_" };
	p[2].Desc[1] = { -65281, "[Item Premium]" };
	p[2].Desc[2] = { -1, "_" };
	p[2].Desc[3] = { -1, "Use este item para abrir a roleta e" };
	p[2].Desc[4] = { -1, "testar sua sorte. Cada uso consome" };
	p[2].Desc[5] = { -1, "uma unidade deste item." };
	p[2].Desc[6] = { 0, "" };
	p[2].Desc[7] = { 0, "" };
	p[2].Desc[8] = { 0, "" };
	p[2].Desc[9] = { 0, "" };

	snprintf(p[3].Name, sizeof(p[3].Name), "Caixa_Good_Luck_IV");
	p[3].Icon = 1442;
	p[3].Desc[0] = { -1, "_" };
	p[3].Desc[1] = { -65281, "[Item Premium]" };
	p[3].Desc[2] = { -1, "_" };
	p[3].Desc[3] = { -1, "Use este item para abrir a roleta e" };
	p[3].Desc[4] = { -1, "testar sua sorte. Cada uso consome" };
	p[3].Desc[5] = { -1, "uma unidade deste item." };
	p[3].Desc[6] = { 0, "" };
	p[3].Desc[7] = { 0, "" };
	p[3].Desc[8] = { 0, "" };
	p[3].Desc[9] = { 0, "" };

	snprintf(p[4].Name, sizeof(p[4].Name), "Caixa_Good_Luck_V");
	p[4].Icon = 1442;
	p[4].Desc[0] = { -1, "_" };
	p[4].Desc[1] = { -65281, "[Item Premium]" };
	p[4].Desc[2] = { -1, "_" };
	p[4].Desc[3] = { -1, "Use este item para abrir a roleta e" };
	p[4].Desc[4] = { -1, "testar sua sorte. Cada uso consome" };
	p[4].Desc[5] = { -1, "uma unidade deste item." };
	p[4].Desc[6] = { 0, "" };
	p[4].Desc[7] = { 0, "" };
	p[4].Desc[8] = { 0, "" };
	p[4].Desc[9] = { 0, "" };

	snprintf(p[5].Name, sizeof(p[5].Name), "Bau_de_Natal");
	p[5].Icon = 1428;
	p[5].Desc[0] = { -16655871, "A magia do Natal esta no ar! Esse bau" };
	p[5].Desc[1] = { -16655871, "reune todos os itens festivos natalinos de " };
	p[5].Desc[2] = { -16655871, "todos os anos dentro dele, incluindo itens " };
	p[5].Desc[3] = { -16655871, "novos excluvisos deste ano! Sendo trajes de " };
	p[5].Desc[4] = { -16655871, "personagem, skin de montaria, montarias " };
	p[5].Desc[5] = { -16655871, "temporarias e armas com tematica natalina." };
	p[5].Desc[6] = { 0, "" };
	p[5].Desc[7] = { 0, "" };
	p[5].Desc[8] = { 0, "" };
	p[5].Desc[9] = { 0, "" };

	snprintf(p[6].Name, sizeof(p[6].Name), "Bau_das_Maravilhas");
	p[6].Icon = 1437;
	p[6].Desc[0] = { -1, "______________________" };
	p[6].Desc[1] = { -1, "Bau repleto de maravilhas!" };
	p[6].Desc[2] = { -1, "______________________" };
	p[6].Desc[3] = { -1, "Teste sua sorte ao abrir o bau e veja" };
	p[6].Desc[4] = { -1, "quais maravilhas te aguardam!" };
	p[6].Desc[5] = { -1, "_" };
	p[6].Desc[6] = { 0, "" };
	p[6].Desc[7] = { 0, "" };
	p[6].Desc[8] = { 0, "" };
	p[6].Desc[9] = { 0, "" };

	snprintf(p[7].Name, sizeof(p[7].Name), "Bau_de_Janeiro");
	p[7].Icon = 1440;
	p[7].Desc[0] = { -1, "Bau cheio de itens exclusivos para voce" };
	p[7].Desc[1] = { -1, "começar o ano e o verao no melhor estilo!" };
	p[7].Desc[2] = { -1, "_" };
	p[7].Desc[3] = { -1, "Use este item para abrir a roleta e testar" };
	p[7].Desc[4] = { -1, "sua sorte. Cada uso consome uma unidade" };
	p[7].Desc[5] = { -1, "deste item. Necessita da chave para abrir." };
	p[7].Desc[6] = { 0, "" };
	p[7].Desc[7] = { 0, "" };
	p[7].Desc[8] = { 0, "" };
	p[7].Desc[9] = { 0, "" };

	snprintf(p[8].Name, sizeof(p[8].Name), "Bau_de_Carnaval");
	p[8].Icon = 1448;
	p[8].Desc[0] = { -1, "Bau cheio de itens exclusivos para voce" };
	p[8].Desc[1] = { -1, "aproveitar o Carnaval na maior festa!" };
	p[8].Desc[2] = { -1, "_" };
	p[8].Desc[3] = { -1, "Use este item para abrir a roleta e testar" };
	p[8].Desc[4] = { -1, "sua sorte. Cada uso consome uma unidade" };
	p[8].Desc[5] = { -1, "deste item. Necessita da chave para abrir." };
	p[8].Desc[6] = { 0, "" };
	p[8].Desc[7] = { 0, "" };
	p[8].Desc[8] = { 0, "" };
	p[8].Desc[9] = { 0, "" };

	snprintf(p[9].Name, sizeof(p[9].Name), "Bau_de_Fevreiro");
	p[9].Icon = 1439;
	p[9].Desc[0] = { -1, "Bau cheio de itens de ferreiro! Utilize e" };
	p[9].Desc[1] = { -1, "comece seu caminho para se tornar um grande" };
	p[9].Desc[2] = { -1, "ferreiro!" };
	p[9].Desc[3] = { -1, "Use este item para abrir a roleta e testar" };
	p[9].Desc[4] = { -1, "sua sorte. Cada uso consome uma unidade" };
	p[9].Desc[5] = { -1, "deste item. Necessita da chave para abrir." };
	p[9].Desc[6] = { 0, "" };
	p[9].Desc[7] = { 0, "" };
	p[9].Desc[8] = { 0, "" };
	p[9].Desc[9] = { 0, "" };

	// Copiando os dados do array temporário para g_LuckBox
	memcpy(g_LuckBox, p.data(), sizeof(g_LuckBox));

	return true;
}


int RouletteSystem::OnKeyVisibleRoleta(char iCharCode, int lParam)
{
	return 0;
	if (iCharCode != 't' && iCharCode != 'T')
		return 0;

	auto pFScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	 
	if (m_pRoulettePanel)
	{

		MSG_MessageWhisper stWhisper{};

		stWhisper.Header.ID = g_pObjectManager->m_dwCharID;
		stWhisper.Header.Type = MSG_MessageWhisper_Opcode;

		sprintf_s(stWhisper.MobName, "roleta");

		SendOneMessage((char*)&stWhisper, sizeof(stWhisper));
		 
		m_pRoulettePanel->SetVisible(m_pRoulettePanel->m_bVisible == 0);
	  
	} 
	return 1;
}
void RouletteSystem::Initialize(SControlContainer* pControlContainer)
{
	m_pControlContainer = pControlContainer;
	m_pRoulettePanel = (SPanel*)pControlContainer->FindControl(82443);

	m_pRoulettePanel->SetVisible(0);
	 
	m_pRouletteBoxGrid = (SGridControl*)pControlContainer->FindControl(82449);
	m_pRouletteRewGrid = (SGridControl*)pControlContainer->FindControl(82450);
	m_pRouletteKeyGrid = (SGridControl*)pControlContainer->FindControl(82477);
	m_pRouletteBoxName = (SText*)pControlContainer->FindControl(82464);
	m_pRouletteBoxDesc = (SText*)pControlContainer->FindControl(82476);
	m_pRouletteProgressDesc = (SText*)pControlContainer->FindControl(82478);
	m_pRouletteProgressBar = (SProgressBar*)pControlContainer->FindControl(82446);
	m_pRouletteItemPanel = (SPanel*)pControlContainer->FindControl(82479);

	for (int i = 0; i < MAX_ROULETTE_ITEMS; i++)
	{
		m_pRouletteDescGrids[i] = (SGridControl*)m_pControlContainer->FindControl(84030 + i);
		m_pRouletteItems[i] = (SPanel*)m_pControlContainer->FindControl(82480 + i);
		m_pRouletteDescItems[i] = (SPanel*)m_pControlContainer->FindControl(82451 + i);
		m_pRouletteRareBorders[i] = (SPanel*)m_pControlContainer->FindControl(82465 + i);
	}

	m_Timer = 8000;
	 
}

bool RouletteSystem::OnPacketOpenRoulette(MSG_SendRollInfo* pRollInfo)
{

	m_pRouletteBoxGrid->Empty();
	m_pRouletteKeyGrid->Empty();
	m_pRouletteRewGrid->Empty();

	m_pRouletteBoxGrid->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	m_pRouletteKeyGrid->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;
	m_pRouletteRewGrid->m_eGridType = TMEGRIDTYPE::GRID_SKILLB;

	STRUCT_ITEM box { 5381, EF_INFORMATION, static_cast<unsigned char>(pRollInfo->BoxId) };
	m_pRouletteBoxGrid->SetItemOnGrid(&box, 0, 0);
	  


	m_pRouletteBoxName->SetText(g_LuckBox[pRollInfo->BoxId].Name, 0);
	m_pRouletteBoxDesc->SetTextColor(0xFFCCCCCC);

	if(CanRollRoulette(pRollInfo->KeyId, pRollInfo->BoxId))
		m_pRouletteBoxDesc->SetText((char*)"Pronto para abrir...", 0);
	else
		m_pRouletteBoxDesc->SetText((char*)"Não possui a chave", 0);

	SetRouletteProgressBar(pRollInfo->Evol);

	for (int i = 0; i < MAX_ROULETTE_ITEMS; i++)
	{
		m_pRouletteItems[i]->SetVisible(0);

		m_pRouletteDescGrids[i]->Empty();
		m_pRouletteDescGrids[i]->m_eGridType = TMEGRIDTYPE::GRID_SKILLB; 

		if (pRollInfo->Items[i].sIndex > 0)
			m_pRouletteDescGrids[i]->SetItemOnGrid(&pRollInfo->Items[i], 0, 0);
	}

	TMFieldScene* pScene = static_cast<TMFieldScene*>(g_pCurrentScene);
	pScene->m_pInvenPanel->SetVisible(0);

	m_pRoulettePanel->SetVisible(1);
	return true;
}

bool RouletteSystem::OnPacketUseRoulette(MSG_RollItem* pRollItem)
{
	auto pMobData = &g_pObjectManager->m_stMobData;

	m_SelectedItem = pRollItem->Slot;

	for (int i = 0; i < MAX_ROULETTE_POSSIBLE_ITEMS; i++)
	{ 
		m_ItemIcons[i] = BASE_GetItemIcon(&pRollItem->Items[i]);
		m_RareBorders[i] = pRollItem->Rares[i] - 1;

		m_Rarities[i] = pRollItem->Rares[i];
		memcpy(&m_Items[i], &pRollItem->Items[i], sizeof(STRUCT_ITEM));
	}

	for (int i = 0; i < MAX_ROULETTE_ITEMS; i++)
	{
		if (m_pRouletteItems[i] == nullptr)
			continue;

		m_pRouletteItems[i]->SetVisible(1);

		m_pRouletteDescItems[i]->m_GCPanel.nTextureIndex = GetItemIcon();		
		m_pRouletteRareBorders[i]->m_GCPanel.nTextureIndex = GetRareBorder(m_pRouletteDescItems[i]->m_GCPanel.nTextureIndex);
	}

	SetRouletteProgressBar(pRollItem->Evol);

	m_pRouletteRewGrid->Empty();

	m_pRouletteBoxDesc->SetText((char*)"Sorteando...", 0);
	m_pRouletteBoxDesc->SetTextColor(Grey);

	if (pRollItem->Evol == 100)
	{
		m_pRouletteBoxName->SetText(strfmt("[Super Drop] %s", g_LuckBox[pRollItem->Level].Name), 0);
		m_pRouletteBoxName->SetTextColor(Yellow);
	}
	else
	{
		m_pRouletteBoxName->SetText(g_LuckBox[pRollItem->Level].Name, 0);
		m_pRouletteBoxName->SetTextColor(White);
	}

	m_LastItem = m_SelectedItem;
	m_ItemValues.fill(0);

	m_Level = 0;
	m_StartTime = timeGetTime();
	m_IsRolling = true;
	return true;
}

void RouletteSystem::OnTimer()
{
	auto currentTime = timeGetTime();
	 

	if (!m_IsRolling)
		return;

	auto startInterval = (currentTime - m_StartTime);
	 

	float progress = (float)startInterval / (float)m_Timer;
	m_pRouletteBoxDesc->SetText(strfmt("Sorteando em... %d.%ds", startInterval / 1000, (startInterval % 1000) / 100), 0);
	
	if (progress > 1.0f)
	{
		progress = 1.0f;
		SetDropRoll(progress);

		m_IsRolling = false;
		m_StartTime = 0;

		m_pRouletteBoxDesc->SetText(BASE_GetItemName(&m_Items[m_SelectedItem]), 0);
		m_pRouletteBoxDesc->SetTextColor(sRouletteTextColors[m_Rarities[m_SelectedItem] - 1]);

		m_pRouletteItems[ROULETTE_CENTER_ITEM_INDEX]->SetVisible(0);

		GetSoundAndPlay(43, 0, 0);

		m_pRouletteRewGrid->SetItemOnGrid(&m_Items[m_SelectedItem], 0, 0);

		MSG_STANDARD packet{};
		packet.Size = 12;
		packet.Type = 0xF07;
		packet.ID = g_pCurrentScene->m_dwID;
		SendOneMessage((char*)&packet, 12);
		return;
	}

	for(int i = 0; i < 10; i++)
		SetDropRoll(progress);
}

void RouletteSystem::SetDropRoll(float progress)
{
	if (progress > 1.0f)
		progress = 1.0f;

	const float size = m_pRouletteItems[0]->m_nWidth;
	const float off = pow(sinf(progress * static_cast<float>(M_PI_2)), 2.6f) * size * ROULETTE_LENGTH;
	const float width = size * MAX_ROULETTE_ITEMS;

	for (int i = 0; i < MAX_ROULETTE_ITEMS; i++)
	{
		const float base = (i + 1) * size - off;
		float index = -floor(base / width);
		const float value = ((((int)base % (int)width) + (int)width) % (int)width) - size;

		m_pRouletteItems[i]->m_nPosX = value;

		if (m_ItemValues[i] != index)
		{
			m_Level += (int)index - m_ItemValues[i];
			m_ItemValues[i] = index;

			if (m_Level == ROULETTE_LENGTH - 5)
			{
				m_pRouletteDescItems[i]->m_GCPanel.nTextureIndex = GetItemIcon(m_LastItem);
				m_pRouletteRareBorders[i]->m_GCPanel.nTextureIndex = GetRareBorder(m_pRouletteDescItems[i]->m_GCPanel.nTextureIndex);
			}
			else
			{
				m_pRouletteDescItems[i]->m_GCPanel.nTextureIndex = GetItemIcon();
				m_pRouletteRareBorders[i]->m_GCPanel.nTextureIndex = GetRareBorder(m_pRouletteDescItems[i]->m_GCPanel.nTextureIndex);
			}
		}
	}
}

int RouletteSystem::GetItemIcon(int id)
{
	if (id != -1)
		return m_ItemIcons[id];

	return m_ItemIcons[rand() % MAX_ROULETTE_POSSIBLE_ITEMS];
}

int RouletteSystem::GetRareBorder(int id)
{
	for (int i = 0; i < MAX_ROULETTE_POSSIBLE_ITEMS; i++)
	{
		if (m_ItemIcons[i] == id)
		{
			return m_RareBorders[i];
		}
	}

	return 0;
}

bool RouletteSystem::OnRouletteControlEvent(unsigned int idwControlID, unsigned int idwEvent)
{
	auto pMobData = &g_pObjectManager->m_stMobData;

	if (idwControlID == 82448)
	{
		OnCloseRouletteEvent();
		return true;
	}
	else if (idwControlID == 82447)
	{
		OnRollRouletteEvent();
		return true;
	}

	return false;
}

void RouletteSystem::OnCloseRouletteEvent()
{
	MSG_STANDARD packet{};
	packet.Type = 0xF07;
	packet.Size = sizeof(packet);
	packet.ID = g_pCurrentScene->m_dwID;
	SendOneMessage((char*)&packet, sizeof(packet));

	m_IsRolling = false;
	m_StartTime = 0;
	m_pRoulettePanel->SetVisible(0);
}

void RouletteSystem::OnRollRouletteEvent()
{
	if (m_IsRolling)
		return;

	MSG_STANDARD packet{};
	packet.Size = sizeof(packet);
	packet.Type = 0xF06;
	packet.ID = g_pCurrentScene->m_dwID;
	SendOneMessage((char*)&packet, sizeof packet);
}

bool RouletteSystem::IsRouletteVisible()
{
	return m_pRoulettePanel->IsVisible() == 1;
}

bool RouletteSystem::CanRollRoulette(int keyId, int boxId)
{
	auto pMobData = &g_pObjectManager->m_stMobData;

	if (keyId < 0)
		return true;

	STRUCT_ITEM key = { 5382, EF_INFORMATION, static_cast<unsigned char>(keyId) };
	m_pRouletteKeyGrid->SetItemOnGrid(&key, 0, 0);

	int count = 0;
	for (int i = 0; i < 60; i++)
	{
		if (pMobData->Carry[i].sIndex == 5382)
		{
			int boxType = BASE_GetEffectValue(&pMobData->Carry[i], EF_INFORMATION);
			if (boxType == boxId)
				return true;
		}
	}

	return false;
}

void RouletteSystem::SetRouletteProgressBar(int evol)
{
	if (evol >= 0)
	{
		m_pRouletteProgressBar->SetVisible(1);
		m_pRouletteProgressBar->SetMaxProgress(100);
		m_pRouletteProgressBar->SetCurrentProgress(evol);
		m_pRouletteProgressDesc->SetText(strfmt("( %d / 100 )", evol), 0);
	}
	else
	{
		m_pRouletteProgressBar->SetVisible(0);
		m_pRouletteProgressDesc->SetText((char*)"", 0);
	}
}
