#pragma once

#include "TMScene.h"
#include "SGrid.h"

struct MSG_STANDARD;

int PatternOnPacketEvent(ESCENE_TYPE scene, MSG_STANDARD* header);
int PatternOnControlEvent(ESCENE_TYPE scene, unsigned int idwControlID, unsigned int idwEvent);
int PatternOnClickGrid(ESCENE_TYPE scene, int PanelId, int CellX, int CellY, TMEGRIDTYPE GridType, int ControlId);
int PatternOnCharEvent(ESCENE_TYPE scene, char iCharCode, int lParam);


int InitializeNewFiles();
