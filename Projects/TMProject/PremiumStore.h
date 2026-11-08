#pragma once

#include "SGrid.h"

constexpr int CashShopInfoPacket = 0xAF1;
constexpr int reqCashShop = 0xAF3;

struct MSG_STANDARD;
int PremiumStoreOnPacketEvent(MSG_STANDARD* header);
int PremiumStoreOnControlEvent(unsigned int idwControlID, unsigned int idwEvent);
int PremiumStoreOnClickGrid(int InvSlot, TMEGRIDTYPE GridType, int PanelId);
void ReqPremiumStore();


