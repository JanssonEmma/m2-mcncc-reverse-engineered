// Target-backed base registry; no newer packet registrations copied.
#include "packet_info.h"
#include "../../libthecore/include/utils.h"
#include <cassert>
#include <cstdio>

CPacketInfo::CPacketInfo() : m_pCurrentPacket(0) {}
CPacketInfo::~CPacketInfo()
{
    for (std::map<int, TPacketElement*>::iterator it=m_pPacketMap.begin();
         it!=m_pPacketMap.end(); ++it)
        delete it->second;
}
void CPacketInfo::Set(int header, int size, const char* name, bool sequence)
{
    if (m_pPacketMap.find(header)!=m_pPacketMap.end())
        return;
    TPacketElement* element=new TPacketElement;
    element->iSize=size;
    element->stName.assign(name);
    element->iCalled=0;
    element->dwLoad=0;
    element->bSequencePacket=sequence;
    if (sequence)
        element->iSize=static_cast<int>(static_cast<unsigned int>(element->iSize)+1U);
    m_pPacketMap.insert(std::make_pair(header,element));
}
bool CPacketInfo::Get(int header, int* size, const char** name)
{
    std::map<int, TPacketElement*>::iterator it=m_pPacketMap.find(header);
    if(it==m_pPacketMap.end())
        return false;
    *size=it->second->iSize;
    *name=it->second->stName.c_str();
    m_pCurrentPacket=it->second;
    return true;
}
TPacketElement* CPacketInfo::GetElement(int header)
{
    std::map<int, TPacketElement*>::iterator it=m_pPacketMap.find(header);
    return it==m_pPacketMap.end()?0:it->second;
}
bool CPacketInfo::IsSequencePacket(int header)
{
    TPacketElement* element=GetElement(header);
    return element?element->bSequencePacket:false;
}
void CPacketInfo::Start()
{
    assert(m_pCurrentPacket!=0);
    m_dwStartTime=get_dword_time();
}
void CPacketInfo::End()
{
    // Target int count uses 32-bit wrapping; DWORD elapsed/load arithmetic is unsigned.
    TPacketElement* packet=m_pCurrentPacket;
    packet->iCalled=static_cast<int>(static_cast<unsigned int>(packet->iCalled)+1U);
    const DWORD now=get_dword_time();
    packet->dwLoad+=now-m_dwStartTime;
}
void CPacketInfo::Log(const char* filename)
{
    FILE* file=std::fopen(filename,"w");
    if(!file)
        return;
    std::fprintf(file,"Name             Called     Load       Ratio\n");
    for(std::map<int, TPacketElement*>::iterator it=m_pPacketMap.begin();
        it!=m_pPacketMap.end(); ++it) {
        TPacketElement* element=it->second;
        const double ratio=element->iCalled!=0?
            static_cast<double>(element->dwLoad)/static_cast<double>(element->iCalled):0.0;
        std::fprintf(file,"%-16s %-10d %-10ld %.2f\n",element->stName.c_str(),
            element->iCalled,static_cast<long>(element->dwLoad),ratio);
    }
    std::fclose(file);
}

