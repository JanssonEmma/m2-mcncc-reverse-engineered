#define protected public
#include "../../game/src/desc.h"
#undef protected
#include <cstddef>
typedef char player_size[(sizeof(TSimplePlayer)==63)?1:-1];
typedef char account_size[(sizeof(TAccountTable)==333)?1:-1];
typedef char string_size[(sizeof(std::string)==4)?1:-1];
typedef char descriptor_size[(sizeof(DESC)==580)?1:-1];
typedef char offset_m_pInputProcessor[(offsetof(DESC,m_pInputProcessor)==4)?1:-1];
typedef char offset_m_lpFdw[(offsetof(DESC,m_lpFdw)==8)?1:-1];
typedef char offset_m_sock[(offsetof(DESC,m_sock)==12)?1:-1];
typedef char offset_m_iPhase[(offsetof(DESC,m_iPhase)==16)?1:-1];
typedef char offset_m_dwHandle[(offsetof(DESC,m_dwHandle)==20)?1:-1];
typedef char offset_m_stHost[(offsetof(DESC,m_stHost)==24)?1:-1];
typedef char offset_m_wPort[(offsetof(DESC,m_wPort)==28)?1:-1];
typedef char offset_m_LastTryTime[(offsetof(DESC,m_LastTryTime)==32)?1:-1];
typedef char offset_m_lpInputBuffer[(offsetof(DESC,m_lpInputBuffer)==36)?1:-1];
typedef char offset_m_iMinInputBufferLen[(offsetof(DESC,m_iMinInputBufferLen)==40)?1:-1];
typedef char offset_m_dwHandshake[(offsetof(DESC,m_dwHandshake)==44)?1:-1];
typedef char offset_m_dwHandshakeSentTime[(offsetof(DESC,m_dwHandshakeSentTime)==48)?1:-1];
typedef char offset_m_dwClientTime[(offsetof(DESC,m_dwClientTime)==52)?1:-1];
typedef char offset_m_iHandshakeRetry[(offsetof(DESC,m_iHandshakeRetry)==56)?1:-1];
typedef char offset_m_lpBufferedOutputBuffer[(offsetof(DESC,m_lpBufferedOutputBuffer)==60)?1:-1];
typedef char offset_m_lpOutputBuffer[(offsetof(DESC,m_lpOutputBuffer)==64)?1:-1];
typedef char offset_m_pkPingEvent[(offsetof(DESC,m_pkPingEvent)==68)?1:-1];
typedef char offset_m_lpCharacter[(offsetof(DESC,m_lpCharacter)==72)?1:-1];
typedef char offset_m_accountTable[(offsetof(DESC,m_accountTable)==76)?1:-1];
typedef char offset_m_SockAddr[(offsetof(DESC,m_SockAddr)==412)?1:-1];
typedef char offset_m_UDPSockAddr[(offsetof(DESC,m_UDPSockAddr)==428)?1:-1];
typedef char offset_m_pLogFile[(offsetof(DESC,m_pLogFile)==444)?1:-1];
typedef char offset_m_stRelayName[(offsetof(DESC,m_stRelayName)==448)?1:-1];
typedef char offset_m_bEncrypted[(offsetof(DESC,m_bEncrypted)==452)?1:-1];
typedef char offset_m_stP2PHost[(offsetof(DESC,m_stP2PHost)==456)?1:-1];
typedef char offset_m_wP2PPort[(offsetof(DESC,m_wP2PPort)==460)?1:-1];
typedef char offset_m_bP2PChannel[(offsetof(DESC,m_bP2PChannel)==462)?1:-1];
typedef char offset_m_bAdminMode[(offsetof(DESC,m_bAdminMode)==463)?1:-1];
typedef char offset_m_bPong[(offsetof(DESC,m_bPong)==464)?1:-1];
typedef char offset_m_iCurrentSequence[(offsetof(DESC,m_iCurrentSequence)==468)?1:-1];
typedef char offset_m_dwMatrixRows[(offsetof(DESC,m_dwMatrixRows)==472)?1:-1];
typedef char offset_m_dwMatrixCols[(offsetof(DESC,m_dwMatrixCols)==476)?1:-1];
typedef char offset_m_bMatrixTryCount[(offsetof(DESC,m_bMatrixTryCount)==480)?1:-1];
typedef char offset_m_pkLoginKey[(offsetof(DESC,m_pkLoginKey)==484)?1:-1];
typedef char offset_m_dwLoginKey[(offsetof(DESC,m_dwLoginKey)==488)?1:-1];
typedef char offset_m_adwDecryptionKey[(offsetof(DESC,m_adwDecryptionKey)==492)?1:-1];
typedef char offset_m_adwEncryptionKey[(offsetof(DESC,m_adwEncryptionKey)==508)?1:-1];
typedef char offset_m_bCRCMagicCubeIdx[(offsetof(DESC,m_bCRCMagicCubeIdx)==524)?1:-1];
typedef char offset_m_dwProcCRC[(offsetof(DESC,m_dwProcCRC)==528)?1:-1];
typedef char offset_m_dwFileCRC[(offsetof(DESC,m_dwFileCRC)==532)?1:-1];
typedef char offset_m_bHackCRCQuery[(offsetof(DESC,m_bHackCRCQuery)==536)?1:-1];
typedef char offset_m_dwBillingExpireSecond[(offsetof(DESC,m_dwBillingExpireSecond)==540)?1:-1];
typedef char offset_m_stClientVersion[(offsetof(DESC,m_stClientVersion)==544)?1:-1];
typedef char offset_m_stMatrixCode[(offsetof(DESC,m_stMatrixCode)==548)?1:-1];
typedef char offset_m_Login[(offsetof(DESC,m_Login)==552)?1:-1];
typedef char offset_m_outtime[(offsetof(DESC,m_outtime)==556)?1:-1];
typedef char offset_m_playtime[(offsetof(DESC,m_playtime)==560)?1:-1];
typedef char offset_m_offtime[(offsetof(DESC,m_offtime)==564)?1:-1];
typedef char offset_m_pkDisconnectEvent[(offsetof(DESC,m_pkDisconnectEvent)==568)?1:-1];
typedef char offset_m_stMatrixCardID[(offsetof(DESC,m_stMatrixCardID)==572)?1:-1];
typedef char offset_m_stMatrixQuiz[(offsetof(DESC,m_stMatrixQuiz)==576)?1:-1];

