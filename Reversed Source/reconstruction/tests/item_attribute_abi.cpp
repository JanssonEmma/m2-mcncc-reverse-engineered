#include "../../game/src/item.h"
typedef char attribute_record_size[sizeof(TPlayerItemAttribute)==3?1:-1];
typedef char socket_scalar_size[sizeof(long)==4?1:-1];
void CheckItemAttributeAPI() {
    void (CItem::*socket)(int)=&CItem::AlterToSocketItem;
    int (CItem::*set)()=&CItem::GetAttributeSetIndex;
    bool (CItem::*has)(BYTE)=&CItem::HasAttr;
    int (CItem::*count)()=&CItem::GetAttributeCount;
    int (CItem::*find)(BYTE)=&CItem::FindAttribute;
    void (CItem::*write)(int,BYTE,short)=&CItem::SetAttribute;
    void (CItem::*add)(BYTE,short)=&CItem::AddAttribute;
    (void)socket;(void)set;(void)has;(void)count;(void)find;(void)write;(void)add;
}
