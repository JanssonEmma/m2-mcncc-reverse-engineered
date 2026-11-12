// Stage under reconstruction/tests after candidate integration. Not built here.
#include "../../common/length.h"
#include "../../game/src/packet.h"
#include <cstddef>
#define TARGET_WALK_ASSERT(name,expr) typedef char target_walk_assert_##name[(expr)?1:-1]
TARGET_WALK_ASSERT(byte1,sizeof(BYTE)==1);
TARGET_WALK_ASSERT(dword4,sizeof(DWORD)==4);
TARGET_WALK_ASSERT(int4,sizeof(int)==4 && sizeof(unsigned int)==4);
TARGET_WALK_ASSERT(target6,sizeof(packet_target)==6 && sizeof(TPacketGCTarget)==6);
TARGET_WALK_ASSERT(target_header0,offsetof(packet_target,header)==0);
TARGET_WALK_ASSERT(target_vid1,offsetof(packet_target,dwVID)==1);
TARGET_WALK_ASSERT(target_hp5,offsetof(packet_target,bHPPercent)==5);
TARGET_WALK_ASSERT(walk6,sizeof(SPacketGCWalkMode)==6 && sizeof(TPacketGCWalkMode)==6);
TARGET_WALK_ASSERT(walk_header0,offsetof(SPacketGCWalkMode,header)==0);
TARGET_WALK_ASSERT(walk_vid1,offsetof(SPacketGCWalkMode,vid)==1);
TARGET_WALK_ASSERT(walk_mode5,offsetof(SPacketGCWalkMode,mode)==5);
TARGET_WALK_ASSERT(walk_modes,WALKMODE_RUN==0 && WALKMODE_WALK==1);
TARGET_WALK_ASSERT(character_pc6,CHAR_TYPE_PC==6 && sizeof(ECharType)==4);
