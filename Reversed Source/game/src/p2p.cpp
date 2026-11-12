#include "guild_manager.h"
#include "party.h"
#include "messenger_manager.h"
#include "../../libthecore/include/utils.h"
#include "p2p.h"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "desc.h"
#include "packet.h"
#include "sectree_manager.h"
#include "../../libthecore/include/log.h"
#include <cstring>
#include <cassert>

P2P_MANAGER::P2P_MANAGER() : m_pkInputProcessor(0),m_iHandleCount(0)
{
    for (int i=0;i<4;++i) m_aiEmpireUserCount[i]=0;
}
P2P_MANAGER::~P2P_MANAGER() {}

void P2P_MANAGER::Send(const void* data,int size,DESC* except)
{
    std::set<DESC*>::iterator it=m_set_pkPeers.begin();
    while (it!=m_set_pkPeers.end()) {
        DESC* peer=*it++;
        if (peer!=except) peer->Packet(data,size);
    }
}
void P2P_MANAGER::FlushOutput()
{
    std::set<DESC*>::iterator it=m_set_pkPeers.begin();
    while (it!=m_set_pkPeers.end()) {
        DESC* peer=*it++;
        peer->FlushOutput();
    }
}
void P2P_MANAGER::Boot(DESC* desc)
{
    CHARACTER_MANAGER::NAME_MAP& players=CHARACTER_MANAGER::instance().m_map_pkPCChr;
    CHARACTER_MANAGER::NAME_MAP::iterator it=players.begin();
    while (it!=players.end()) {
        CHARACTER* ch=(it++)->second;
        TPacketGGLogin packet;
        packet.bHeader=1;
        std::strcpy(packet.szName,ch->GetName());
        packet.dwPID=ch->GetPlayerID();
        packet.bEmpire=ch->GetEmpire();
        packet.bMapIndex=static_cast<BYTE>(SECTREE_MANAGER::instance().GetMapIndex(ch->GetX(),ch->GetY()));
        packet.bChannel=g_bChannel;
        desc->Packet(&packet,33);
    }
}
void P2P_MANAGER::RegisterAcceptor(DESC* desc)
{
    sys_log(0,"P2P Acceptor opened (host %s)",desc->GetHostName());
    m_set_pkPeers.insert(desc);
    Boot(desc);
}
void P2P_MANAGER::RegisterConnector(DESC* desc)
{
    sys_log(0,"P2P Connector opened (host %s)",desc->GetHostName());
    m_set_pkPeers.insert(desc);
    Boot(desc);
    TPacketGGSetup setup;
    setup.wPort=p2p_port;
    setup.bHeader=9;
    setup.bChannel=g_bChannel;
    desc->Packet(&setup,4);
}

void P2P_MANAGER::EraseUserByDesc(DESC* desc)
{
    for (__gnu_cxx::hash_map<std::string,CCI*,stringhash,std::equal_to<std::string>,std::allocator<CCI*> >::iterator
             it=m_map_pkCCI.begin();it!=m_map_pkCCI.end();) {
        __gnu_cxx::hash_map<std::string,CCI*,stringhash,std::equal_to<std::string>,std::allocator<CCI*> >::iterator current=it++;
        CCI* info=current->second;
        if (info->pkDesc!=desc) continue;
        if (info->bChannel==g_bChannel) --m_aiEmpireUserCount[info->bEmpire];
        m_map_dwPID_pkCCI.erase(info->dwPID);
        delete info;
        m_map_pkCCI.erase(current);
    }
}

void P2P_MANAGER::UnregisterAcceptor(DESC* desc)
{
    sys_log(0,"P2P Acceptor closed (host %s)",desc->GetHostName());
    EraseUserByDesc(desc);
    m_set_pkPeers.erase(desc);
}

void P2P_MANAGER::UnregisterConnector(DESC* desc)
{
    std::set<DESC*>::iterator it=m_set_pkPeers.find(desc);
    if (it==m_set_pkPeers.end()) return;
    sys_log(0,"P2P Connector closed (host %s)",desc->GetHostName());
    EraseUserByDesc(desc);
    m_set_pkPeers.erase(it);
}

CCI* P2P_MANAGER::Find(const char* name)
{
    __gnu_cxx::hash_map<std::string,CCI*,stringhash,std::equal_to<std::string>,std::allocator<CCI*> >::iterator it=m_map_pkCCI.find(name);
    return it==m_map_pkCCI.end() ? 0 : it->second;
}

CCI* P2P_MANAGER::FindByPID(DWORD pid)
{
    __gnu_cxx::hash_map<DWORD,CCI*,__gnu_cxx::hash<DWORD>,std::equal_to<DWORD>,std::allocator<CCI*> >::iterator it=m_map_dwPID_pkCCI.find(pid);
    return it==m_map_dwPID_pkCCI.end() ? 0 : it->second;
}

bool P2P_MANAGER::IsExist(const char* host, WORD port)
{
    std::set<DESC*>::iterator it=m_set_pkPeers.begin();
    while (it!=m_set_pkPeers.end()) {
        DESC* peer=*it++;
        if (std::strcmp(host,peer->GetHostName())==0 && peer->GetP2PPort()==port) return true;
    }
    return false;
}

int P2P_MANAGER::GetCount()
{
    return m_aiEmpireUserCount[1]+m_aiEmpireUserCount[2]+m_aiEmpireUserCount[3];
}

int P2P_MANAGER::GetEmpireUserCount(int index)
{
    assert(index<4);
    return m_aiEmpireUserCount[index];
}

void P2P_MANAGER::RecvLogin(DESC* desc, const SPacketGGLogin* packet)
{
    CCI* info=Find(packet->szName);
    if (!info) {
        info=new CCI;
        std::strncpy(info->szName,packet->szName,24);
        info->szName[24]='\0';
        info->dwPID=packet->dwPID;
        info->bEmpire=packet->bEmpire;
        info->dwTime=get_dword_time();
        if (g_bChannel==packet->bChannel) ++m_aiEmpireUserCount[info->bEmpire];
        m_map_pkCCI.insert(std::make_pair(std::string(info->szName),info));
        m_map_dwPID_pkCCI.insert(std::make_pair(info->dwPID,info));
    } else info->dwTime=get_dword_time();
    info->bMapIndex=packet->bMapIndex;
    info->pkDesc=desc;
    info->bChannel=packet->bChannel;
    sys_log(0,"P2P: Login %s",info->szName);
    CGuildManager::instance().P2PLoginMember(info->dwPID);
    CPartyManager::instance().P2PLogin(info->dwPID,info->szName);
    MessengerManager::instance().P2PLogin(std::string(info->szName));
}

void P2P_MANAGER::RecvLogout(DESC*, const char* name)
{
    CCI* info=Find(name);
    if (!info) {
        _sys_err("RecvLogout",0xc1,"cannot find %s",name);
        return;
    }
    if (info->bChannel==g_bChannel) --m_aiEmpireUserCount[info->bEmpire];
    CGuildManager::instance().P2PLogoutMember(info->dwPID);
    CPartyManager::instance().P2PLogout(info->dwPID);
    MessengerManager::instance().P2PLogout(std::string(info->szName));
    m_map_pkCCI.erase(std::string(name));
    m_map_dwPID_pkCCI.erase(info->dwPID);
    delete info;
    sys_log(0,"P2P: Logout %s",name);
}
