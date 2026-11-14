#include "locale_service.h"
#include "xmas_event.h"
#include "empire_text_convert.h"
#include "desc_manager.h"
#include "guild_manager.h"
#include "guild.h"
#include "messenger_manager.h"
#include "input.h"
#include "char.h"
#include "char_manager.h"
#include <cstring>
#include "config.h"
#include "desc.h"
#include "desc_client.h"
#include "dev_log.h"
#include "p2p.h"
#include "packet.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"

void Shutdown(int seconds);
void LoadValidCRCList();
void CheckClientVersion();
void castle_siege(int empire);

CInputP2P::CInputP2P()
{
    static CPacketInfoGG s_packetInfo;
    BindPacketInfo(&s_packetInfo);
}

CInputP2P::~CInputP2P() {}

BYTE CInputP2P::GetType() { return 7; }

void CInputP2P::Login(DESC* desc, const char* data)
{
    P2P_MANAGER::instance().RecvLogin(desc,reinterpret_cast<const TPacketGGLogin*>(data));
}

void CInputP2P::Logout(DESC* desc, const char* data)
{
    P2P_MANAGER::instance().RecvLogout(desc,data+1);
}

void CInputP2P::Setup(DESC* desc, const char* data)
{
    const TPacketGGSetup* packet=reinterpret_cast<const TPacketGGSetup*>(data);
    sys_log(0,"P2P: Setup %s:%d",desc->GetHostName(),packet->wPort);
    desc->SetP2P(desc->GetHostName(),packet->wPort,packet->bChannel);
}

int CInputP2P::Analyze(DESC* desc, BYTE header, const char* data)
{
    if (test_server) sys_log(0,"CInputP2P::Anlayze[Header %d]",static_cast<unsigned int>(header));
    int extra=0;
    switch (header) {
        case 1: Login(desc,data); break;
        case 2: Logout(desc,data); break;
        case 3: extra=Relay(desc,data,m_iBufferLeft); break;
        case 4: extra=Notice(desc,data,m_iBufferLeft); break;
        case 5: Shutdown(10); break;
        case 6: extra=Guild(desc,data,m_iBufferLeft); break;
        case 7: Disconnect(data); break;
        case 8: Shout(data); break;
        case 9: Setup(desc,data); break;
        case 10: MessengerAdd(data); break;
        case 11: MessengerRemove(data); break;
        case 12: FindPosition(desc,data); break;
        case 13: WarpCharacter(data); break;
        case 14: MessengerMobile(data); break;
        case 15: GuildWarZoneMapIndex(data); break;
        case 16: Transfer(data); break;
        case 17: XmasWarpSanta(data); break;
        case 18: XmasWarpSantaReply(data); break;
        case 19: LoadValidCRCList(); break;
        case 20: LoginPing(desc,data); break;
        case 21: CheckClientVersion(); break;
        case 22: BlockChat(data); break;
        case 23: ForkedMapInfo(data); break;
        case 25:
            dev_log("input_p2p.cpp",0x1d5,"Analyze",0x800,const_cast<char*>("INPUT_P2P SIEGE"));
            castle_siege(number_ex(1,3,"input_p2p.cpp",0x1d6));
            break;
    }
    return extra<0 ? -1 : extra;
}

namespace {
template<class T>
T PeerField32(const char* data)
{
    T value;
    std::memcpy(&value,data,4);
    return value;
}
}

void CInputP2P::WarpCharacter(const char* data)
{
    CHARACTER* character=CHARACTER_MANAGER::instance().FindByPID(PeerField32<unsigned int>(data+1));
    if (character) character->WarpSet(PeerField32<int>(data+5),PeerField32<int>(data+9),0);
}

void CInputP2P::Transfer(const char* data)
{
    CHARACTER* character=CHARACTER_MANAGER::instance().FindPC(data+1);
    if (character) character->WarpSet(PeerField32<int>(data+26),PeerField32<int>(data+30),0);
}

void CInputP2P::FindPosition(DESC* desc, const char* data)
{
    CHARACTER* target=CHARACTER_MANAGER::instance().FindByPID(PeerField32<unsigned int>(data+5));
    if (!target || target->GetMapIndex()>=10000) return;

    char reply[13];
    reply[0]=13;
    std::memcpy(reply+1,data+1,4);
    int x=target->GetX();
    int y=target->GetY();
    std::memcpy(reply+5,&x,4);
    std::memcpy(reply+9,&y,4);
    desc->Packet(reply,13);
}

class CLoginData;
void SendBillingExpire(const char* login, BYTE type, int seconds, CLoginData* data);
void SendShout(const char* text, BYTE empire);
void SendNotice(const char* text);
#include "questlua_forked.h"

void CInputP2P::Disconnect(const char* data)
{
    constexpr std::size_t loginWidth = 25;
    const char* login = data + 1;
    const char* end = static_cast<const char*>(std::memchr(login, 0, loginWidth));
    DESC* desc = DESC_MANAGER::instance().FindByLoginName(
        std::string(login, end ? end : login + loginWidth));
    if (!desc) return;
    if (desc->GetCharacter()) desc->DisconnectOfSameLogin();
    else desc->SetPhase(0);
}

void CInputP2P::Shout(const char* data)
{
    SendShout(data+2,static_cast<BYTE>(data[1]));
}

void CInputP2P::MessengerAdd(const char* data)
{
    sys_log(0,"P2P: Messenger Add %s %s",data+1,data+26);
    MessengerManager::instance().__AddToList(std::string(data+1),std::string(data+26));
}

