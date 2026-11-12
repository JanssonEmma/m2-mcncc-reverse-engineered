from pathlib import Path
import json
p=Path('work/stat_apply_scc');literal=json.loads((p/'original-api.json').read_text())['strings'][0]['escaped']
s='''// Original item.cpp event-info record, original size4; not a polymorphic event_info.
struct item_vid_event_info { DWORD item_vid; };

long accessory_socket_expire_event(LPEVENT event)
{
    item_vid_event_info* info=static_cast<item_vid_event_info*>(event->info);
    LPITEM item=ITEM_MANAGER::instance().FindByVID(info->item_vid);
    int current=item->GetAccessorySocketDownGradeTime();
    DWORD time;
    if (current<2 || apply_item_detail::bits(time=static_cast<DWORD>(current)-60u)<2) {
        item->SetAccessorySocketExpireEvent(0);
        item->AccessorySocketDegrade();
        return 0;
    }
    item->SetAccessorySocketDownGradeTime(time);
    return apply_item_detail::bits(time)<61?apply_item_detail::mul(time,passes_per_sec):apply_item_detail::mul(passes_per_sec,60);
}

void CItem::StartAccessorySocketExpireEvent()
{
    if (IsAccessoryForSocket() && !m_pkAccessorySocketExpireEvent && m_alSockets[1]!=0 && m_alSockets[0]!=0) {
        int seconds=m_alSockets[2];
        SetAccessorySocketExpireEvent(0);
        if (seconds<2) seconds=5;
        else seconds=MIN(seconds,60);
        item_vid_event_info* info;
        DWORD id=__AllocEventInfo(4,reinterpret_cast<void**>(&info));
        info->item_vid=m_dwVID;
        LPEVENT event=event_create_ex(accessory_socket_expire_event,id,apply_item_detail::mul(seconds,passes_per_sec),const_cast<char*>("item.cpp"),0x4e8);
        SetAccessorySocketExpireEvent(event);
    }
}

void CItem::SetAccessorySocketExpireEvent(LPEVENT event)
{
    m_pkAccessorySocketExpireEvent=event;
}

void CItem::AccessorySocketDegrade()
{
    if (m_alSockets[0]>0) {
        LPCHARACTER ch=m_pOwner;
        if (ch) {
            TItemTable* proto=m_pProto;
            const char* format=locale_find("LITERAL",g_iUseLocale);
            ch->ChatPacket(1,format,proto?proto->szLocaleName:static_cast<const char*>(0));
        }
        ModifyPoints(false);
        SetAccessorySocketGrade(apply_item_detail::sub(m_alSockets[0],1));
        ModifyPoints(true);
        DWORD time=aiAccessorySocketDegradeTime[m_alSockets[0]];
        if (test_server) time=apply_item_detail::bits(time)/60;
        SetAccessorySocketDownGradeTime(time);
        if (time) StartAccessorySocketExpireEvent();
    }
}
'''.replace('LITERAL',literal)
p.joinpath('item.cpp.accessory.candidate').write_text(s)
p.joinpath('original-api.inc.candidate').write_text('''// Add to existing complete CHARACTER public API, original char.cpp3489:
void ApplyPoint(BYTE bApplyType,long lVal);
// Add to existing complete CItem public API, original item.cpp562 and1283:
void ModifyPoints(bool bAdd);
void AccessorySocketDegrade();
void StartAccessorySocketExpireEvent();
void SetAccessorySocketExpireEvent(LPEVENT event);
// StopAccessorySocketExpireEvent full candidate already saved under set_count/.
''')
print('Complete accessory event/degrade bodies staged')
