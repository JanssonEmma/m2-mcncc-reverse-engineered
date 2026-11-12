// TEST ONLY: shared pre-existing full-data guild lifecycle boundary.
#include "../../game/src/guild.h"
#include <cstring>
CGuild::CGuild(DWORD id) { std::memset(&m_data,0,sizeof(m_data)); m_data.guild_id=id; }
CGuild::~CGuild() {}
