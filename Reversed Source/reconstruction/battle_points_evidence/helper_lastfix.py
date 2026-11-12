from pathlib import Path
p=Path('work/battle_points/helper_regression.cpp');s=p.read_text().replace('assert(ch.GetPolymorphPoint(95)==333&&quests.m_mapEventFlag.size()==count&&quests.m_mapEventFlag["poly"]==-50);','assert(ch.GetPolymorphPoint(95)==333&&quests.m_mapEventFlag.size()==count&&quests.m_mapEventFlag["poly"]==-50&&manager.m_map_pkMobByVnum.size()==1);');p.write_text(s)
