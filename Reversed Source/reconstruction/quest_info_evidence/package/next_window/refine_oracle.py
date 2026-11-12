from pathlib import Path
p=Path('work/pc_quest_info/next_window/pc_quest_info_regression.cpp');s=p.read_text(encoding='utf-8-sig')
s=s.replace('else if(mode==1 && copied==2)pc->m_RunningQuestState=a;}','else if(mode==1 && copied==2)pc->m_RunningQuestState=a;else if(mode==3 && copied==2){b->bStart=false;pc->m_RunningQuestState=b;}}')
s=s.replace('assert(liveHeader && Read2(liveHeader+1)==frame.size()); // Local qi.size updated; WIRE header still6.','assert(liveHeader && Read2(liveHeader+1)==frame.size());liveHeader=0; // Observe ONLY while sender is active; no dangling header pointer retained.')
s=s.replace('if(mode==1 && kind==2){','if(mode==3 && kind==1){assert(value==1 && pc->m_RunningQuestState==b && !b->bStart);}\n    else if(mode==1 && kind==2){')
needle='    // Getter is INDEX, not bStart. Current scalar/string setters OR mask then mutate, never send.\n';assert s.count(needle)==1
s=s.replace(needle,'    // Begin logger uses cached BYTE written BEFORE callback, not the now-current state.\n    pc->m_RunningQuestState=a;a->bStart=true;pc->m_iSendToClient=1;qm->m_pCurrentCharacter=charA;Begin(outputA);Expected(1,*a);mode=3;active=true;pc->SendQuestInfoPakcet();End(0);assert(pc->m_RunningQuestState==b && !b->bStart);\n'+needle)
s=s.replace('assert(pc->m_iSendToClient==(INT_MIN|127) && a->_title.size()==80 && a->_clock_value==INT_MAX && a->_counter_value==INT_MIN && !a->bStart);','assert(pc->m_iSendToClient==(INT_MIN|127) && a->_title==std::string(80,\'T\') && a->_clock_name==std::string(80,\'C\') && a->_counter_name==std::string(80,\'N\') && a->_icon_file==std::string(80,\'I\') && a->_clock_value==INT_MAX && a->_counter_value==INT_MIN && !a->bStart);')
p.write_text(s,encoding='utf-8')
