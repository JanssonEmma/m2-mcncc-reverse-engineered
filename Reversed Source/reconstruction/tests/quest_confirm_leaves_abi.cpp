#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#include "../../game/src/packet.h"
#include <cstddef>
void ConfirmLeavesAPI(){void(CHARACTER::*message)(const char*,int,DWORD)=&CHARACTER::ConfirmWithMsg;void(quest::CQuestManager::*style)(int)=&quest::CQuestManager::SetSkinStyle;(void)message;(void)style;}
typedef char confirm_long4[sizeof(long)==4?1:-1];
typedef char confirm_DWORD4[sizeof(DWORD)==4?1:-1];
typedef char confirm_packet74[sizeof(TPacketGCQuestConfirm)==74?1:-1];
typedef char confirm_header0[offsetof(TPacketGCQuestConfirm,header)==0?1:-1];
typedef char confirm_message1[offsetof(TPacketGCQuestConfirm,msg)==1?1:-1];
typedef char confirm_message65[sizeof(((TPacketGCQuestConfirm*)0)->msg)==65?1:-1];
typedef char confirm_timeout66[offsetof(TPacketGCQuestConfirm,timeout)==66?1:-1];
typedef char confirm_request70[offsetof(TPacketGCQuestConfirm,requestPID)==70?1:-1];
