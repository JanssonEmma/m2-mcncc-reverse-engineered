#include "locale.hpp"
#include <cstring>
#include "messenger_manager.h"
#include "buffer_manager.h"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "desc.h"
#include "desc_client.h"
#include "packet.h"
#include "p2p.h"
#include "crc32.h"
#include <cstdio>
#include "../../libthecore/include/log.h"
#include <functional>

#include "gm.h"

// 080dd820, 080ddbb0, 080ddfe0, 080ddff0
MessengerManager::MessengerManager() {}
MessengerManager::~MessengerManager() {}
void MessengerManager::Initialize() {}
void MessengerManager::Destroy() {}

// 080de000, 080de010
void MessengerManager::P2PLogin(keyA account) { Login(account); }
void MessengerManager::P2PLogout(keyA account) { Logout(account); }

// 080de020
void MessengerManager::Login(keyA account)
{
    if (m_set_loginAccount.find(account) != m_set_loginAccount.end())
        return;
    DBManager::instance().FuncQuery(std::bind1st(std::mem_fun(&MessengerManager::LoadList), this),
        "SELECT m.account, m.companion, p.mobile FROM messenger_list%s AS m, player%s AS p "
        "WHERE m.account='%s' AND m.companion=p.name",
        get_table_postfix(), get_table_postfix(), account.c_str());
    m_set_loginAccount.insert(account);
}

// 080de140
void MessengerManager::LoadList(SQLMsg* msg)
{
    if (msg->Get()->iNumRows < 1)
        return;
    sys_log(0, "Messenger::LoadList");
    std::string account;
    for (int i = 0; i < msg->Get()->iNumRows; ++i) {
        MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
        if (row[0] && row[1]) {
            if (account.empty())
                account = row[0];
            m_Relation[row[0]].insert(row[1]);
            m_InverseRelation[row[1]].insert(row[0]);
        }
        if (row[1] && row[2] && *row[2])
            m_map_stMobile[row[1]] = row[2];
    }
    SendList(account);
    AccountSet& watchers = m_InverseRelation[account];
    for (AccountSet::iterator watcher = watchers.begin(); watcher != watchers.end(); ++watcher)
        SendLogin(*watcher, account);
}

// 080dee30
void MessengerManager::Logout(keyA account)
{
    if (m_set_loginAccount.find(account) == m_set_loginAccount.end())
        return;
    m_set_loginAccount.erase(account);
    AccountSet& watchers = m_InverseRelation[account];
    for (AccountSet::iterator watcher = watchers.begin(); watcher != watchers.end(); ++watcher)
        SendLogout(*watcher, account);
    for (RelationMap::iterator entry = m_Relation.begin(); entry != m_Relation.end(); ++entry)
        entry->second.erase(account);
    m_Relation.erase(account);
}

// 080e0560
void MessengerManager::SendList(keyA account)
{
    CHARACTER* character = CHARACTER_MANAGER::instance().FindPC(account.c_str());
    if (!character || !character->GetDesc())
        return;
    LPDESC desc = character->GetDesc();
    RelationMap::iterator relation = m_Relation.find(account);
    if (relation == m_Relation.end() || relation->second.empty())
        return;
    TPacketGCMessenger packet;
    packet.header = 74;
    packet.size = sizeof(packet);
    packet.subheader = 0;
    TEMP_BUFFER buffer;
    for (AccountSet::iterator it = relation->second.begin(); it != relation->second.end(); ++it) {
        const bool online = m_set_loginAccount.find(*it) != m_set_loginAccount.end();
        const bool mobile = m_map_stMobile.find(*it) != m_map_stMobile.end();
        const BYTE length = static_cast<BYTE>(it->size());
        if (online) {
            TPacketGCMessengerListOnline entry;
            entry.connected = mobile ? 3 : 1;
            entry.length = length;
            buffer.write(&entry, sizeof(entry));
        } else {
            TPacketGCMessengerListOffline entry;
            entry.connected = mobile ? 2 : 0;
            entry.length = length;
            buffer.write(&entry, sizeof(entry));
        }
        buffer.write(it->c_str(), it->size());
    }
    packet.size += buffer.size();
    desc->BufferedPacket(&packet, sizeof(packet));
    desc->Packet(buffer.read_peek(), buffer.size());
}

