// Stage under reconstruction/tests only after integrating candidates. Not built here.
#include "../../common/tables.h"
#include "../../game/src/packet.h"
#include <cstddef>
#include <climits>
#define PRIMITIVE_ASSERT(name,expr) typedef char primitive_assert_##name[(expr)?1:-1]
PRIMITIVE_ASSERT(byte1,sizeof(BYTE)==1);
PRIMITIVE_ASSERT(int32,sizeof(int)==4);
PRIMITIVE_ASSERT(int_signed,INT_MIN==(-2147483647-1) && INT_MAX==2147483647);
PRIMITIVE_ASSERT(long32,sizeof(long)==4);
PRIMITIVE_ASSERT(dword4,sizeof(DWORD)==4);
PRIMITIVE_ASSERT(skill6,sizeof(SPlayerSkill)==6 && sizeof(TPlayerSkill)==6);
PRIMITIVE_ASSERT(skill_master0,offsetof(SPlayerSkill,bMasterType)==0);
PRIMITIVE_ASSERT(skill_level1,offsetof(SPlayerSkill,bLevel)==1);
PRIMITIVE_ASSERT(skill_next2,offsetof(SPlayerSkill,tNextRead)==2);
PRIMITIVE_ASSERT(skill_time4,sizeof(((SPlayerSkill*)0)->tNextRead)==4);
PRIMITIVE_ASSERT(point_packet17,sizeof(packet_point_change)==17 && sizeof(TPacketGCPointChange)==17);
PRIMITIVE_ASSERT(packet_header0,offsetof(packet_point_change,header)==0);
PRIMITIVE_ASSERT(packet_header4,sizeof(((packet_point_change*)0)->header)==4);
PRIMITIVE_ASSERT(packet_vid4,offsetof(packet_point_change,dwVID)==4);
PRIMITIVE_ASSERT(packet_type8,offsetof(packet_point_change,type)==8);
PRIMITIVE_ASSERT(packet_amount9,offsetof(packet_point_change,amount)==9);
PRIMITIVE_ASSERT(packet_value13,offsetof(packet_point_change,value)==13);
