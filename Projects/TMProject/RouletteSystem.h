#pragma once

#include "Basedef.h"

class SControlContainer;
class SPanel;
class SGridControl;
class SText;
class SProgressBar;

constexpr auto ROULETTE_CENTER_ITEM_INDEX = 8;
constexpr auto ROULETTE_LENGTH = 80;
constexpr auto MAX_ROULETTE_ITEMS = 11;

extern stItemPack g_LuckBox[100];
extern stItemPack g_KeyBox[100];
extern stItemPack g_PackBox[100];
class RouletteSystem
{
public:
	void Initialize(SControlContainer* pControlContainer);

	int OnKeyVisibleRoleta(char iCharCode, int lParam);
	bool OnPacketOpenRoulette(MSG_SendRollInfo* pRollInfo);
	bool OnPacketUseRoulette(MSG_RollItem* pRollItem);

	bool OnRouletteControlEvent(unsigned int idwControlID, unsigned int idwEvent);
	bool IsRouletteVisible();
	void OnTimer();

private:
	void OnCloseRouletteEvent();
	void OnRollRouletteEvent();

	bool CanRollRoulette(int keyId, int boxId);
	void SetRouletteProgressBar(int evol);

	void SetDropRoll(float progress);

	int GetItemIcon(int id = -1);
	int GetRareBorder(int id); 
private:
	SControlContainer* m_pControlContainer;

	SPanel* m_pRoulettePanel;

	SGridControl* m_pRouletteBoxGrid;
	SGridControl* m_pRouletteRewGrid;
	SGridControl* m_pRouletteKeyGrid;

	SText* m_pRouletteBoxName;
	SText* m_pRouletteBoxDesc;
	SText* m_pRouletteProgressDesc;

	SProgressBar* m_pRouletteProgressBar;

	SPanel* m_pRouletteItemPanel;
	SGridControl* m_pRouletteDescGrids[MAX_ROULETTE_ITEMS];
	SPanel* m_pRouletteItems[MAX_ROULETTE_ITEMS];
	SPanel* m_pRouletteDescItems[MAX_ROULETTE_ITEMS];
	SPanel* m_pRouletteRareBorders[MAX_ROULETTE_ITEMS];
	
	unsigned int m_Timer;
	unsigned int m_StartTime;

	std::array<STRUCT_ITEM, MAX_ROULETTE_POSSIBLE_ITEMS> m_Items;
	std::array<int, MAX_ROULETTE_POSSIBLE_ITEMS> m_Rarities;

	std::array<int, MAX_ROULETTE_POSSIBLE_ITEMS> m_ItemValues;
	std::array<int, MAX_ROULETTE_POSSIBLE_ITEMS> m_ItemIcons;
	std::array<int, MAX_ROULETTE_POSSIBLE_ITEMS> m_RareBorders;

	bool m_IsRolling;
	int m_SelectedItem;
	int m_LastItem;
	int m_Level;
};