// Missing ORIGINAL declarations only. Merge into the named complete classes;
// these are not implementations and do not close their behavior dependencies.
// CHARACTER, char.h (all non-const):
void PointChange(BYTE type,int amount,bool bAmount,bool bBroadcast);
void ComputeBattlePoints();
void ComputePoints();
void Save();
LPITEM AutoGiveItem(DWORD vnum,BYTE count,int iRarePct,bool bMsg);
void SetPolymorph(DWORD vnum,bool maintainStat);
void MountVnum(DWORD vnum);
// quest::CQuestManager, questmanager.h (questmanager.cpp488/081a3310):
void LevelUp(unsigned int pc);
// CGuild, guild.h187/0808f5d0:
void LevelChange(DWORD pid,BYTE level);
// LogManager, log.h/log.cpp (PUBLIC declarations):
// MOVE existing Query from private to public; never add a duplicate declaration.
void Query(const char* format,...);
void CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint);
// constants.h: mutable pointer to original read-only level table. Definition,
// initial selection and locale reassignment remain separate actual-data work.
extern const DWORD* exp_table;
// Default function arguments are deliberately not invented from DWARF.
