// Complete addressed source translations. Runtime closure remains in RECOVERY.md.
#include "char.h"
#include "constants.h"
#include "mob_manager.h"
#include "item.h"
#include "affect.h"
#include "guild.h"
#include "questmanager.h"
#include "../../libthecore/include/log.h"
#include <cstring>
#include <climits>
namespace mount_polymorph_detail {
// Explicit target32 arithmetic, with C++03 spelling and no signed overflow.
inline long Signed(unsigned int bits) {
    int value; std::memcpy(&value,&bits,sizeof(value)); return value;
}
inline long Add(long a,long b) { return Signed(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline long Sub(long a,long b) { return Signed(static_cast<unsigned int>(a)-static_cast<unsigned int>(b)); }
inline long Mul(long a,long b) { return Signed(static_cast<unsigned int>(a)*static_cast<unsigned int>(b)); }
}
//08135b10
void CHARACTER::SetPolymorph(DWORD race,bool maintain) {
    if(race<4) { race=0; maintain=false; }
    if(m_dwPolymorphRace==race) return;
    m_dwPolymorphRace=race;
    m_bPolyMaintainStat=maintain;
    sys_log(0,"POLYMORPH: %s race %lu ",GetName(),race);
    if(race) StopRiding(); // Virtual primary vptr slot13; target0816d720.
    m_bAddChrState|=2;
    m_afAffectFlag.bits[0]|=4;
    ViewReencode();
    m_bAddChrState&=0xfd;
    if(!maintain) {
        PointChange(12,0,false,false);
        PointChange(14,0,false,false);
        PointChange(15,0,false,false);
        PointChange(13,0,false,false);
    }
    ComputeBattlePoints();
}
//08136010
void CHARACTER::MountVnum(DWORD vnum) {
    if(m_dwMountVnum==vnum) return;
    m_dwMountVnum=vnum;
    ViewReencode();
    ComputePoints();
}
//0812c520
void CHARACTER::ComputePoints() {
    using namespace mount_polymorph_detail;
    const long stat=GetPoint(26);
    const long reset=GetPoint(112);
    const long skill=GetPoint(28);
    const long subskill=GetPoint(27);
    const long horseSkill=GetPoint(113);
    const long step=GetPoint(25);
    const long attacker=GetPoint(91);
    const long tanker=GetPoint(92);
    const long buffer=GetPoint(103);
    const long skillMaster=GetPoint(104);
    const long haste=GetPoint(110);
    const long defender=GetPoint(111);
    const long hpRecovery=GetPoint(35);
    const long spRecovery=GetPoint(36);
    // Exactly the instant points array; do not clear maxima/parts/other fields.
    std::memset(m_pointsInstant.points,0,sizeof(m_pointsInstant.points));
    SetPoint(26,stat);
    SetPoint(28,skill);
    SetPoint(27,subskill);
    SetPoint(113,horseSkill);
    SetPoint(25,step);
    SetPoint(112,reset);
    long value=GetRealPoint(12);SetPoint(12,value);
    value=GetRealPoint(13);SetPoint(13,value);
    value=GetRealPoint(14);SetPoint(14,value);
    value=GetRealPoint(15);SetPoint(15,value);
    WORD part=GetOriginalPart(0);SetPart(0,part);
    part=GetOriginalPart(1);SetPart(1,part);
    part=GetOriginalPart(2);SetPart(2,part);
    part=GetOriginalPart(3);SetPart(3,part);
    SetPoint(91,attacker);SetPoint(92,tanker);
    SetPoint(103,buffer);SetPoint(104,skillMaster);
    SetPoint(110,haste);SetPoint(111,defender);
    SetPoint(35,hpRecovery);SetPoint(36,spRecovery);
    SetPoint(125,0);SetPoint(126,0);
    int maxHP,maxSP,maxStamina;
    if(!GetDesc()) {
        maxSP=0;maxStamina=0;
        maxHP=static_cast<int>(Signed(static_cast<unsigned int>(m_pkMobData->m_table.dwMaxHP)));
        SetPoint(17,m_pkMobData->m_table.sAttackSpeed);
        SetPoint(19,m_pkMobData->m_table.sMovingSpeed);
        value=m_pkMobData->m_table.sAttackSpeed;
    } else {
        BYTE job=GetJob();
        int random=m_points.iRandomHP;
        int base=JobInitialPoints[job].max_hp;
        value=GetPoint(13);
        job=GetJob();
        maxHP=static_cast<int>(Add(Add(random,base),Mul(value,JobInitialPoints[job].hp_per_ht)));
        job=GetJob();
        random=m_points.iRandomSP;
        base=JobInitialPoints[job].max_sp;
        value=GetPoint(15);
        job=GetJob();
        maxSP=static_cast<int>(Add(Add(random,base),Mul(value,JobInitialPoints[job].sp_per_iq)));
        job=GetJob();
        value=GetPoint(13);
        BYTE coefficientJob=GetJob();
        maxStamina=static_cast<int>(Add(Mul(value,JobInitialPoints[coefficientJob].stamina_per_con),
                                      JobInitialPoints[job].max_stamina));
        SetPoint(19,100);SetPoint(17,100);
        value=GetPoint(110);
        PointChange(17,static_cast<int>(value),false,false);
        value=100;
    }
    SetPoint(21,value);
    if(GetDesc() && m_dwMountVnum) {
        int horse=c_aHorseStat[m_Horse.bLevel].iST;
        value=GetPoint(12);
        if(value<horse) {
            horse=c_aHorseStat[m_Horse.bLevel].iST;value=GetPoint(12);
            PointChange(12,static_cast<int>(Sub(horse,value)),false,false);
        }
        horse=c_aHorseStat[m_Horse.bLevel].iDX;value=GetPoint(14);
        if(value<horse) {
            horse=c_aHorseStat[m_Horse.bLevel].iDX;value=GetPoint(14);
            PointChange(14,static_cast<int>(Sub(horse,value)),false,false);
        }
        horse=c_aHorseStat[m_Horse.bLevel].iHT;value=GetPoint(13);
        if(value<horse) {
            horse=c_aHorseStat[m_Horse.bLevel].iHT;value=GetPoint(13);
            PointChange(13,static_cast<int>(Sub(horse,value)),false,false);
        }
        horse=c_aHorseStat[m_Horse.bLevel].iIQ;value=GetPoint(15);
        if(value<horse) {
            horse=c_aHorseStat[m_Horse.bLevel].iIQ;value=GetPoint(15);
            PointChange(15,static_cast<int>(Sub(horse,value)),false,false);
        }
    }
    ComputeBattlePoints();
    if(maxHP!=m_pointsInstant.iMaxHP) SetRealPoint(6,maxHP);
    PointChange(6,0,false,false);
    if(maxSP!=m_pointsInstant.iMaxSP) SetRealPoint(8,maxSP);
    PointChange(8,0,false,false);
    m_pointsInstant.iMaxStamina=maxStamina;
    m_pointsInstant.dwImmuneFlag=0;
    for(int cell=0;cell<19;++cell) {
        BYTE slot=static_cast<BYTE>(cell);
        if(GetWear(slot)) {
            GetWear(slot)->ModifyPoints(true);
            LPITEM item=GetWear(slot); // Reload after item callbacks.
            DWORD flags=item->GetProto() ? item->GetProto()->dwImmuneFlag : 0;
            m_pointsInstant.dwImmuneFlag|=flags;
        }
    }
    int hp=m_points.hp;
    int maximum=m_pointsInstant.iMaxHP;
    if(maximum<hp) PointChange(5,static_cast<int>(Sub(maximum,hp)),false,false);
    int sp=m_points.sp;
    maximum=m_pointsInstant.iMaxSP;
    if(maximum<sp) PointChange(7,static_cast<int>(Sub(maximum,sp)),false,false);
    UpdatePacket();
    ComputeSkillPoints();
    RefreshAffect();
}
//0806cd70: actual target body is empty, proven by complete4instruction export.
void CHARACTER::ComputeSkillPoints() {}
//0813e780
void CHARACTER::RefreshAffect() {
    DWORD oldHigh=m_afAffectFlag.bits[1];
    std::list<CAffect*>::iterator it=m_list_pkAffect.begin();
    DWORD oldLow=m_afAffectFlag.bits[0];
    long move=GetPoint(19);
    long attack=GetPoint(17);
    while(it!=m_list_pkAffect.end()) {
        CAffect* affect=*it++;
        ComputeAffect(affect,true);
    }
    if(oldLow!=m_afAffectFlag.bits[0] || oldHigh!=m_afAffectFlag.bits[1] ||
       move!=GetPoint(19) || attack!=GetPoint(17)) UpdatePacket();
}
//0813e880
void CHARACTER::ComputeAffect(CAffect* affect,bool add) {
    using namespace mount_polymorph_detail;
    if(add && affect->dwType>=151 && affect->dwType<=162 &&
       (!m_pGuild || !m_pGuild->UnderAnyWar(3))) return;
    DWORD flag=affect->dwFlag;
    if(flag) {
        unsigned int shifted=static_cast<unsigned int>(flag)-1U;
        int index=static_cast<int>(Signed(shifted))>>5;
        unsigned int mask=1U<<(shifted&31U);
        if(add) m_afAffectFlag.bits[index]|=mask;
        else m_afAffectFlag.bits[index]&=static_cast<DWORD>(~mask);
    }
    int amount=static_cast<int>(add?Signed(static_cast<unsigned int>(affect->lApplyValue)):
                                     Sub(0,affect->lApplyValue));
    PointChange(affect->bApplyOn,amount,false,false);
    if(affect->dwType==78) {
        if(add) StartMuyeongEvent();else StopMuyeongEvent();
    }
}
//0816d720: genuine unresolved quest/horse runtime dependencies retained.
bool CHARACTER::StopRiding() {
    if(!CHorseRider::StopRiding()) return false;
    quest::CQuestManager::instance().Unmount(static_cast<unsigned int>(m_dwPlayerID));
    if(IsDead() || IsStun()) {
        m_dwMountVnum=0;
        ComputePoints();
        UpdatePacket();
    } else {
        MountVnum(0);
        HorseSummon(true,false);
    }
    PointChange(12,0,false,false);
    PointChange(14,0,false,false);
    PointChange(13,0,false,false);
    PointChange(15,0,false,false);
    return true;
}
