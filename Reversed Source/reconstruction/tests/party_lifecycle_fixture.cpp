// TEST ONLY. Full original CParty object; no production lifecycle replacement.
// This intentionally makes no claim to recover Destroy/destructor/Join behavior.
#include <map>
#include <string>
#define private public
#define protected public
#include "../../game/src/party.h"
#undef protected
#undef private
#include <cassert>
#include <cstdlib>
CParty::~CParty() {
    // Fixture must explicitly dismantle all live relationships before scope exit.
    assert(m_memberMap.empty() && m_map_iFlag.empty());
    assert(!m_eventUpdate && !m_bPCParty && !m_pkChrLeader && !m_pkChrExpCentralize && !m_pkDungeon);
    // Real std::map/string destructors run automatically after this boundary.
}
void CParty::Join(DWORD) { std::abort(); } // Required virtual slot; never exercised.
