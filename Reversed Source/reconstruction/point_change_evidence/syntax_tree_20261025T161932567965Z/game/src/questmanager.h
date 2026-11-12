//Complete old quest manager data (target216 bytes); most methods remain pending.
#ifndef MCNCC_GAME_QUESTMANAGER_H
#define MCNCC_GAME_QUESTMANAGER_H
#include "questnpc.h"
#include "../../common/singleton.h"
#include <set>
#include <ext/hash_map>
class CDungeon;
class CHARACTER;
namespace quest {
class CQuestManager : public singleton<CQuestManager> {
public:
    CQuestManager();
    virtual ~CQuestManager();
    int GetEventFlag(const std::string& name);
    typedef std::map<std::string,int> TEventNameMap;
    TEventNameMap m_mapEventName;
private:
    CDungeon* m_pSelectedDungeon;
    DWORD m_dwServerTimerArg;
    std::map<std::pair<std::string,DWORD>,LPEVENT> m_mapServerTimer;
    int m_iRunningEventIndex;
    std::map<std::string,int> m_mapEventFlag;
    lua_State* L;
    bool m_bNoSend;
    std::set<unsigned int> m_registeredNPCVnum;
    std::map<unsigned int,NPC> m_mapNPC;
    std::map<std::string,unsigned int> m_mapNPCNameID;
    std::map<std::string,unsigned int> m_mapTimerID;
    QuestState* m_CurrentRunningState;
    std::map<unsigned int,PC> m_mapPC;
    CHARACTER* m_pCurrentCharacter;
    CHARACTER* m_pCurrentNPCCharacter;
    CHARACTER* m_pCurrentPartyMember;
    PC* m_pCurrentPC;
    std::string m_strScript;
    int m_iCurrentSkin;
    struct stringhash { std::size_t operator()(const std::string&) const; };
    __gnu_cxx::hash_map<std::string,int,stringhash,std::equal_to<std::string>,std::allocator<int> > m_hmQuestName;
    __gnu_cxx::hash_map<unsigned int,std::vector<char>,__gnu_cxx::hash<unsigned int>,std::equal_to<unsigned int>,std::allocator<std::vector<char> > > m_hmQuestStartScript;
    std::map<unsigned int,std::string> m_mapQuestNameByIndex;
    std::vector<unsigned char> compression_vector;
    bool m_bError;

public:
    // Syntax overlay declaration only; logger-quest-dwarf.json:_ZN5quest13CQuestManager7LevelUpEj
    void LevelUp(unsigned int pc);
};
}
#endif