// Packed account/player member offsets from target DWARF.
typedef char offset_SAccountTable_id[(offsetof(SAccountTable,id)==0)?1:-1];
typedef char offset_SAccountTable_login[(offsetof(SAccountTable,login)==4)?1:-1];
typedef char offset_SAccountTable_passwd[(offsetof(SAccountTable,passwd)==35)?1:-1];
typedef char offset_SAccountTable_social_id[(offsetof(SAccountTable,social_id)==52)?1:-1];
typedef char offset_SAccountTable_status[(offsetof(SAccountTable,status)==71)?1:-1];
typedef char offset_SAccountTable_bEmpire[(offsetof(SAccountTable,bEmpire)==80)?1:-1];
typedef char offset_SAccountTable_players[(offsetof(SAccountTable,players)==81)?1:-1];
typedef char offset_SSimplePlayer_dwID[(offsetof(SSimplePlayer,dwID)==0)?1:-1];
typedef char offset_SSimplePlayer_szName[(offsetof(SSimplePlayer,szName)==4)?1:-1];
typedef char offset_SSimplePlayer_byJob[(offsetof(SSimplePlayer,byJob)==29)?1:-1];
typedef char offset_SSimplePlayer_byLevel[(offsetof(SSimplePlayer,byLevel)==30)?1:-1];
typedef char offset_SSimplePlayer_dwPlayMinutes[(offsetof(SSimplePlayer,dwPlayMinutes)==31)?1:-1];
typedef char offset_SSimplePlayer_byST[(offsetof(SSimplePlayer,byST)==35)?1:-1];
typedef char offset_SSimplePlayer_byHT[(offsetof(SSimplePlayer,byHT)==36)?1:-1];
typedef char offset_SSimplePlayer_byDX[(offsetof(SSimplePlayer,byDX)==37)?1:-1];
typedef char offset_SSimplePlayer_byIQ[(offsetof(SSimplePlayer,byIQ)==38)?1:-1];
typedef char offset_SSimplePlayer_wMainPart[(offsetof(SSimplePlayer,wMainPart)==39)?1:-1];
typedef char offset_SSimplePlayer_bChangeName[(offsetof(SSimplePlayer,bChangeName)==41)?1:-1];
typedef char offset_SSimplePlayer_wHairPart[(offsetof(SSimplePlayer,wHairPart)==42)?1:-1];
typedef char offset_SSimplePlayer_bDummy[(offsetof(SSimplePlayer,bDummy)==44)?1:-1];
typedef char offset_SSimplePlayer_x[(offsetof(SSimplePlayer,x)==48)?1:-1];
typedef char offset_SSimplePlayer_y[(offsetof(SSimplePlayer,y)==52)?1:-1];
typedef char offset_SSimplePlayer_lAddr[(offsetof(SSimplePlayer,lAddr)==56)?1:-1];
typedef char offset_SSimplePlayer_wPort[(offsetof(SSimplePlayer,wPort)==60)?1:-1];
typedef char offset_SSimplePlayer_skill_group[(offsetof(SSimplePlayer,skill_group)==62)?1:-1];

