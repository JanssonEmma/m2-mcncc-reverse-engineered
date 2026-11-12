#include "gm.h"
#include "config.h"
#include "locale_service.h"
#include "../../libthecore/include/log.h"
#include <cstring>
#include <map>
#include <set>
#include <string>

struct tGM {
    tAdminInfo Info;
    std::set<std::string>* pset_Host;
};

std::set<std::string> g_set_Host;
std::map<std::string,tGM> g_map_GM;

void gm_new_clear()
{
    g_set_Host.clear();
    g_map_GM.clear();
}

void gm_new_insert(const tAdminInfo& info)
{
    sys_log(0,"InsertGMList(account:%s, player:%s, contact_ip:%s, server_ip:%s, auth:%d)",
            info.m_szAccount,info.m_szName,info.m_szContactIP,info.m_szServerIP,info.m_Authority);
    tGM record;
    const bool useHostSet=!info.m_szContactIP[0];
    record.pset_Host=useHostSet ? &g_set_Host : 0;
    sys_log(0,useHostSet ? "GM Use ContactIP" : "GM Use Default Host List");
    std::memcpy(&record.Info,&info,sizeof(info));
    g_map_GM[info.m_szName]=record;
}

void gm_new_host_inert(const char* host)
{
    g_set_Host.insert(host);
    sys_log(0,"InsertGMHost(ip:%s)",host);
}

BYTE gm_new_get_level(const char* name, const char* host)
{
    if (test_server) return 5;
    std::map<std::string,tGM>::iterator entry=g_map_GM.find(name);
    if (entry==g_map_GM.end()) return 0;
    const tGM& gm=entry->second;
    if (!LC_IsGermany() && !LC_IsEurope() && host) {
        if (gm.pset_Host) {
            if (gm.pset_Host->find(host)==gm.pset_Host->end()) return 0;
        } else if (std::strcmp(gm.Info.m_szContactIP,host)!=0) return 0;
    }
    return static_cast<BYTE>(gm.Info.m_Authority);
}

BYTE gm_get_level(const char* name, const char* host)
{
    return gm_new_get_level(name,host);
}