// Registration sizes/IDs are literal target constants, not newer sizeof values.
CPacketInfoCG::CPacketInfoCG()
{
    Set(64, 1, "Text", false);
    Set(255, 13, "Handshake", false);
    Set(252, 13, "TimeSync", true);
    Set(100, 9, "MarkLogin", false);
    Set(104, 5, "MarkIdxList", false);
    Set(101, 257, "MarkCrcList", false);
    Set(102, 773, "MarkUpload", false);
    Set(112, 7, "SymbolUpload", false);
    Set(113, 13, "SymbolCRC", false);
    Set(1, 49, "Login", true);
    Set(109, 52, "Login2", true);
    Set(111, 65, "Login3", true);
    Set(2, 8, "Attack", true);
    Set(3, 3, "Chat", true);
    Set(19, 28, "Whisper", true);
    Set(6, 2, "Select", true);
    Set(4, 34, "Create", true);
    Set(5, 10, "Delete", true);
    Set(10, 1, "EnterGame", true);
    Set(11, 2, "ItemUse", true);
    Set(12, 6, "ItemDrop", true);
    Set(20, 7, "ItemDrop2", true);
    Set(13, 4, "ItemMove", true);
    Set(15, 5, "ItemPickup", true);
    Set(16, 4, "QuickslotAdd", true);
    Set(17, 2, "QuickslotDel", true);
    Set(18, 3, "QuickslotSwap", true);
    Set(50, 2, "Shop", true);
    Set(26, 5, "OnClick", true);
    Set(27, 7, "Exchange", true);
    Set(28, 2, "Position", true);
    Set(29, 2, "ScriptAnswer", true);
    Set(66, 5, "ScriptButton", true);
    Set(30, 66, "QuestInputString", true);
    Set(31, 6, "QuestConfirm", true);
    Set(7, 16, "Move", true);
    Set(8, 3, "SyncPosition", true);
    Set(51, 13, "FlyTarget", true);
    Set(53, 13, "AddFlyTarget", true);
    Set(54, 2, "Shoot", true);
    Set(52, 9, "UseSkill", true);
    Set(60, 3, "UseItemToItem", true);
    Set(61, 5, "Target", true);
    Set(65, 1, "Warp", true);
    Set(67, 2, "Messenger", true);
    Set(74, 5, "PartyRemove", true);
    Set(72, 5, "PartyInvite", true);
    Set(73, 6, "PartyInviteAnswer", true);
    Set(75, 7, "PartySetState", true);
    Set(76, 6, "PartyUseSkill", true);
    Set(78, 2, "PartyParam", true);
    Set(90, 2, "Empire", true);
    Set(71, 3, "SafeboxCheckout", true);
    Set(70, 3, "SafeboxCheckin", true);
    Set(77, 4, "SafeboxItemMove", true);
    Set(80, 2, "Guild", true);
    Set(81, 14, "AnswerMakeGuild", true);
    Set(82, 2, "Fishing", true);
    Set(83, 7, "ItemGive", true);
    Set(105, 257, "Hack", true);
    Set(55, 35, "MyShop", true);
    Set(96, 3, "Refine", true);
    Set(106, 27, "ChangeName", true);
    Set(107, 28, "SMS", true);
    Set(253, 67, "Version", true);
    Set(241, 67, "Version", true);
    Set(103, 14, "CRCReport", true);
    Set(108, 10, "MatrixCard", true);
    Set(254, 1, "Pong", true);
    Set(69, 3, "MallCheckout", true);
    Set(114, 5, "ScriptSelectItem", true);
    Set(201, 6, "RunupMatrixAnswer", true);
    Set(202, 10, "PasspodAnswer", true);
}
CPacketInfoCG::~CPacketInfoCG()
{
    Log("packet_info.txt");
}

// Registration sizes/IDs are literal target constants, not newer sizeof values.
CPacketInfoGG::CPacketInfoGG()
{
    Set(9, 4, "Setup", false);
    Set(1, 33, "Login", false);
    Set(2, 26, "Logout", false);
    Set(3, 30, "Relay", false);
    Set(4, 5, "Notice", false);
    Set(5, 1, "Shutdown", false);
    Set(6, 6, "Guild", false);
    Set(8, 88, "Shout", false);
    Set(7, 26, "Disconnect", false);
    Set(10, 51, "MessengerAdd", false);
    Set(11, 51, "MessengerRemove", false);
    Set(12, 9, "FindPosition", false);
    Set(13, 13, "WarpCharacter", false);
    Set(14, 59, "MessengerMobile", false);
    Set(15, 13, "GuildWarMapIndex", false);
    Set(16, 34, "Transfer", false);
    Set(17, 6, "XmasWarpSanta", false);
    Set(18, 2, "XmasWarpSantaReply", false);
    Set(19, 1, "ReloadCRCList", false);
    Set(21, 1, "CheckClientVersion", false);
    Set(20, 32, "LoginPing", false);
    Set(22, 30, "BlockChat", false);
    Set(23, 3, "ForkedMapInfo", false);
    Set(25, 1, "Siege", false);
}
CPacketInfoGG::~CPacketInfoGG()
{
    Log("p2p_packet_info.txt");
}
