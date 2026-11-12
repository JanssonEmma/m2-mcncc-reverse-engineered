#include "char.h"
#include "char_manager.h"
LPCHARACTER CHARACTER::GetVictim() { return CHARACTER_MANAGER::instance().Find(m_kVIDVictim); }
