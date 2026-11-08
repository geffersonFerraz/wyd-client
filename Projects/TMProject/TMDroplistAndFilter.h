#pragma once

#include "SGrid.h"
 

struct MSG_STANDARD;
int DropOnPacketEvent(MSG_STANDARD* header);
int DropOnControlEvent(unsigned int idwControlID, unsigned int idwEvent);
int DropOnClickGrid(int InvSlot, TMEGRIDTYPE GridType, int PanelId);
void DropSetCurrentPosSize(float x, float y, float w, float h);

void ReqDropList(); 
void ReqDropListZone(int indicex, int indicey);
