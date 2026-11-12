#include "../../game/src/item.h"
#include "../../game/src/constants.h"
#include <cstddef>
typedef char tail_full_row[sizeof(TItemAttrTable)==69?1:-1];
typedef char tail_seven_attrs[sizeof(TPlayerItemAttribute)*7==21?1:-1];
void CheckTailAPIs(){
 void(CItem::*change)(const int*)=&CItem::ChangeAttribute;
 void(CItem::*add)()=&CItem::AddAttribute;
 void(CItem::*clear)()=&CItem::ClearAttribute;
 bool(CItem::*remove)(int)=&CItem::RemoveAttributeAt;
 void(CItem::*set)(const TPlayerItemAttribute*)=&CItem::SetAttributes;
 void(CItem::*copy)(CItem*)=&CItem::CopyAttributeTo;
 bool(CItem::*hasRare)(BYTE)=&CItem::HasRareAttr;
 int(CItem::*countRare)()=&CItem::GetRareAttrCount;
 bool(CItem::*changeRare)()=&CItem::ChangeRareAttribute;
 bool(CItem::*addRare)()=&CItem::AddRareAttribute;
 // Existing original recursion API/weighted nonconst ABI remain intact.
 bool(CItem::*recursive)(BYTE)=&CItem::RemoveAttributeType;
 void(CItem::*put)(int*)=&CItem::PutAttribute;
 (void)change;(void)add;(void)clear;(void)remove;(void)set;(void)copy;(void)hasRare;(void)countRare;(void)changeRare;(void)addRare;(void)recursive;(void)put;
}
