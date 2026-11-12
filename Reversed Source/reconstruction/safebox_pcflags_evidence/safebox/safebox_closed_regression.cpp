// Full actual safebox constructor/Add/Save/lookups; explicit empty raw-storage teardown boundary.
#include <map>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <string>
#include <ext/hash_map>
#include <new>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <climits>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/safebox.h"
#include "../../game/src/item.h"
#include "../../game/src/item_manager.h"
#include "../../game/src/questmanager.h"
#include "../../libgame/include/grid.h"
#undef protected
#undef private
#include "../../game/src/desc_client.h"
#include "../../game/src/packet.h"
#include "../../libthecore/include/memcpy.h"
#include "desc_fixture.h"
int max_bytes_written,current_bytes_written,total_bytes_written;
void ShutdownOnFatalError(){std::abort();}
extern "C" void desc_test_allow_clock(void);
static void* watchedDelete;static bool wasDeleted;
void* operator new(std::size_t n)throw(std::bad_alloc){void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();std::memset(p,0xa5,n);return p;}
void operator delete(void* p)throw(){if(p==watchedDelete)wasDeleted=true;std::free(p);}
void* operator new[](std::size_t n)throw(std::bad_alloc){return ::operator new(n);}
void operator delete[](void* p)throw(){::operator delete(p);}
// No CSafebox destructor body is defined in tests. Do NOT pretend populated lifecycle is closed.
struct RawBox {
    void* storage;CSafebox* box;std::vector<CGrid*> leaked;
    RawBox(CHARACTER* owner,int size,DWORD gold){storage=::operator new(sizeof(CSafebox));box=new(storage)CSafebox(owner,size,gold);}
    ~RawBox(){for(int i=0;i<135;++i)assert(!box->m_pkItems[i]);delete box->m_pkGrid;for(unsigned i=0;i<leaked.size();++i)delete leaked[i];::operator delete(storage);}
};
class ClientProbe:public CLIENT_DESC{public:void Output(LPBUFFER b){m_lpOutputBuffer=b;}};
static std::vector<std::string> events;
static bool active,special;
static CSafebox* currentBox;static CItem* currentItem;static CHARACTER* firstOwner;static CHARACTER* secondOwner;static TItemTable* currentProto;
static LPBUFFER gc1,gc2,dbout;
static unsigned errors;static int errorLine;static DWORD errorArgument;
extern "C" void safebox_observe_error(int line,DWORD arg){++errors;errorLine=line;errorArgument=arg;}
extern "C" void safebox_observe_log(int kind){
    if(!active)return;
    if(kind==1){events.push_back("item-save");if(special){currentBox->m_bWindowMode=4;currentBox->m_pkChrOwner=secondOwner;currentItem->m_dwCount=0x1234;currentProto->dwVnum=0x87654321UL;}}
    else if(kind==2)events.push_back("add-log");
    else if(kind==3)events.push_back("box-save-log");
}
static void* Copy(void* out,const void* in,size_t size){
    if(active){
        if(out==dbout->write_point)events.push_back(size==1?"db-header":size==4?"db-word":size==52?"db-item":"db-box");
        else if(out==gc1->write_point || out==gc2->write_point)events.push_back("gc");
        else if(currentItem && in==currentItem->m_alSockets && size==12){events.push_back("sockets");if(special){currentItem->m_dwCount=999;currentProto->dwVnum=777;currentItem->m_aAttr[0].sValue=-321;currentBox->m_bWindowMode=255;currentBox->m_pkChrOwner=firstOwner;}}
        else if(currentItem && in==currentItem->m_aAttr && size==21)events.push_back("attrs");
    }
    return std::memcpy(out,in,size);
}
static DWORD Read4(const BYTE* b){return static_cast<DWORD>(b[0])|static_cast<DWORD>(b[1])<<8|static_cast<DWORD>(b[2])<<16|static_cast<DWORD>(b[3])<<24;}
static const BYTE* Frame(BYTE header,DWORD size){assert(buffer_size(dbout)==size+9);const BYTE* p=static_cast<const BYTE*>(buffer_read_peek(dbout));assert(p[0]==header && !Read4(p+1) && Read4(p+5)==size);return p+9;}
static void Begin(){active=false;events.clear();buffer_reset(gc1);buffer_reset(gc2);buffer_reset(dbout);special=false;currentItem=0;}
static void Sequence(bool saved){std::vector<std::string> e;if(saved){e.push_back("item-save");e.push_back("db-header");e.push_back("db-word");e.push_back("db-word");e.push_back("db-item");}e.push_back("sockets");e.push_back("attrs");e.push_back("gc");e.push_back("add-log");assert(events==e);}
static void Detach(RawBox& raw,CItem& item){for(int i=0;i<135;++i)if(raw.box->m_pkItems[i]==&item)raw.box->m_pkItems[i]=0;item.SetCell(0,0);item.SetWindow(0);}
int main(){
    desc_test_allow_clock();quest::CQuestManager quests;ITEM_MANAGER manager;CHARACTER owner,other;
    owner.m_stName="safe-owner";other.m_stName="other-owner";owner.m_dwPlayerID=0x11223344UL;other.m_dwPlayerID=99;
    DescOutputProbe d1,d2;ClientProbe database;LPFDWATCH watch=fdwatch_new(16);
    gc1=buffer_new(0,8192);gc2=buffer_new(0,8192);dbout=buffer_new(0,8192);
    d1.Attach(watch,5,gc1);d1.SetPacketState(5);d2.Attach(watch,6,gc2);d2.SetPacketState(5);
    d1.GetAccountTable().id=0xaabbccddUL;d2.GetAccountTable().id=0x12345678UL;
    owner.BindDesc(&d1);other.BindDesc(&d2);database.Output(dbout);db_clientdesc=&database;thecore_memcpy=Copy;
    {
        RawBox zero(&owner,0,0xffffffffUL);assert(!zero.box->m_pkGrid && zero.box->m_iSize==0 && zero.box->m_lGold==-1 && zero.box->m_bWindowMode==3);
        for(int i=0;i<135;++i)assert(!zero.box->m_pkItems[i]);
        assert(!zero.box->IsEmpty(0,0) && !zero.box->IsValidPosition(0));
        unsigned before=errors;assert(!zero.box->GetItem(0));assert(errors==before+1 && errorLine==159);
        for(unsigned mode=0;mode<256;++mode){zero.box->SetWindowMode(static_cast<BYTE>(mode));assert(zero.box->m_bWindowMode==mode);}
        zero.box->ChangeSize(1);assert(zero.box->m_iSize==1 && zero.box->m_pkGrid->GetSize()==5);
        CGrid* old=zero.box->m_pkGrid;assert(old->Put(0,1,1));watchedDelete=old;wasDeleted=false;
        zero.box->ChangeSize(2);assert(!wasDeleted && zero.box->m_pkGrid!=old && zero.box->m_iSize==2);watchedDelete=0;
        zero.leaked.push_back(old);assert(old->GetSize()==5 && zero.box->m_pkGrid->m_pGrid[0]==1);
        for(int i=5;i<10;++i)assert(static_cast<unsigned char>(zero.box->m_pkGrid->m_pGrid[i])==0xa5); // Allocator initialized bytes, not an indeterminate read.
        CGrid* grown=zero.box->m_pkGrid;zero.box->ChangeSize(2);zero.box->ChangeSize(-1);assert(zero.box->m_pkGrid==grown && zero.box->m_iSize==2);
        grown->Clear();assert(zero.box->IsEmpty(9,1) && !zero.box->IsEmpty(9,2));
    }
    {
        RawBox raw(&owner,27,0x80000000UL);currentBox=raw.box;
        assert(raw.box->m_pkGrid->GetSize()==135);
        CItem item;item.Initialize();TItemTable proto;std::memset(&proto,0,sizeof proto);proto.dwVnum=123;proto.bSize=1;std::strcpy(proto.szLocaleName,"safe-item");item.SetProto(&proto);item.SetID(7);
        item.m_dwCount=0x12345678;item.m_alSockets[0]=-1;item.m_alSockets[1]=0x12345678;item.m_alSockets[2]=INT_MIN;
        for(int i=0;i<7;++i){item.m_aAttr[i].bType=static_cast<BYTE>(200+i);item.m_aAttr[i].sValue=static_cast<short>(-20-i);}
        assert(!owner.GetSafebox() && !owner.GetMall());owner.m_pkSafebox=raw.box;owner.m_pkMall=raw.box;assert(owner.GetSafebox()==raw.box && owner.GetMall()==raw.box);
        for(unsigned cell=0;cell<135;++cell){raw.box->m_pkItems[cell]=&item;assert(raw.box->Get(cell)==&item && raw.box->GetItem(static_cast<BYTE>(cell))==&item);raw.box->m_pkItems[cell]=0;}
        const DWORD bad[]={135,255,0x80000000UL,0xffffffffUL};for(unsigned i=0;i<4;++i){assert(!raw.box->IsValidPosition(bad[i]) && !raw.box->Get(bad[i]));}
        unsigned before=errors;for(unsigned cell=135;cell<256;++cell)assert(!raw.box->GetItem(static_cast<BYTE>(cell)));assert(errors==before+121 && errorLine==159 && errorArgument==255);
        int oldSize=raw.box->m_iSize;raw.box->m_iSize=INT_MIN;assert(!raw.box->GetItem(0));raw.box->m_iSize=oldSize;
        Begin();currentItem=&item;active=true;before=errors;assert(!raw.box->Add(135,&item));active=false;assert(errors==before+1 && errorLine==58 && errorArgument==135 && events.empty() && !buffer_size(dbout));
        const BYTE modes[]={3,4,0,2,255};
        for(unsigned mode=0;mode<5;++mode){
            Begin();raw.box->m_pkGrid->Clear();raw.box->SetWindowMode(modes[mode]);currentItem=&item;active=true;
            assert(raw.box->Add(134,&item));active=false;Sequence(true);assert(raw.box->Get(134)==&item && item.GetOwner()==&owner && item.GetCell()==134 && item.GetWindow()==modes[mode]);
            assert(manager.m_set_pkItemForDelayedSave.empty());TPlayerItem saved;std::memcpy(&saved,Frame(30,52),52);
            assert(saved.id==7 && saved.window==modes[mode] && saved.pos==(modes[mode]==2?44:134) && saved.count==0x12345678 && saved.vnum==123);
            assert(saved.owner==((modes[mode]==3 || modes[mode]==4)?0xaabbccddUL:owner.GetPlayerID()));
            const BYTE* shown=static_cast<const BYTE*>(buffer_read_peek(gc1));assert(buffer_size(gc1)==40 && !buffer_size(gc2) && shown[0]==(modes[mode]==3?85:128) && shown[1]==134 && Read4(shown+2)==123 && shown[6]==0x78);
            assert(!std::memcmp(shown+7,item.GetSockets(),12) && !std::memcmp(shown+19,item.GetAttributes(),21));Detach(raw,item);
        }
        // Grid placement failure does not roll back ownership/save/slot/packet.
        Begin();raw.box->m_pkGrid->Clear();raw.box->SetWindowMode(3);proto.bSize=2;currentItem=&item;active=true;assert(raw.box->Add(134,&item));active=false;Sequence(true);assert(raw.box->m_pkItems[134]==&item && raw.box->IsEmpty(134,1));Detach(raw,item);
        // Size0 occupies a slot despite marking no grid cells.
        Begin();proto.bSize=0;currentItem=&item;active=true;assert(raw.box->Add(0,&item));active=false;Sequence(true);assert(raw.box->m_pkItems[0]==&item && raw.box->IsEmpty(0,1));Detach(raw,item);
        // Actual delayed-save logging and copy callbacks mutate subsequent reads; old arguments stay captured where proven.
        Begin();raw.box->m_pkGrid->Clear();proto.bSize=1;raw.box->SetWindowMode(3);raw.box->m_pkChrOwner=&owner;
        currentItem=&item;currentProto=&proto;firstOwner=&owner;secondOwner=&other;special=true;active=true;assert(raw.box->Add(5,&item));active=false;Sequence(true);
        TPlayerItem saved;std::memcpy(&saved,Frame(30,52),52);assert(saved.window==3 && saved.owner==0xaabbccddUL && saved.vnum==0x87654321UL && saved.count==0x1234);
        const BYTE* shown=static_cast<const BYTE*>(buffer_read_peek(gc1));assert(buffer_size(gc1)==40 && !buffer_size(gc2) && shown[0]==128 && Read4(shown+2)==0x87654321UL && shown[6]==0x34);
        short attribute;std::memcpy(&attribute,shown+20,2);assert(attribute==-321 && item.GetCount()==999 && item.GetVnum()==777 && raw.box->m_bWindowMode==255);Detach(raw,item);
        // ID0 / skip-save avoid a new delayed entry but retain full Add packet/grid path.
        Begin();raw.box->m_pkGrid->Clear();item.SetID(0);currentItem=&item;active=true;assert(raw.box->Add(0,&item));active=false;Sequence(false);assert(!buffer_size(dbout));Detach(raw,item);
        Begin();raw.box->m_pkGrid->Clear();item.SetID(7);item.SetSkipSave(true);currentItem=&item;active=true;assert(raw.box->Add(0,&item));active=false;Sequence(false);assert(!buffer_size(dbout));Detach(raw,item);item.SetSkipSave(false);
        // Existing-slot overlap is accepted even when grid Put fails; original owner is not detached.
        CItem second;second.Initialize();second.SetProto(&proto);proto.bSize=1;item.SetID(0);
        Begin();raw.box->m_pkGrid->Clear();currentItem=&item;active=true;assert(raw.box->Add(0,&item));active=false;Sequence(false);
        Begin();currentItem=&second;active=true;assert(raw.box->Add(0,&second));active=false;Sequence(false);
        assert(raw.box->Get(0)==&second && item.GetOwner()==&owner && second.GetOwner()==&owner && !raw.box->IsEmpty(0,1));Detach(raw,item);Detach(raw,second);second.SetSkipSave(true);
        // Null prototype remains a valid size0/vnum0 Add path, without inventing an item prototype.
        CItem untyped;untyped.Initialize();Begin();raw.box->m_pkGrid->Clear();currentItem=&untyped;active=true;assert(raw.box->Add(6,&untyped));active=false;Sequence(false);
        const BYTE* noProto=static_cast<const BYTE*>(buffer_read_peek(gc1));assert(buffer_size(gc1)==40 && !Read4(noProto+2) && !noProto[6]);Detach(raw,untyped);untyped.SetSkipSave(true);
        // Original Save leaves bSize/wItemCount ZERO, regardless actual size, and uses account ID/gold32.
        Begin();currentItem=0;raw.box->m_lGold=INT_MIN;active=true;raw.box->Save();active=false;
        const BYTE* table=Frame(13,11);assert(Read4(table)==0xaabbccddUL && table[4]==0 && Read4(table+5)==0x80000000UL && !table[9] && !table[10]);
        assert(events.size()==5 && events[0]=="db-header" && events[1]=="db-word" && events[2]=="db-word" && events[3]=="db-box" && events[4]=="box-save-log");
        owner.m_pkSafebox=owner.m_pkMall=0;item.SetSkipSave(true);manager.m_set_pkItemForDelayedSave.clear();
    }
    active=false;thecore_memcpy=std::memcpy;owner.BindDesc(0);other.BindDesc(0);db_clientdesc=0;database.Output(0);d1.GetAccountTable().id=0;d2.GetAccountTable().id=0;
    d1.Attach(0,-1,0);d2.Attach(0,-1,0);buffer_delete(gc1);buffer_delete(gc2);buffer_delete(dbout);fdwatch_delete(watch);
    std::puts("safebox actual constructor/Add/Save/lookups passed; populated destructor/removal NOT exercised");return 0;
}
