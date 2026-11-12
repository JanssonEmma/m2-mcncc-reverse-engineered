#ifndef MCNCC_GAME_EMPIRE_TEXT_CONVERT_H
#define MCNCC_GAME_EMPIRE_TEXT_CONVERT_H
#include "../../libthecore/include/typedef.h"

bool LoadEmpireTextConvertTable(DWORD empire, const char* filename);
void ConvertEmpireText(DWORD empire, char* text, int percent);
#endif
