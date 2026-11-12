// Explicit unused DB transport boundary for the isolated item lifecycle suite.
// Actual CLIENT_DESC framing/buffers are linked by item_persistence_regression.
#include "../../game/src/desc_client.h"
#include <cstdlib>
CLIENT_DESC* db_clientdesc=0;
void CLIENT_DESC::DBPacketHeader(BYTE,DWORD,DWORD) { std::abort(); }
void CLIENT_DESC::Packet(const void*,int) { std::abort(); }

void CLIENT_DESC::DBPacket(BYTE,DWORD,const void*,DWORD) { std::abort(); }
