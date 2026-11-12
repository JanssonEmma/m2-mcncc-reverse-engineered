from pathlib import Path
import json
p=Path('work/stat_apply_scc');rows=json.loads(Path('work/stat_dependency/apply-point-map.json').read_text());ordinary=[r for r in rows if r['apply'] not in (0,3,4,51,77)]
helper='''// C++03, original int/long32 x86 arithmetic. Include <string.h>.
namespace apply_item_detail {
typedef char require_int32[sizeof(int)==4?1:-1];
typedef char require_long32[sizeof(long)==4?1:-1];
inline int bits(unsigned int value) { int result; memcpy(&result,&value,4); return result; }
inline int add(int a,int b) { return bits(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline int sub(int a,int b) { return bits(static_cast<unsigned int>(a)-static_cast<unsigned int>(b)); }
inline int mul(int a,int b) { return bits(static_cast<unsigned int>(a)*static_cast<unsigned int>(b)); }
inline int negate(int value) { return bits(0u-static_cast<unsigned int>(value)); }
inline int removal(int value,bool add) { return add?value:negate(value); }
inline int skill(int value,bool add) { return add?value:bits(static_cast<unsigned int>(value)^0x00800000u); }
}
'''
p.joinpath('wrap32.inc.candidate').write_text(helper)
s='''// Original char.cpp, CHARACTER::ApplyPoint 0812f7d0. Full body; NOT independently link closed.
void CHARACTER::ApplyPoint(BYTE bApplyType,long lVal)
{
    switch (bApplyType) {
    case APPLY_NONE: break;
    case APPLY_CON:
        PointChange(POINT_HT,lVal,false,false);
        PointChange(POINT_MAX_HP,apply_item_detail::mul(lVal,JobInitialPoints[GetJob()].hp_per_ht),false,false);
        PointChange(POINT_MAX_STAMINA,apply_item_detail::mul(lVal,JobInitialPoints[GetJob()].stamina_per_con),false,false);
        break;
    case APPLY_INT:
        PointChange(POINT_IQ,lVal,false,false);
        PointChange(POINT_MAX_SP,apply_item_detail::mul(lVal,JobInitialPoints[GetJob()].sp_per_iq),false,false);
        break;
    case APPLY_SKILL:
        if (m_pSkillLevels) {
            BYTE bSkillVnum=static_cast<BYTE>(static_cast<unsigned long>(lVal)>>24);
            int iAdd=(static_cast<unsigned long>(lVal)&0x00800000u)!=0;
            int iChange=static_cast<unsigned long>(lVal)&0x007fffffu;
            sys_log(1,"APPLY_SKILL skill %d add? %d change %d",static_cast<unsigned int>(bSkillVnum),iAdd,iChange);
            int& bonus=m_SkillDamageBonus[bSkillVnum];
            if (iAdd) bonus=apply_item_detail::add(bonus,iChange);
            else bonus=apply_item_detail::sub(bonus,iChange);
            if (m_SkillDamageBonus[bSkillVnum]==0) m_SkillDamageBonus.erase(bSkillVnum);
        }
        break;
'''
s+=''.join('    case '+r['apply_name']+': // '+str(r['apply'])+'\n' for r in ordinary)
s+='''        PointChange(aApplyInfo[bApplyType].bPointType,lVal,false,false);
        break;
    default:
        _sys_err("ApplyPoint",0xe2c,"Unknown apply type %d name %s",static_cast<unsigned int>(bApplyType),GetName());
        break;
    }
}
''';p.joinpath('char.cpp.ApplyPoint.candidate').write_text(s)
p.joinpath('item.cpp.ModifyPoints.candidate').write_text('''// Original item.cpp, CItem::ModifyPoints 08167420. Full body; requires REAL ApplyPoint.
void CItem::ModifyPoints(bool bAdd)
{
    if (!IsAccessoryForSocket() && static_cast<BYTE>(m_pProto->bType-1)<2) {
        for (int i=0;i<3;++i) {
            DWORD dwVnum=m_alSockets[i];
            if (dwVnum>2) {
                TItemTable* p=ITEM_MANAGER::instance().GetTable(dwVnum);
                if (!p) _sys_err("ModifyPoints",0x247,"cannot find table by vnum %u",dwVnum);
                else if (p->bType==10) {
                    for (int j=0;j<3;++j) {
                        if (p->aApplies[j].bType) {
                            int value=p->aApplies[j].lValue;
                            value=p->aApplies[j].bType==APPLY_SKILL?apply_item_detail::skill(value,bAdd):apply_item_detail::removal(value,bAdd);
                            m_pOwner->ApplyPoint(p->aApplies[j].bType,value);
                        }
                    }
                }
            }
        }
    }
    for (int i=0;i<3;++i) {
        TItemTable* p=m_pProto;
        if (p->aApplies[i].bType) {
            sys_log(2,"%s %d %d",p?p->szLocaleName:static_cast<const char*>(0),static_cast<unsigned int>(m_pProto->aApplies[i].bType),p->aApplies[i].lValue);
            p=m_pProto;
            int value;
            if (p->aApplies[i].bType==APPLY_SKILL) value=apply_item_detail::skill(p->aApplies[i].lValue,bAdd);
            else {
                if (IsAccessoryForSocket()) {
                    value=m_pProto->aApplies[i].lValue;
                    int grade=MINMAX(0,m_alSockets[0],3);
                    if (grade) value=apply_item_detail::add(value,MAX(grade,apply_item_detail::mul(value,aiAccessorySocketEffectivePct[grade])/100));
                } else value=m_pProto->aApplies[i].lValue;
                value=apply_item_detail::removal(value,bAdd);
            }
            m_pOwner->ApplyPoint(m_pProto->aApplies[i].bType,value);
        }
    }
    for (int i=0;i<7;++i) {
        if (m_aAttr[i].bType) {
            int value=m_aAttr[i].sValue;
            value=m_aAttr[i].bType==APPLY_SKILL?apply_item_detail::skill(value,bAdd):apply_item_detail::removal(value,bAdd);
            m_pOwner->ApplyPoint(m_aAttr[i].bType,value);
        }
    }
    TItemTable* p=m_pProto;
    BYTE type=p->bType;
    if (type==2) {
        BYTE subtype=p?p->bSubType:0;
        if (subtype!=0 && (p?p->bSubType:0)!=1 && (p?p->bSubType:0)!=4 && (p?p->bSubType:0)!=2) return;
        if (bAdd) {
            if (p->bSubType!=0) return;
            m_pOwner->SetPart(0,static_cast<WORD>(p->dwVnum));
        } else {
            if (p->bSubType!=0) return;
            m_pOwner->SetPart(0,m_pOwner->GetOriginalPart(0));
        }
    } else if (type==1 || type==13 || type==24) {
        if (m_bCell!=94) return;
        WORD value=bAdd?(p?static_cast<WORD>(p->dwVnum):0):m_pOwner->GetOriginalPart(1);
        m_pOwner->SetPart(1,value);
    }
}
''')
print('Full ApplyPoint and ModifyPoints candidates staged; not built/applied')
