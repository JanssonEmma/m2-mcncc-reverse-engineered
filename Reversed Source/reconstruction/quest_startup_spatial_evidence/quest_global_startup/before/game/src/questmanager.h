//Complete old quest manager data (target216 bytes); most methods remain pending.
#ifndef MCNCC_GAME_QUESTMANAGER_H
#define MCNCC_GAME_QUESTMANAGER_H
#include "questnpc.h"
#include "../../common/singleton.h"
#include <set>
#include <cstddef>
#include <ext/hash_map>
struct luaL_reg;
class CDungeon;
class CHARACTER;
namespace quest {
class CQuestManager : public singleton<CQuestManager> {
public:
    CHARACTER* GetCurrentNPCCharacterPtr();
    unsigned int GetCurrentNPCRace();

    // Original public questmanager.h SetSkinStyle(int).
    void SetSkinStyle(int);

    // Original complete public APIs; existing full216byte owner unchanged.
    bool CanStartQuest(unsigned int quest_index);
    void WriteRunningStateToSyserr();

// Original public variadic declaration (rawELF _ZN5quest13CQuestManager10QuestErrorEPKciS2_z).
    void QuestError(const char*,int,const char*,...);

// Original public APIs, existing full216byteold record retained.
    QuestState OpenState(const std::string&,int);
    void CloseState(QuestState&);
    void ClearScript();
    void AddScript(const std::string&);
    void SendScript();

// Original declarations recovered from full CQM API; complete owner/data retained.
    void SendEventFlagList(CHARACTER*);
    void RequestSetEventFlag(const std::string&,int);
    void BroadcastEventFlagOnLogin(CHARACTER*);

// Original public NONCONST getter questmanager.h122, inline at0805f12c..138.
// Add to existing COMPLETE CQuestManager public section, no class/field replacement.
CHARACTER* GetCurrentCharacterPtr(){return m_pCurrentCharacter;}

    // Original full API state queries; Lua state remains original private owner member.
    const char* GetQuestStateName(const std::string& quest_name,int state_index);
    int GetQuestStateIndex(const std::string& quest_name,const std::string& state_name);

    // Original public owning cleanup API, actualSDK only; no class/data replacement.
    void Destroy();
    void AddLuaFunctionTable(const char* name, luaL_reg* functions);
    void BuildStateIndexToName(const char* quest_name);
    void RegisterNPCVnum(DWORD vnum);
    unsigned int LoadTimerScript(const std::string& name);

    CQuestManager();
    virtual ~CQuestManager();
    int GetEventFlag(const std::string& name);
    PC* GetPC(unsigned int pc);
    PC* GetPCForce(unsigned int pc);
    void LoadStartQuest(const std::string& name,unsigned int index);
    void RegisterQuest(const std::string& name,unsigned int index);
    unsigned int GetQuestIndexByName(const std::string& name);
    const std::string& GetQuestNameByIndex(unsigned int index);
    unsigned int FindNPCIDByName(const std::string& name);
    const std::string& GetCurrentQuestName();

    typedef std::map<std::string,int> TEventNameMap;
    TEventNameMap m_mapEventName;
private:
// Original private QuestState& bridges, completeoldCQM record/API retained.
    void GotoPauseState(QuestState&);
    void GotoInputState(QuestState&);
    void GotoSelectItemState(QuestState&);
    void GotoSelectState(QuestState&);

// Original PRIVATE declaration, questmanager.h181 DWARFaccess3.
    void GotoEndState(QuestState&);

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
    // Original questmanager.h inline operator. Target size_t was uint32.
struct stringhash {
    std::size_t operator()(const std::string& value) const {
        const unsigned char* current=reinterpret_cast<const unsigned char*>(value.c_str());
        const unsigned char* end=current+value.size();
        unsigned int hash=0;
        while(current<end) { hash*=16777619U;hash^=*current++; }
        return hash;
    }
};
    __gnu_cxx::hash_map<std::string,int,stringhash,std::equal_to<std::string>,std::allocator<int> > m_hmQuestName;
    __gnu_cxx::hash_map<unsigned int,std::vector<char>,__gnu_cxx::hash<unsigned int>,std::equal_to<unsigned int>,std::allocator<std::vector<char> > > m_hmQuestStartScript;
    std::map<unsigned int,std::string> m_mapQuestNameByIndex;
    std::vector<unsigned char> compression_vector;
    bool m_bError;
};
}
#endif
