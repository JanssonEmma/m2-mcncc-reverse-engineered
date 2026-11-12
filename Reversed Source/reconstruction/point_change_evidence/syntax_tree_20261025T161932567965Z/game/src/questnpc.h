//Complete old NPC data declaration; the target has20 event slots.
#ifndef MCNCC_GAME_QUESTNPC_H
#define MCNCC_GAME_QUESTNPC_H
#include "questpc.h"
namespace quest {
class NPC {
public:
    typedef std::map<int,AStateScriptType> AQuestScriptType;
    typedef std::map<unsigned int,AQuestScriptType> QuestMapType;
    typedef std::map<int,std::vector<AArgScript> > AArgQuestScriptType;
    typedef std::map<unsigned int,AArgQuestScriptType> ArgQuestMapType;
    NPC();
    ~NPC();
protected:
    unsigned int m_vnum;
    QuestMapType m_mapOwnQuest[20];
    ArgQuestMapType m_mapOwnArgQuest[20];
};
}
#endif