void CInputP2P::MessengerRemove(const char* data)
{
    sys_log(0,"P2P: Messenger Remove %s %s",data+1,data+26);
    MessengerManager::instance().__RemoveFromList(std::string(data+1),std::string(data+26));
}

void CInputP2P::MessengerMobile(const char* data)
{
    MessengerManager::instance().__UpdateMobile(data+1,data+26);
}

void CInputP2P::GuildWarZoneMapIndex(const char* data)
{
    CGuildManager& manager=CGuildManager::instance();
    DWORD first=PeerField32<unsigned int>(data+1);
    DWORD second=PeerField32<unsigned int>(data+5);
    long map=PeerField32<int>(data+9);
    sys_log(0,"P2P: GuildWarZoneMapIndex g1(%lu) vs g2(%lu), mapIndex(%ld)",first,second,map);
    CGuild* guild=manager.FindGuild(first);
    CGuild* opponent=manager.FindGuild(second);
    if (guild && opponent) {
        guild->SetGuildWarMapIndex(second,map);
        opponent->SetGuildWarMapIndex(first,map);
    }
}

void CInputP2P::BlockChat(const char* data)
{
    const char* name=data+1;
    CHARACTER* character=CHARACTER_MANAGER::instance().FindPC(name);
    int duration=PeerField32<int>(data+26);
    if (!character) {
        sys_log(0,"BLOCK CHAT fail name %s dur %d",name,duration);
        return;
    }
    sys_log(0,"BLOCK CHAT apply name %s dur %d",name,duration);
    character->AddAffect(223,0,0,0,duration,0,true,false);
}

void CInputP2P::ForkedMapInfo(const char* data)
{
    BYTE pass=static_cast<BYTE>(data[1]);
    BYTE sungzi=static_cast<BYTE>(data[2]);
    if (test_server) sys_log(0,"Initialize forkedmap infomation %d %d",pass,sungzi);
    SetForkedMap(pass,sungzi);
}

void CInputP2P::LoginPing(DESC* desc, const char* data)
{
    SendBillingExpire(data+1,6,0,0);
    if (!g_pkAuthMasterDesc) P2P_MANAGER::instance().Send(data,32,desc);
}

namespace {
int PeerPacketSize(int payload, unsigned int header)
{
    unsigned int bits=static_cast<unsigned int>(payload)+header;
    int result;
    std::memcpy(&result,&bits,4);
    return result;
}
}

int CInputP2P::Notice(DESC*, const char* data, int bytes)
{
    if (bytes<PeerPacketSize(PeerField32<int>(data+1),5)) return -1;
    char notice[257];
    std::strncpy(notice,data+5,256);
    notice[256]=0;
    SendNotice(notice);
    return PeerField32<int>(data+1);
}

int CInputP2P::Relay(DESC*, const char* data, int bytes)
{
    if (bytes<PeerPacketSize(PeerField32<int>(data+26),30)) return -1;
    sys_log(0,"InputP2P::Relay : %s size %d",data+1,PeerField32<int>(data+26));
    CHARACTER* character=CHARACTER_MANAGER::instance().FindPC(data+1);
    if (character) {
        if (data[30]==34) {
            if (character->GetBlockMode()&8) return PeerField32<int>(data+26);
            char* packet=const_cast<char*>(data);
            BYTE empire=static_cast<BYTE>(~static_cast<BYTE>(packet[33]));
            packet[33]=0;
            if (!character->IsEquipUniqueGroup(10025) &&
                empire>=1 && empire<=3 && character->GetEmpire()!=empire) {
                int power=character->GetSkillPower(empire+125,0);
                ConvertEmpireText(empire,packet+59,power*2+10);
            }
        }
        character->GetDesc()->Packet(data+30,PeerField32<int>(data+26));
    }
    return PeerField32<int>(data+26);
}

namespace {
struct FuncShout
{
    const char* m_str;
    BYTE m_bEmpire;

    void operator()(LPDESC desc)
    {
        if (desc->GetCharacter() && (desc->GetCharacter()->GetGMLevel() || desc->GetEmpire() == m_bEmpire))
            desc->GetCharacter()->ChatPacket(6, "%s", m_str);
    }
};
}

void SendShout(const char* text, BYTE empire)
{
    FuncShout send{text, empire};
    for (DESC* desc : DESC_MANAGER::instance().GetClientSet())
        send(desc);
}

int CInputP2P::Guild(DESC*, const char* data, int bytes)
{
    CGuild* guild=CGuildManager::instance().FindGuild(PeerField32<unsigned int>(data+2));
    if (data[1]) return 0;
    if (bytes<98) return -1;
    if (guild) guild->P2PChat(data+12);
    return 92;
}

void CInputP2P::XmasWarpSanta(const char* data)
{
    if (static_cast<BYTE>(data[1])!=g_bChannel) return;
    int map=PeerField32<int>(data+2);
    if (!map_allow_find(map)) return;
    xmas::SpawnSanta(map,LC_IsYMIR() ? 1200 : 3000);
    BYTE reply[2]={18,g_bChannel};
    P2P_MANAGER::instance().Send(reply,2,0);
}

void CInputP2P::XmasWarpSantaReply(const char* data)
{
    if (static_cast<BYTE>(data[1])!=g_bChannel) return;
    CHARACTER_MANAGER& manager=CHARACTER_MANAGER::instance();
    CHARACTER_SET* characters=manager.GetCharacterSetByRaceNum(20031);
    CHARACTER_SET::iterator it=characters->begin();
    while (it!=characters->end()) manager.DestroyCharacter(*it++);
}
