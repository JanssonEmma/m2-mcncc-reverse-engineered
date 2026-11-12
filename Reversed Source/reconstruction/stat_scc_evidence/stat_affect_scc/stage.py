from pathlib import Path
import json,shutil
p=Path('work/stat_affect_scc');m=json.loads((p/'original-metadata.json').read_text());literal=m['strings'][0]['escaped']
p.joinpath('char_affect.cpp.full.candidate').write_text('''// Complete original mutations. Not independently closed: real stat/combat SCC required.
bool CHARACTER::RemoveAffect(CAffect* affect)
{
    if (!affect) return false;
    m_list_pkAffect.remove(affect);
    ComputeAffect(affect,false);
    if (affect->dwFlag || affect->bApplyOn==19 || affect->bApplyOn==17) UpdatePacket();
    ComputePoints();
    CheckMaximumPoints();
    if (test_server) sys_log(0,"Flag %d Apply %d ",affect->dwFlag,static_cast<unsigned int>(affect->bApplyOn));
    LPDESC d=GetDesc();
    if (d) SendAffectRemovePacket(d,m_dwPlayerID,affect->dwType,affect->bApplyOn);
    delete affect;
    return true;
}
bool CHARACTER::RemoveAffect(DWORD type)
{
    if (type==512) sys_log(0,"XXX RemoveAffect SKILL_BOOK_BONUS from %s",GetName());
    if (type==223) ChatPacket(1,locale_find("LITERAL",g_iUseLocale));
    bool flag=false;
    for (;;) {
        CAffect* affect=FindAffect(type,0);
        if (!affect) break;
        RemoveAffect(affect);
        flag=true;
    }
    return flag;
}
'''.replace('LITERAL',literal))
for name in ('RefreshAffect','ComputeAffect'):shutil.copyfile('work/stat_compute_scc/char_affect.cpp.'+name+'.candidate',p/('char_affect.cpp.'+name+'.candidate'))
p.joinpath('char_skill.cpp.StartMuyeong.candidate').write_text('''// Original08073890, full scheduling body; callback below remains gameplay/lifetime frontier.
void CHARACTER::StartMuyeongEvent()
{
    if (!m_pkMuyeongEvent) {
        char_event_info* info;
        DWORD id=__AllocEventInfo(4,reinterpret_cast<void**>(&info));
        info->ch=this;
        m_pkMuyeongEvent=event_create_ex(skill_muyoung_event,id,passes_per_sec,const_cast<char*>("char_skill.cpp"),0xa60);
    }
}
''')
p.joinpath('char_skill.cpp.muyoung.candidate').write_text('''// Full original logical record20; const set reference at+16 is NOT owning.
// Includes char.h/sectree.h/battle.h, coreutils/log, set and wrap32 helpers.
struct FFindNearVictim {
    LPCHARACTER m_pkChrCenter;
    LPCHARACTER m_pkChrNextTarget;
    LPCHARACTER m_pkChrAttacker;
    int m_count;
    const std::set<CHARACTER*>& m_excepts_set;
    FFindNearVictim(LPCHARACTER center,LPCHARACTER attacker,const std::set<CHARACTER*>& excepts):
        m_pkChrCenter(center),m_pkChrNextTarget(0),m_pkChrAttacker(attacker),m_count(0),m_excepts_set(excepts) {}
    void operator()(LPENTITY ent) {
        if (!ent->IsType(0)) return;
        LPCHARACTER ch=static_cast<LPCHARACTER>(ent);
        if (m_excepts_set.find(ch)!=m_excepts_set.end() || m_pkChrCenter==ch || !battle_is_attackable(m_pkChrAttacker,ch)) return;
        int dx=apply_item_detail::sub(m_pkChrCenter->GetX(),ch->GetX());
        int absolute=dx<0?apply_item_detail::negate(dx):dx;
        if (absolute>=1001) return;
        int dy=apply_item_detail::sub(m_pkChrCenter->GetY(),ch->GetY());
        absolute=dy<0?apply_item_detail::negate(dy):dy;
        if (absolute>=1001) return;
        if (static_cast<float>(DISTANCE_APPROX(dx,dy))<1000.0f) {
            m_count=apply_item_detail::add(m_count,1);
            if (m_count!=1 && number_ex(1,m_count,"char_skill.cpp",0x359)!=1) return;
            m_pkChrNextTarget=ch;
        }
    }
};
long skill_muyoung_event(LPEVENT event)
{
    LPCHARACTER ch=static_cast<char_event_info*>(event->info)->ch;
    if (!ch->IsAffectFlag(27)) { ch->StopMuyeongEvent(); return 0; }
    // INTENTIONAL ORIGINAL DANGLING TEMPORARY! Header freed080736cd BEFORE traversal.
    // Do not replace with a static/local owning set to make this callback appear safe.
    FFindNearVictim f(ch,ch,std::set<CHARACTER*>());
    if (ch->GetSectree()) {
        ch->GetSectree()->ForEachAround(f);
        if (f.m_pkChrNextTarget) {
            ch->CreateFly(16,f.m_pkChrNextTarget);
            ch->ComputeSkill(78,f.m_pkChrNextTarget,0);
        }
    }
    return apply_item_detail::mul(passes_per_sec,3);
}
''')
q=p/'closed_leaves';q.mkdir(exist_ok=True)
q.joinpath('char_affect.cpp.additions').write_text('''#include "desc.h"
#include "desc_client.h"
#include "packet.h"
#include "../../common/tables.h"
void SendAffectRemovePacket(LPDESC d,DWORD pid,DWORD type,BYTE point)
{
    TPacketGCAffectRemove ptoc;
    ptoc.bApplyOn=point;ptoc.dwType=type;ptoc.bHeader=127;
    d->Packet(&ptoc,6);
    TPacketGDRemoveAffect ptod;
    ptod.dwType=type;ptod.dwPID=pid;ptod.bApplyOn=point;
    db_clientdesc->DBPacket(33,0,&ptod,9);
}
bool CHARACTER::IsAffectFlag(DWORD flag)
{
    int shifted=static_cast<int>(static_cast<unsigned int>(flag)-1u);
    return ((m_afAffectFlag.bits[shifted>>5]>>(static_cast<unsigned int>(shifted)&31u))&1u)!=0;
}
''')
q.joinpath('char_battle.cpp.additions').write_text('''#include "packet.h"
void CHARACTER::CreateFly(BYTE type,LPCHARACTER victim)
{
    TPacketGCCreateFly packet;
    packet.bHeader=70;packet.bType=type;
    packet.dwStartVID=static_cast<DWORD>(m_vid);packet.dwEndVID=static_cast<DWORD>(victim->m_vid);
    PacketAround(&packet,10,0);
}
''')
q.joinpath('packet.h.additions').write_text('''#pragma pack(push,1)
typedef struct packet_fly { BYTE bHeader; BYTE bType; DWORD dwStartVID; DWORD dwEndVID; } TPacketGCCreateFly;
typedef struct { BYTE bHeader; DWORD dwType; BYTE bApplyOn; } TPacketGCAffectRemove;
#pragma pack(pop)
''')
q.joinpath('tables.h.additions').write_text('''#pragma pack(push,1)
typedef struct SPacketGDRemoveAffect { DWORD dwPID; DWORD dwType; BYTE bApplyOn; } TPacketGDRemoveAffect;
#pragma pack(pop)
''')
q.joinpath('char.h.additions').write_text('''bool IsAffectFlag(DWORD flag);
void CreateFly(BYTE type,LPCHARACTER victim);
''')
print('Full mutations/callback and independently closed3-leaf source fragments staged only')