typedef char login_key_size[(sizeof(CLoginKey)==12)?1:-1];
typedef char login_key_key_offset[(offsetof(CLoginKey,m_dwKey)==0)?1:-1];
typedef char login_key_expire_offset[(offsetof(CLoginKey,m_dwExpireTime)==4)?1:-1];
typedef char login_key_desc_offset[(offsetof(CLoginKey,m_pkDesc)==8)?1:-1];

#include "../../game/src/packet.h"
typedef char relay_packet_size[(sizeof(TPacketGGRelay)==30)?1:-1];
typedef char relay_header_offset[(offsetof(TPacketGGRelay,bHeader)==0)?1:-1];
typedef char relay_name_offset[(offsetof(TPacketGGRelay,szName)==1)?1:-1];
typedef char relay_size_offset[(offsetof(TPacketGGRelay,lSize)==26)?1:-1];

typedef char handshake_packet_size[(sizeof(TPacketGCHandshake)==13)?1:-1];
typedef char handshake_id_offset[(offsetof(TPacketGCHandshake,dwHandshake)==1)?1:-1];
typedef char handshake_time_offset[(offsetof(TPacketGCHandshake,dwTime)==5)?1:-1];
typedef char handshake_delta_offset[(offsetof(TPacketGCHandshake,lDelta)==9)?1:-1];
typedef char bindudp_size[(sizeof(TPacketGCBindUDP)==7)?1:-1];
typedef char bindudp_addr_offset[(offsetof(TPacketGCBindUDP,addr)==1)?1:-1];
typedef char bindudp_port_offset[(offsetof(TPacketGCBindUDP,port)==5)?1:-1];

#include "../../game/src/desc_client.h"
typedef char client_desc_size[(sizeof(CLIENT_DESC)==588)?1:-1];

typedef char logout_size[(sizeof(TLogoutPacket)==48)?1:-1];
typedef char logout_passwd_offset[(offsetof(TLogoutPacket,passwd)==31)?1:-1];

typedef char chat_size[(sizeof(TPacketGCChat)==9)?1:-1];
typedef char chat_size_offset[(offsetof(TPacketGCChat,size)==1)?1:-1];
typedef char chat_id_offset[(offsetof(TPacketGCChat,id)==4)?1:-1];
typedef char chat_empire_offset[(offsetof(TPacketGCChat,bEmpire)==8)?1:-1];
