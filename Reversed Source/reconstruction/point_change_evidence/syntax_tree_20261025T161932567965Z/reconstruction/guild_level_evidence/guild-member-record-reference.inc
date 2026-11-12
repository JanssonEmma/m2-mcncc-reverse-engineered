// REFERENCE ONLY: current guild.h already contains this complete old record.
// Do not add a reduced13-byte member structure or duplicate this declaration.
// Natural alignment; original target sizeof20, name@16 with old string ABI.
typedef struct SGuildMember {
    DWORD pid;
    BYTE grade,is_general,job,level;
    DWORD offer_exp;
    BYTE _dummy;
    std::string name;
    SGuildMember(CHARACTER* ch,BYTE grade,DWORD offer_exp);
    SGuildMember(DWORD pid,BYTE grade,BYTE is_general,BYTE job,BYTE level,DWORD offer_exp,char* name);
} TGuildMember;
