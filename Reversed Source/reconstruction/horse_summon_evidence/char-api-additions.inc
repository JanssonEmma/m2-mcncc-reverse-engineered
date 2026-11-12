void HorseSummon(bool summon,bool fromFar);
void SetRider(LPCHARACTER rider);
void SetRotation(float rotation);
void SetEmpire(BYTE empire);
BYTE GetEmpire() { return m_bEmpire; }
LPCHARACTER GetVictim();
bool Show(long map,long x,long y,long z,bool spawnMotion);
bool Goto(long x,long y);
void SendMovePacket(BYTE function,BYTE argument,DWORD x,DWORD y,DWORD duration,DWORD time,int rotation);
void SetProto(const CMob* mob);
void StartAffectEvent();
void MonsterChat(BYTE type);
