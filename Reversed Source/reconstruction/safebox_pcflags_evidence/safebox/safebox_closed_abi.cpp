#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include "../../game/src/char.h"
#define protected public
#include "../../game/src/safebox.h"
#undef protected
#include "../../common/tables.h"
#include <stddef.h>
typedef char windows_width4[sizeof(EWindows)==4?1:-1];
typedef char windows6[RESERVED_WINDOW==0 && INVENTORY==1 && EQUIPMENT==2 && SAFEBOX==3 && MALL==4 && GROUND==5?1:-1];
typedef char long32[sizeof(long)==4?1:-1];
typedef char pointer32[sizeof(void*)==4?1:-1];
typedef char full_class560[sizeof(CSafebox)==560?1:-1];
typedef char owner0[offsetof(CSafebox,m_pkChrOwner)==0?1:-1];
typedef char items4[offsetof(CSafebox,m_pkItems)==4?1:-1];
typedef char item_count135[sizeof(((CSafebox*)0)->m_pkItems)/sizeof(LPITEM)==135?1:-1];
typedef char grid544[offsetof(CSafebox,m_pkGrid)==544?1:-1];
typedef char size548[offsetof(CSafebox,m_iSize)==548?1:-1];
typedef char gold552[offsetof(CSafebox,m_lGold)==552?1:-1];
typedef char mode556[offsetof(CSafebox,m_bWindowMode)==556?1:-1];
typedef char table11[sizeof(TSafeboxTable)==11?1:-1];
typedef char table_id0[offsetof(TSafeboxTable,dwID)==0?1:-1];
typedef char table_size4[offsetof(TSafeboxTable,bSize)==4?1:-1];
typedef char table_gold5[offsetof(TSafeboxTable,dwGold)==5?1:-1];
typedef char table_count9[offsetof(TSafeboxTable,wItemCount)==9?1:-1];
void Gates(){
    bool(CSafebox::*add)(DWORD,LPITEM)=&CSafebox::Add;
    LPITEM(CSafebox::*get)(DWORD)=&CSafebox::Get;
    LPITEM(CSafebox::*remove)(DWORD)=&CSafebox::Remove;
    bool(CSafebox::*move)(BYTE,BYTE,BYTE)=&CSafebox::MoveItem;
    LPITEM(CSafebox::*getItem)(BYTE)=&CSafebox::GetItem;
    bool(CSafebox::*empty)(DWORD,BYTE)=&CSafebox::IsEmpty;
    bool(CSafebox::*valid)(DWORD)=&CSafebox::IsValidPosition;
    void(CSafebox::*size)(int)=&CSafebox::ChangeSize;
    void(CSafebox::*mode)(BYTE)=&CSafebox::SetWindowMode;
    void(CSafebox::*save)()=&CSafebox::Save;
    CSafebox*(CHARACTER::*safe)()=&CHARACTER::GetSafebox;
    CSafebox*(CHARACTER::*mall)()=&CHARACTER::GetMall;
    (void)add;(void)get;(void)remove;(void)move;(void)getItem;(void)empty;(void)valid;(void)size;(void)mode;(void)save;(void)safe;(void)mall;
}
