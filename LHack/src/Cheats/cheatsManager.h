#pragma once

#include "../Hook/hookManager.h"
#include "../Memory/memory.h"
#include "../Gui/gui.h"

class CheatsManager
{
public:
	VOID CreateFly(BOOL Enable);
	VOID NoKickBack(BOOL Enable);
};

CheatsManager inline* cheatsManager = new CheatsManager();