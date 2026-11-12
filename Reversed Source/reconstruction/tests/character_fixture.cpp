// Test-only CHARACTER destructor and remaining virtual boundaries.
// They do not claim recovery of the original lifecycle/virtual behavior.
#include "../../game/src/char.h"
#include "../../game/src/mob_manager.h"
#include <cstdlib>
#include <cstring>


CHARACTER::~CHARACTER() { CEntity::Destroy(); }
void CHARACTER::EncodeInsertPacket(CEntity *) { std::abort(); }
void CHARACTER::EncodeRemovePacket(CEntity *) { std::abort(); }
void CHARACTER::StateMove() { std::abort(); }
void CHARACTER::StateBattle() { std::abort(); }
void CHARACTER::StateIdle() { std::abort(); }
void CHARACTER::StateFlag() { std::abort(); }
void CHARACTER::StateFlagBase() { std::abort(); }
void CHARACTER::SetHorseLevel(int ) { std::abort(); }
bool CHARACTER::StartRiding() { std::abort(); }
bool CHARACTER::StopRiding() { std::abort(); }
DWORD CHARACTER::GetMyHorseVnum() { std::abort(); }
void CHARACTER::HorseDie() { std::abort(); }
bool CHARACTER::ReviveHorse() { std::abort(); }
void CHARACTER::SendHorseInfo() { std::abort(); }
void CHARACTER::ClearHorseInfo() { std::abort(); }

#include "../../game/src/sectree.h"
#include <cstdarg>
SECTREE::~SECTREE() {} // Explicit test-only sector ownership boundary.


