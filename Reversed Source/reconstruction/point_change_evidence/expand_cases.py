from pathlib import Path
p=Path('work/point_change/PointChange.cpp.candidate');s=p.read_text();a=s.index('    case 88:case 89:case 90: {');b=s.index('    case 108:',a)
s=s[:a]+'''    case 88:
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
'''+s[b:];p.write_text(s)
