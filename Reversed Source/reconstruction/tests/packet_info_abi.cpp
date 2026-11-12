#include "../../game/src/packet_info.h"
#include <cstddef>
#define ABI(n,e) typedef char n[(e)?1:-1]
ABI(element_size,sizeof(TPacketElement)==20);
ABI(element_size_offset,offsetof(TPacketElement,iSize)==0);
ABI(element_name_offset,offsetof(TPacketElement,stName)==4);
ABI(element_calls_offset,offsetof(TPacketElement,iCalled)==8);
ABI(element_load_offset,offsetof(TPacketElement,dwLoad)==12);
ABI(element_sequence_offset,offsetof(TPacketElement,bSequencePacket)==16);
// CPacketInfo's GCC3.3 std::map representation is NOT restored by string ABI flags.
// Do not assert target class size24 under a modern library.

// These relative checks confirm no new state, not equality with GCC3.3 class24.
ABI(cg_has_no_added_state,sizeof(CPacketInfoCG)==sizeof(CPacketInfo));
ABI(gg_has_no_added_state,sizeof(CPacketInfoGG)==sizeof(CPacketInfo));
ABI(udp_has_no_added_state,sizeof(CPacketInfoUDP)==sizeof(CPacketInfo));
