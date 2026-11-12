from pathlib import Path
p=Path('work/party_dependency')
s=(p/'packets.h.additions.candidate').read_text();db=s[s.index('struct SPacket'):s.index('struct packet_party_update')];gc=s[s.index('struct packet_party_update'):s.index('#pragma pack(pop)')]
(p/'common_packet.h.candidate').write_text('#ifndef MCNCC_COMMON_PACKET_H\n#define MCNCC_COMMON_PACKET_H\n#include "../libthecore/include/typedef.h"\n#pragma pack(push,1)\n'+db+'#pragma pack(pop)\n#endif\n')
(p/'game_packet.h.additions.candidate').write_text('#pragma pack(push,1)\n'+gc+'#pragma pack(pop)\n')
s=(p/'party.cpp.candidate').read_text().replace('#include "packet.h"','#include "packet.h"\n#include "../../common/packet.h"');(p/'party.cpp.candidate').write_text(s)
with (p/'README.md').open('a') as f:f.write('\nPacket placement split: common_packet.h.candidate is a proposed NEW common/packet.h (currently absent); game_packet.h.additions.candidate adds the client record to existing game/src/packet.h. party.cpp.candidate includes both. party_abi.cpp.candidate stages nested-record/packet/signature checks, deliberately does not assert160 bytes against modern STL. No ABI checks were executed.\n')