// 080e0d80
void MessengerManager::SendLogin(keyA account, keyA companion)
{
    CHARACTER* character = CHARACTER_MANAGER::instance().FindPC(account.c_str());
    LPDESC desc = character ? character->GetDesc() : 0;
    if (!desc || !desc->GetCharacter())
        return;
    if (!character->GetGMLevel() && gm_get_level(companion.c_str(), 0))
        return;
    const BYTE length = static_cast<BYTE>(companion.size());
    TPacketGCMessenger packet;
    packet.header = 74;
    packet.size = sizeof(packet) + sizeof(length) + length;
    packet.subheader = 1;
    desc->BufferedPacket(&packet, sizeof(packet));
    desc->BufferedPacket(&length, sizeof(length));
    desc->Packet(companion.c_str(), companion.size());
}

// 080e0ec0
void MessengerManager::SendLogout(keyA account, keyA companion)
{
    if (companion.empty())
        return;
    CHARACTER* character = CHARACTER_MANAGER::instance().FindPC(account.c_str());
    LPDESC desc = character ? character->GetDesc() : 0;
    if (!desc)
        return;
    const BYTE length = static_cast<BYTE>(companion.size());
    TPacketGCMessenger packet;
    packet.header = 74;
    packet.size = sizeof(packet) + sizeof(length) + length;
    packet.subheader = 2;
    desc->BufferedPacket(&packet, sizeof(packet));
    desc->BufferedPacket(&length, sizeof(length));
    desc->Packet(companion.c_str(), companion.size());
}

void MessengerManager::__AddToList(keyA account, keyA companion)
{
    m_Relation[account].insert(companion);
    m_InverseRelation[companion].insert(account);
    CHARACTER_MANAGER& characters=CHARACTER_MANAGER::instance();
    CHARACTER* owner=characters.FindPC(account.c_str());
    if (owner && owner->GetDesc())
        owner->ChatPacket(1,locale_find("\074\270\336\275\305\301\256\076\040\045\163\040\264\324\300\273\040\304\243\261\270\267\316\040\303\337\260\241\307\317\277\264\275\300\264\317\264\331\056",g_iUseLocale),companion.c_str());
    if (characters.FindPC(companion.c_str())) SendLogin(account,companion);
    else SendLogout(account,companion);
}

void MessengerManager::__RemoveFromList(keyA account, keyA companion)
{
    m_Relation[account].erase(companion);
    m_InverseRelation[companion].erase(account);
    CHARACTER* owner=CHARACTER_MANAGER::instance().FindPC(account.c_str());
    if (owner && owner->GetDesc())
        owner->ChatPacket(1,locale_find("\074\270\336\275\305\301\256\076\040\045\163\040\264\324\300\273\040\270\336\275\305\300\372\277\241\274\255\040\273\350\301\246\307\317\277\264\275\300\264\317\264\331\056",g_iUseLocale),companion.c_str());
}

void MessengerManager::__UpdateMobile(const char* name, const char* mobile)
{
    if (!name || !mobile) return;
    std::map<keyT,keyT>::iterator it=m_map_stMobile.find(name);
    BYTE exists=*mobile!=0;
    if (it==m_map_stMobile.end()) {
        if (exists) m_map_stMobile.insert(std::make_pair(std::string(name),std::string(mobile)));
    } else if (exists) it->second.assign(mobile);
    else m_map_stMobile.erase(it);

    BYTE length=static_cast<BYTE>(std::strlen(name));
    TPacketGCMessenger packet;
    packet.header=74;
    packet.size=length+6;
    packet.subheader=4;
    AccountSet& watchers=m_InverseRelation[name];
    for (AccountSet::iterator watcher=watchers.begin();watcher!=watchers.end();++watcher) {
        CHARACTER* target=CHARACTER_MANAGER::instance().FindPC(watcher->c_str());
        if (target) {
            DESC* desc=target->GetDesc();
            desc->BufferedPacket(&packet,4);
            desc->BufferedPacket(&exists,1);
            desc->BufferedPacket(&length,1);
            desc->Packet(name,length);
        }
    }
}

namespace {
DWORD MessengerRequestKey(const std::string& requester, const std::string& recipient)
{
    const crc_t first = get_crc32(requester.c_str(), requester.size());
    const crc_t second = get_crc32(recipient.c_str(), recipient.size());
    char identity[64];
    std::snprintf(identity, sizeof(identity), "%lu:%lu", first, second);
    return get_crc32(identity, std::strlen(identity));
}

void SendMessengerChange(BYTE header, const std::string& account, const std::string& companion)
{
    char packet[51]{};
    packet[0] = header;
    std::strncpy(packet + 1, account.c_str(), 24);
    std::strncpy(packet + 26, companion.c_str(), 24);
    P2P_MANAGER::instance().Send(packet, sizeof(packet));
}
}

