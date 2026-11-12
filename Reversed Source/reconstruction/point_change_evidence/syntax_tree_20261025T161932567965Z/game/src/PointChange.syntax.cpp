// Complete staged0812e340. Missing ORIGINAL declarations/real dependencies are
// listed separately; no replacement methods or reduced dispatch are supplied.
#include "char.h"
#include "constants.h"
#include "config.h"
#include "locale_service.h"
#include "questmanager.h"
#include "guild.h"
#include "party.h"
#include "log.h"
#include "dev_log.h"
#include "desc.h"
#include "packet.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
namespace point_change_detail {
inline int add(int a,int b){return static_cast<int>(static_cast<unsigned int>(a)+static_cast<unsigned int>(b));}
inline int sub(int a,int b){return static_cast<int>(static_cast<unsigned int>(a)-static_cast<unsigned int>(b));}
inline int mul(int a,int b){return static_cast<int>(static_cast<unsigned int>(a)*static_cast<unsigned int>(b));}
inline int neg(int a){return static_cast<int>(0U-static_cast<unsigned int>(a));}
}
void CHARACTER::PointChange(BYTE type,int amount,bool bAmount,bool bBroadcast) {
    using namespace point_change_detail;
    long val=0;
    switch(type) {
    case 0:return;
    case 1: {
        if(add(amount,m_points.level)>99)return;
        SetLevel(static_cast<BYTE>(add(amount,m_points.level)));
        BYTE level=m_points.level;val=level;
        int next=level<99?static_cast<int>(exp_table[level]):2000000000;
        sys_log(0,"LEVELUP: %s %d NEXT EXP %d",GetName(),static_cast<unsigned int>(level),next);
        unsigned int currentLevel=m_points.level;
        next=currentLevel<99?static_cast<int>(exp_table[currentLevel]):2000000000;
        PointChange(4,next,false,false);
        if(amount) {
            unsigned int pid=static_cast<unsigned int>(m_dwPlayerID);
            quest::CQuestManager::instance().LevelUp(pid);
            long realPlayTime=GetRealPoint(31);
            DWORD ticks=get_dword_time();DWORD start=m_dwPlayStartTime;
            const char* name=GetName();const char* postfix=get_table_postfix();
            int playTime=add(static_cast<int>(realPlayTime),static_cast<int>(static_cast<unsigned int>(ticks-start)/60000U));
            LogManager::instance().Query("REPLACE INTO levellog%s (name, level, time, playtime) VALUES('%s', %lu, NOW(), %d)",postfix,name,static_cast<DWORD>(val),playTime);
            if(m_pGuild)m_pGuild->LevelChange(m_dwPlayerID,m_points.level);
            if(m_pkParty)m_pkParty->RequestSetMemberLevel(m_dwPlayerID,m_points.level);
        }
        break;
    }
    case 2:case 24:val=GetRealPoint(type);break;
    case 3: {
        // Both old locals are signedint32, although the stored exp field isDWORD.
        int previous=static_cast<int>(m_points.exp);
        unsigned int level=m_points.level;
        int next=level<99?static_cast<int>(exp_table[level]):2000000000;
        if(LC_IsNewCIBN()) {
            switch(m_eOverTime) {
            case OT_NONE:dev_log("char.cpp",2920,"PointChange",0x800,const_cast<char*>("<EXP_LOG> %s = NONE"),GetName());break;
            case OT_3HOUR:amount/=2;dev_log("char.cpp",2925,"PointChange",0x800,const_cast<char*>("<EXP_LOG> %s = 3HOUR"),GetName());break;
            case OT_5HOUR:amount=0;dev_log("char.cpp",2930,"PointChange",0x800,const_cast<char*>("<EXP_LOG> %s = 5HOUR"),GetName());break;
            default:break;
            }
        }
        if(amount<0 && previous<neg(amount)) {
            sys_log(1,"%s AMOUNT < 0 %ld, CUR EXP: %ld",GetName(),static_cast<long>(neg(amount)),static_cast<long>(previous));
            amount=neg(previous);m_points.exp=0;val=0;
        } else {
            if(m_points.level>98)return;
            if(test_server)ChatPacket(1,"You have gained %d exp.",amount);
            int balance=0;
            int exp=add(previous,amount);
            int excess=sub(exp,next);
            sys_log(1,"PRE EXP: %d + %d >= %d (%d)",previous,amount,next,excess);
            if(exp<next)m_points.exp=static_cast<DWORD>(static_cast<unsigned int>(exp));
            else {
                amount=sub(next,previous);m_points.exp=0;exp=next;balance=excess;
            }
            // Only three format operands are stored for this call in assembly;
            // Ghidra's extra operand is a stale priorPRE-log stack slot.
            sys_log(1,"EXP: %s %d / %d",GetName(),exp,next);
            int quarter=next/4;
            long step=GetRealPoint(25);
            if(step>3) {
                _sys_err("PointChange",2975,"%s LEVEL_STEP bigger than 4! (%d)",GetName(),static_cast<int>(step));
                step=4;
            }
            if(exp>=next && step<=3) {
                int count=sub(4,static_cast<int>(step));
                while(count>0){PointChange(25,1,false,true);--count;}
            } else if(exp>=mul(quarter,3) && step<=2) {
                int count=sub(3,static_cast<int>(step));
                while(count>0){PointChange(25,1,false,true);--count;}
            } else if(exp>=mul(quarter,2) && step<=1) {
                int count=sub(2,static_cast<int>(step));
                while(count>0){PointChange(25,1,false,true);--count;}
            } else if(exp>=quarter && step<1)PointChange(25,1,false,false);
            if(balance) {
                sys_log(1,"%s iExpBalance %d",GetName(),balance);
                PointChange(3,balance,false,false);
            }
            val=static_cast<int>(m_points.exp);
        }
        break;
    }
    case 4: {
        unsigned int level=m_points.level;
        val=level<99?static_cast<int>(exp_table[level]):2000000000;
        bAmount=false;break;
    }
    case 5: {
        if(IsDead() || IsStun())return;
        int previous=m_points.hp;
        amount=MIN(sub(m_pointsInstant.iMaxHP,previous),amount);
        val=add(amount,m_points.hp);m_points.hp=static_cast<int>(val);
        BroadcastTargetPacket();
        if(m_pkParty && GetDesc() && val!=previous)m_pkParty->SendPartyInfoOneToAll(this);
        break;
    }
    case 6: {
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));
        long base=GetRealPoint(6);
        long percent=GetPoint(119);
        int bonus=MIN(1500,mul(static_cast<int>(percent),static_cast<int>(base))/100);
        long point=GetPoint(6);long party=GetPoint(92);
        val=add(add(add(static_cast<int>(base),bonus),static_cast<int>(point)),static_cast<int>(party));
        m_pointsInstant.iMaxHP=static_cast<int>(val);break;
    }
    case 7: {
        if(IsDead() || IsStun())return;
        amount=MIN(sub(m_pointsInstant.iMaxSP,m_points.sp),amount);
        val=add(amount,m_points.sp);m_points.sp=static_cast<int>(val);break;
    }
    case 8: {
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));
        long base=GetRealPoint(8);long percent=GetPoint(120);
        int bonus=MIN(800,mul(static_cast<int>(percent),static_cast<int>(base))/100);
        long point=GetPoint(8);long party=GetPoint(104);
        val=add(add(add(static_cast<int>(base),bonus),static_cast<int>(point)),static_cast<int>(party));
        m_pointsInstant.iMaxSP=static_cast<int>(val);break;
    }
    case 9: {
        if(IsDead() || IsStun())return;
        int previous=m_points.stamina;
        amount=MIN(sub(m_pointsInstant.iMaxStamina,previous),amount);
        val=add(amount,m_points.stamina);m_points.stamina=static_cast<int>(val);
        if(!val)SetNowWalking(true);
        else if(!previous)SetNowWalking(m_bWalking);
        if(amount<0 && val!=0)return;
        break;
    }
    case 10:val=add(amount,m_pointsInstant.iMaxStamina);m_pointsInstant.iMaxStamina=static_cast<int>(val);break;
    case 11: {
        if(add(amount,static_cast<int>(m_points.gold))<2000000000) {
            if(LC_IsNewCIBN() && amount>0) {
                switch(m_eOverTime) {
                case OT_NONE:dev_log("char.cpp",3192,"PointChange",0x800,const_cast<char*>("<GOLD_LOG> %s = NONE"),GetName());break;
                case OT_3HOUR:amount/=2;dev_log("char.cpp",3197,"PointChange",0x800,const_cast<char*>("<GOLD_LOG> %s = 3HOUR"),GetName());break;
                case OT_5HOUR:amount=0;dev_log("char.cpp",3202,"PointChange",0x800,const_cast<char*>("<GOLD_LOG> %s = 5HOUR"),GetName());break;
                default:break;
                }
            }
            val=add(amount,static_cast<int>(m_points.gold));m_points.gold=val;
        } else {
            const char* name=GetName();
            _sys_err("PointChange",3181,"[OVERFLOW_GOLD] OriGold %ld AddedGold %ld id %lu Name %s ",m_points.gold,static_cast<long>(amount),m_dwPlayerID,name);
            int gold=static_cast<int>(m_points.gold);DWORD y=static_cast<DWORD>(GetY());DWORD pid=m_dwPlayerID;DWORD x=static_cast<DWORD>(GetX());
            LogManager::instance().CharLog(pid,x,y,static_cast<DWORD>(static_cast<unsigned int>(add(amount,gold))),"OVERFLOW_GOLD","");
            // Original val remains initialized0; do not report currentgold here.
        }
        break;
    }
    case 16:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));
        val=GetPoint(type);PointChange(20,amount,false,false);break;
    case 25: {
        if(amount<=0){val=GetPoint(25);break;}
        int step=add(amount,static_cast<int>(GetPoint(25)));
        BYTE level;
        if(step>=1 && step<=3) {
            if(m_points.level<91)PointChange(26,1,false,false);
        } else if(step==4) {
            // Preserve actualGetJob call order: upperbound first, lower second.
            BYTE upperJob=GetJob();BYTE lowerJob=GetJob();
            int hp=number_ex(JobInitialPoints[lowerJob].hp_per_lv_begin,JobInitialPoints[upperJob].hp_per_lv_end,"char.cpp",3023);
            upperJob=GetJob();lowerJob=GetJob();
            int sp=number_ex(JobInitialPoints[lowerJob].sp_per_lv_begin,JobInitialPoints[upperJob].sp_per_lv_end,"char.cpp",3024);
            m_points.iRandomSP=add(m_points.iRandomSP,sp);m_points.iRandomHP=add(m_points.iRandomHP,hp);
            if(m_points.skill_group) {
                level=m_points.level;
                if(level>4){PointChange(28,1,false,false);level=m_points.level;}
                if(level>8)PointChange(27,1,false,false);
            }
            PointChange(6,hp,false,false);PointChange(8,sp,false,false);
            step=0;PointChange(1,1,false,true);
        }
        level=m_points.level;
        DWORD item=level<11?27001:27002;
        AutoGiveItem(item,static_cast<BYTE>(2),-1,true);
        PointChange(5,sub(m_pointsInstant.iMaxHP,m_points.hp),false,false);
        PointChange(7,sub(m_pointsInstant.iMaxSP,m_points.sp),false,false);
        PointChange(9,sub(m_pointsInstant.iMaxStamina,m_points.stamina),false,false);
        SetPoint(25,step);SetRealPoint(25,step);Save();val=step;break;
    }
    case 26:case 27:case 28:case 112:case 113:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);SetRealPoint(type,val);break;
    case 83:case 84:case 85:case 86:
        if(add(static_cast<int>(GetPoint(type)),amount)>100) {
            _sys_err("PointChange",3355,"BONUS exceeded over 100!! point type: %d name: %s amount %d",static_cast<unsigned int>(type),GetName(),amount);
            amount=sub(100,static_cast<int>(GetPoint(type)));
        }
        goto additive;
    case 88:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);
        if(val==0)m_pointsInstant.dwImmuneFlag&=0xfffffffeUL;else m_pointsInstant.dwImmuneFlag|=1;break;
    case 89:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);
        if(val==0)m_pointsInstant.dwImmuneFlag&=0xfffffffdUL;else m_pointsInstant.dwImmuneFlag|=2;break;
    case 90:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);
        if(val==0)m_pointsInstant.dwImmuneFlag&=0xfffffffbUL;else m_pointsInstant.dwImmuneFlag|=4;break;
    case 95:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));PointChange(18,amount,false,false);val=GetPoint(type);break;
    case 96:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));PointChange(16,amount,false,false);val=GetPoint(type);break;
    case 97:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));PointChange(22,amount,false,false);val=GetPoint(type);break;
    case 98:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));PointChange(23,amount,false,false);val=GetPoint(type);break;
    case 108:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);SetPolymorph(static_cast<DWORD>(static_cast<unsigned int>(val)),false);break;
    case 109:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);MountVnum(static_cast<DWORD>(static_cast<unsigned int>(val)));break;
    case 114:case 115:case 116:case 117:case 118:
        if(add(static_cast<int>(GetPoint(type)),amount)>100) {
            _sys_err("PointChange",3333,"MALL_BONUS exceeded over 100!! point type: %d name: %s amount %d",static_cast<unsigned int>(type),GetName(),amount);
            amount=sub(100,static_cast<int>(GetPoint(type)));
        }
        goto additive;
    case 119:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);PointChange(6,0,false,false);break;
    case 120:
        SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);PointChange(8,0,false,false);break;
    case 125:case 126:SetPoint(type,amount);val=GetPoint(type);break;
    // EXACT original simpleadditive dispatch targets from127-entry jump table.
    case 12:case 13:case 14:case 15:case 17:case 18:case 19:case 20:case 21:case 22:case 23:
    case 32:case 33:case 34:case 35:case 36:case 37:case 38:case 39:case 40:case 41:case 42:case 43:case 44:case 45:case 46:case 47:case 48:
    case 53:case 54:case 55:case 56:case 57:case 59:case 60:case 61:case 62:case 63:case 64:case 65:case 66:case 67:case 68:case 69:case 70:case 71:case 72:case 73:case 74:case 75:case 76:case 77:case 78:case 79:case 80:case 81:case 82:
    case 87:case 91:case 92:case 93:case 94:case 99:case 100:case 101:case 102:case 103:case 104:case 110:case 111:case 121:case 122:case 123:case 124:
        goto additive;
    default:
        _sys_err("PointChange",3445,"CHARACTER::PointChange: %s: unknown point change type %d",GetName(),static_cast<unsigned int>(type));return;
    }
    goto epilogue;
additive:
    SetPoint(type,add(static_cast<int>(GetPoint(type)),amount));val=GetPoint(type);
epilogue:
    switch(type){case 1:case 12:case 13:case 14:case 15:ComputeBattlePoints();break;default:break;}
    if(type!=5 || amount!=0) {
        DESC* descriptor=GetDesc();
        if(descriptor) {
            TPacketGCPointChange pack;pack.header=17;pack.dwVID=m_vid;pack.type=type;
            pack.amount=bAmount?amount:0;pack.value=val;
            if(bBroadcast)CEntity::PacketAround(&pack,sizeof(pack),0);
            else descriptor->Packet(&pack,sizeof(pack));
        }
    }
}
