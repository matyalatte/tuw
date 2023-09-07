#pragma once
#include "ui.h"

uintptr_t SpinboxSetTooltip(uiSpinbox* c, const char* text);
uintptr_t ControlSetTooltip(uiControl* c, const char* text);
void DestroyTooltip(uintptr_t tooltip);
