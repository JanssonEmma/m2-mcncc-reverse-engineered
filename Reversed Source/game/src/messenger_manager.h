#ifndef MCNCC_GAME_MESSENGER_MANAGER_H
#define MCNCC_GAME_MESSENGER_MANAGER_H

#include "db.h"
#include <set>

class MessengerManager : public singleton<MessengerManager> {
public:
    typedef std::string keyT;
    typedef const std::string& keyA;

    MessengerManager();
    virtual ~MessengerManager();

    void P2PLogin(keyA account);
    void P2PLogout(keyA account);
    void Login(keyA account);
    void Logout(keyA account);
    void RequestToAdd(CHARACTER* character, CHARACTER* target);
    void AuthToAdd(keyA account, keyA companion, bool deny);
    void __AddToList(keyA account, keyA companion);
    void AddToList(keyA account, keyA companion);
    void __RemoveFromList(keyA account, keyA companion);
    void RemoveFromList(keyA account, keyA companion);
    void __UpdateMobile(const char* name, const char* mobile);
    void UpdateMobile(const char* name, const char* mobile);
    void Initialize();
    int GetSMSState(const char* name);
    void SendSMS(const char* name, const char* target, const char* text);

private:
    typedef std::set<keyT> AccountSet;
    typedef std::map<keyT, AccountSet> RelationMap;

    void SendList(keyA account);
    void SendLogin(keyA account, keyA companion);
    void SendLogout(keyA account, keyA companion);
    void LoadList(SQLMsg* msg);
    void Destroy();

    // Original DWARF field order, size 64 in the original C++ library ABI.
    std::map<keyT, keyT> m_map_stMobile;
    AccountSet m_set_loginAccount;
    RelationMap m_Relation;
    RelationMap m_InverseRelation;
    std::set<DWORD> m_set_requestToAdd;
};

#endif
