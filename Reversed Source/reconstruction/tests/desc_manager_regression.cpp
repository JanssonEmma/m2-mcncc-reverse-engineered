#include <map>
#include <set>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/desc_manager.h"
#undef private
#include <cassert>
#include <cstdio>
// Test-only lifecycle boundary: production destructor/Destroy is unrecovered.
DESC_MANAGER::~DESC_MANAGER() {}
int main() {
    stringhash hash;
    assert(hash(std::string("",0))==0UL);
    assert(hash(std::string("\x61",1))==97UL);
    assert(hash(std::string("\x61\x62\x63",3))==2230358368UL);
    assert(hash(std::string("\x41",1))==65UL);
    assert(hash(std::string("\x61\x00\x62",3))==1727029675UL);
    assert(hash(std::string("\xff\x80\x01",3))==1551415318UL);
    assert(hash(std::string("\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a",26))==3300669417UL);
    int token1=1,token2=2;
    DESC* first=reinterpret_cast<DESC*>(&token1);
    DESC* second=reinterpret_cast<DESC*>(&token2); // opaque values, never dereferenced
    assert(!DESC_MANAGER::instance_ptr());
    {
        DESC_MANAGER manager;
        assert(DESC_MANAGER::instance_ptr()==&manager);
        assert(!manager.m_iSocketsConnected && !manager.m_iHandleCount && !manager.m_iLocalUserCount);
        assert(!manager.m_bDisconnectInvalidCRC && manager.m_map_loginName.empty());
        for(int i=0;i<4;++i) assert(!manager.m_aiEmpireUserCount[i]);
        assert(!manager.FindByLoginName("absent"));
        manager.ConnectAccount("Alice",first); manager.ConnectAccount("Alice",second);
        manager.ConnectAccount("alice",second);
        assert(manager.FindByLoginName("Alice")==first && manager.FindByLoginName("alice")==second);
        std::string binary("a\0b",3); manager.ConnectAccount(binary,first);
        assert(manager.FindByLoginName(binary)==first && !manager.FindByLoginName("a"));
        manager.DisconnectAccount("absent"); manager.DisconnectAccount("Alice");
        assert(!manager.FindByLoginName("Alice") && manager.FindByLoginName("alice")==second);
        manager.m_map_handle[-1]=first; manager.m_map_handshake[0xffffffffUL]=second;
        assert(manager.FindByHandle(0xffffffffUL)==first && !manager.FindByHandle(1));
        assert(manager.FindByHandshake(0xffffffffUL)==second && !manager.FindByHandshake(1));
        manager.m_set_pkDesc.insert(first); manager.m_set_pkClientDesc.insert(second);
        assert(&manager.GetClientSet()==&manager.m_set_pkDesc && manager.GetClientSet().count(first));
        manager.m_iLocalUserCount=9; manager.m_bDisconnectInvalidCRC=true;
        manager.Initialize();
        assert(!manager.m_iLocalUserCount && !manager.m_bDisconnectInvalidCRC);
        assert(manager.FindByLoginName("alice")==second && manager.FindByHandle(0xffffffffUL)==first);
    }
    assert(!DESC_MANAGER::instance_ptr());
    std::puts("descriptor manager passed: exact hash vectors, unique login insertion, lookups and reset scope");
}
