
#include "../../game/src/char.h"
#include "../../game/src/constants.h"
typedef void(CHARACTER::*PolymorphSignature)(DWORD,bool);
typedef void(CHARACTER::*MountSignature)(DWORD);
typedef void(CHARACTER::*ComputeSignature)();
typedef bool(CHARACTER::*StopRidingSignature)();
PolymorphSignature exact_polymorph=&CHARACTER::SetPolymorph;
MountSignature exact_mount=&CHARACTER::MountVnum;
ComputeSignature exact_compute=&CHARACTER::ComputePoints;
ComputeSignature exact_skill_compute=&CHARACTER::ComputeSkillPoints;
StopRidingSignature exact_stop=&CHARACTER::StopRiding;
typedef char TargetInt32[sizeof(int)==4?1:-1];
typedef char TargetDWORD32[sizeof(DWORD)==4?1:-1];
typedef char JobRecord64[sizeof(TJobInitialPoints)==64?1:-1];
typedef char HorseRecord48[sizeof(THorseStat)==48?1:-1];
// No obsolete whole STL-containing class-size gate against a modern compiler.