// 080df280
void MessengerManager::RequestToAdd(CHARACTER* character, CHARACTER* target)
{
    if (!character->GetDesc() || !target->GetDesc())
        return;
    m_set_requestToAdd.insert(MessengerRequestKey(character->GetName(), target->GetName()));
    target->ChatPacket(5, "messenger_auth %s", character->GetName());
}

// 080df3d0
void MessengerManager::AuthToAdd(keyA account, keyA companion, bool deny)
{
    if (!m_set_requestToAdd.erase(MessengerRequestKey(companion, account))) {
        sys_log(0, "MessengerManager::AuthToAdd : request not exist %s -> %s",
                companion.c_str(), account.c_str());
        return;
    }
    if (!deny) {
        AddToList(companion, account);
        AddToList(account, companion);
    }
}

// 080dfa70
void MessengerManager::AddToList(keyA account, keyA companion)
{
    if (companion.empty() || account.size() > 24 || companion.size() > 24 ||
        m_Relation[account].count(companion))
        return;

    sys_log(0, "Messenger Add %s %s", account.c_str(), companion.c_str());
    auto& db = DBManager::instance();
    char escapedAccount[49]{};
    char escapedCompanion[49]{};
    if (db.EscapeString(escapedAccount, account.c_str(), account.size()) >= sizeof(escapedAccount) ||
        db.EscapeString(escapedCompanion, companion.c_str(), companion.size()) >= sizeof(escapedCompanion))
        return;
    db.Query("INSERT INTO messenger_list%s VALUES ('%s', '%s')",
             get_table_postfix(), escapedAccount, escapedCompanion);
    __AddToList(account, companion);
    SendMessengerChange(10, account, companion);
}

// 080e0390
void MessengerManager::RemoveFromList(keyA account, keyA companion)
{
    if (companion.empty() || account.size() > 24 || companion.size() > 24)
        return;

    sys_log(1, "Messenger Remove %s %s", account.c_str(), companion.c_str());
    auto& db = DBManager::instance();
    char escapedAccount[49]{};
    char escapedCompanion[49]{};
    if (db.EscapeString(escapedAccount, account.c_str(), account.size()) >= sizeof(escapedAccount) ||
        db.EscapeString(escapedCompanion, companion.c_str(), companion.size()) >= sizeof(escapedCompanion))
        return;
    db.Query("DELETE FROM messenger_list%s WHERE account='%s' AND companion = '%s'",
             get_table_postfix(), escapedAccount, escapedCompanion);
    __RemoveFromList(account, companion);
    SendMessengerChange(11, account, companion);
}

// 080e0ff0
void MessengerManager::UpdateMobile(const char* name, const char* mobile)
{
    if (!name || !*name || !mobile)
        return;

    __UpdateMobile(name, mobile);
    char packet[59]{};
    packet[0] = 14;
    std::strncpy(packet + 1, name, 24);
    std::strncpy(packet + 26, mobile, 32);
    P2P_MANAGER::instance().Send(packet, sizeof(packet));
}

// 080e18e0
int MessengerManager::GetSMSState(const char* name)
{
    if (m_set_loginAccount.count(name)) {
        sys_log(0, "CanSendSMS: %s is already login", name);
        return 1;
    }
    if (!m_map_stMobile.count(name)) {
        sys_log(0, "CanSendSMS: %s do not have mobile number", name);
        return 2;
    }
    return 0;
}

// 080e1a20
void MessengerManager::SendSMS(const char* name, const char* target, const char* text)
{
    const auto recipient = m_map_stMobile.find(target);
    if (recipient == m_map_stMobile.end() || !db_clientdesc)
        return;

    struct {
        char from[25];
        char to[25];
        char mobile[33];
        char message[81];
    } packet{};
    static_assert(sizeof(packet) == 164);
    std::strncpy(packet.from, name, sizeof(packet.from) - 1);
    std::strncpy(packet.to, target, sizeof(packet.to) - 1);
    std::strncpy(packet.mobile, recipient->second.c_str(), sizeof(packet.mobile) - 1);
    std::strncpy(packet.message, text, sizeof(packet.message) - 1);
    db_clientdesc->DBPacket(45, 0, &packet, sizeof(packet));
}
