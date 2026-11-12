# Resolve portable paths from the repository root, regardless of current working directory.
import sys as _portable_sys
from pathlib import Path as _portable_Path
for _portable_parent in (_portable_Path(__file__).resolve().parent, *_portable_Path(__file__).resolve().parents):
    if (_portable_parent / 'project_paths.py').is_file():
        _portable_sys.path.insert(0, str(_portable_parent))
        break
else:
    raise RuntimeError('project_paths.py not found in repository ancestors')
from project_paths import resolve_path as _resolve_portable_path
from pathlib import Path
p=Path(__file__).resolve().parent
s=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\item_addon_regression.cpp')).read_text()
s=s[:s.index('static void GaussianPair(')]
s=s.replace('#include "../../game/src/utils.h"','#include "../../game/src/utils.h"\n#include "../../game/src/constants.h"')
s=s.replace('static bool allowNull;','''static bool allowNull;
static int reverseCalls,cannotCalls;
static unsigned mutateAt=UINT_MAX;
static int mutateKind,mutateSet;
static bool mutateLocale;
''')
s=s.replace('extern "C" long item_addon_random() { assert(rngAt<rng.size());return rng[rngAt++]; }','''extern "C" long weighted_test_random() {
    assert(rngAt<rng.size());unsigned at=rngAt++;long raw=rng[at];
    if(at==mutateAt) {
        if(mutateKind==1)g_map_itemAttr[7].bMaxLevelBySet[mutateSet]=0;
        if(mutateKind==2){g_map_itemAttr[7].dwProb=0;g_map_itemAttr[8].dwProb=5;}
    }
    return raw;
}''')
s=s.replace('rng.clear();rngAt=0;','rng.clear();rngAt=0;mutateAt=UINT_MAX;mutateKind=0;')
# Remove unused old diagnostic observers. All new diagnostics are external below.
start=s.index('extern "C" void item_attr_clamp(');end=s.index('struct Runtime : SqlRuntime',start);s=s[:start]+s[end:]
s=s.replace('--depth;++logs;return 0;','--depth;++logs;if(mutateLocale&&logs==1)g_iUseLocale=g_iUseLocale?0:1;return 0;')
s=s.replace('packets=logs=clamps=overflows=0;ops.clear();','packets=logs=clamps=overflows=reverseCalls=cannotCalls=0;ops.clear();')
s+=r'''
extern "C" void weighted_test_error(const char* function,int line,const char* format,int a,int b,const char* file,int sourceLine) {
    assert(activeItem);
    if(!std::strcmp(function,"number_ex")) {
        assert(line==339&&a==1&&b==0&&!std::strcmp(file,"item_attribute.cpp")&&sourceLine==139);
        assert(!std::strcmp(format,"number(): first argument is bigger than second argument %d -> %d, %s %d"));
        ++reverseCalls;
    } else if(!std::strcmp(function,"AddAttr")) {
        assert(line==101&&!std::strcmp(format,"item attribute overflow!"));++overflows;
    } else {
        assert(!std::strcmp(function,"PutAttributeWithLevel")&&line==157);
        assert(!std::strcmp(format,"Cannot put item attribute %d %d")&&a>=0&&a<8&&b>=0&&b<=5);++cannotCalls;
    }
}
static TItemAttrTable rareSentinel;
static void Maps() {
    g_map_itemAttr.clear();g_map_itemRare.clear();
    std::memset(&rareSentinel,0,sizeof(rareSentinel));std::strcpy(rareSentinel.szApply,"rare-only");
    rareSentinel.dwApplyIndex=999;rareSentinel.dwProb=0xffffffffUL;
    for(int i=0;i<5;++i)rareSentinel.lValues[i]=(-2147483647L-1)+i;
    for(int i=0;i<8;++i)rareSentinel.bMaxLevelBySet[i]=static_cast<BYTE>(i+1);
    g_map_itemRare[999]=rareSentinel;
}
static void RareUnchanged() {
    assert(g_map_itemRare.size()==1&&g_map_itemRare.find(999)!=g_map_itemRare.end());
    assert(!std::memcmp(&g_map_itemRare.find(999)->second,&rareSentinel,sizeof(rareSentinel)));
}
static void Row(int key,DWORD weight,int set,int maximum) {
    TItemAttrTable r;std::memset(&r,0,sizeof(r));std::snprintf(r.szApply,sizeof(r.szApply),"row-%d",key);
    r.dwApplyIndex=777; // Selection must use KEY, not record's independently stored index.
    r.dwProb=weight;for(int i=0;i<5;++i)r.lValues[i]=key*100+i+1;
    r.bMaxLevelBySet[set]=static_cast<BYTE>(maximum);g_map_itemAttr[key]=r;
}
static void Attach(CItem& item,TItemTable& proto,CHARACTER& owner,int type=1,int subtype=0) {
    item.Initialize();std::memset(&proto,0,sizeof(proto));proto.bType=static_cast<BYTE>(type);proto.bSubType=static_cast<BYTE>(subtype);item.SetProto(&proto);
    item.SetID(1234);item.SetCell(&owner,107);item.m_dwCount=300;
}
static void Begin(CItem& item,bool skip=false) { Start(item,true,skip,false);mutateLocale=false; }
static void Done(int overflow=0,int reverse=0,int cannot=0) {
    int n=static_cast<int>(ops.size());Finish(n,n,0,overflow,n!=0);
    assert(reverseCalls==reverse&&cannotCalls==cannot);RngDone();RareUnchanged();
}
static void QueryGuards(CHARACTER& owner) {
    const int types[]={0,1,2,99};const int subtypes[]={0,6,7,0};
    for(int i=0;i<4;++i) {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner,types[i],subtypes[i]);
        Begin(item);item.PutAttributeWithLevel(1);Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    Maps();CItem noProto;noProto.Initialize();Begin(noProto);noProto.PutAttributeWithLevel(1);Done();assert(g_map_itemAttr.empty());
    Maps();CItem item;TItemTable proto;Attach(item,proto,owner);Begin(item);item.PutAttributeWithLevel(6);Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
}
static void AddAttrCases(CHARACTER& owner) {
    const int levels[]={0,1,5,6,255};
    for(unsigned k=0;k<5;++k) {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner,99);Row(7,0,0,0);
        g_map_itemAttr[7].lValues[0]=0;g_map_itemAttr[7].lValues[4]=65536;
        Begin(item);if(levels[k]>1)Expect(0,7,0);item.AddAttr(7,static_cast<BYTE>(levels[k]));Done();
        assert(g_map_itemAttr.size()==1);item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);item.AddAttr(255,1);Done();assert(g_map_itemAttr.size()==1);
        TItemAttrTable zero;std::memset(&zero,0,sizeof(zero));assert(!std::memcmp(&g_map_itemAttr[255],&zero,sizeof(zero)));item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);proto.aApplies[2].bType=7;
        Begin(item);item.AddAttr(7,255);Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);
        Begin(item);item.AddAttr(7,1);Done(1);assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);Row(7,0,0,0);g_map_itemAttr[7].lValues[4]=2147483647;
        item.m_aAttr[5].bType=7;item.m_aAttr[5].sValue=55;
        Begin(item,true);Expect(0,7,-1);item.AddAttr(7,255);Done();assert(item.m_aAttr[5].sValue==55);item.SetCell(0,0);
    }
}
static void WeightedCases(CHARACTER& owner) {
    const int types[8]={1,2,2,2,2,2,2,2},subtypes[8]={0,0,3,4,5,1,2,6};
    for(int set=0;set<8;++set)for(int q=1;q<=10;++q) {
        Maps();Row(7,2,set,2);Row(8,3,set,4);Row(9,5,set,5);Row(90,0xffffffffUL,set,5);
        CItem item;TItemTable proto;Attach(item,proto,owner,types[set],subtypes[set]);
        Begin(item);int key=q<=2?7:q<=5?8:9;int cap=key==7?2:key==8?4:5;
        Expect(0,key,static_cast<short>(key*100+cap));Rand(q-1);item.PutAttributeWithLevel(5);Done();
        assert(g_map_itemAttr.size()==83&&g_map_itemAttr[90].dwProb==0xffffffffUL);item.SetCell(0,0);
    }
    // Zero level still scans/inserts maps and draws before AddAttr refuses level0.
    {
        Maps();Row(7,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);Rand(0);item.PutAttributeWithLevel(0);Done();assert(g_map_itemAttr.size()==82);item.SetCell(0,0);
    }
    // Empty distribution still number_ex(1,0): original reversal diagnostic + draw.
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);Rand(0);item.PutAttributeWithLevel(1);Done(0,1,1);assert(g_map_itemAttr.size()==82);
        TItemAttrTable zero;std::memset(&zero,0,sizeof(zero));for(int i=0;i<82;++i)assert(!std::memcmp(&g_map_itemAttr[i],&zero,sizeof(zero)));item.SetCell(0,0);
    }
    // An eligible zero-weight row can be chosen by original swapped-range result0.
    for(int raw=0;raw<2;++raw) {
        Maps();Row(7,0,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);if(!raw)Expect(0,7,701);Rand(raw);item.PutAttributeWithLevel(1);Done(0,1,raw);item.SetCell(0,0);
    }
    // Unsigned weights sum with signed32 wrap; no saturation/rejection.
    {
        Maps();Row(7,0xffffffffUL,0,5);Row(8,2,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);Expect(0,7,701);Rand(0);item.PutAttributeWithLevel(1);Done();item.SetCell(0,0);
    }
    // APPLY_NONE can be eligible when every proto/normal type is nonzero,
    // but selected key0 remains the original failure sentinel.
    {
        Maps();Row(0,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        for(int i=0;i<3;++i)proto.aApplies[i].bType=static_cast<BYTE>(90+i);
        for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);
        Begin(item);Rand(0);item.PutAttributeWithLevel(1);Done(0,0,1);item.SetCell(0,0);
    }
    // External RNG callback mutates current record data: final cap is reread.
    {
        Maps();Row(7,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);mutateAt=0;mutateKind=1;mutateSet=0;Rand(0);item.PutAttributeWithLevel(5);Done();item.SetCell(0,0);
    }
    // Availability/total already captured; selection weights remain live.
    {
        Maps();Row(7,2,0,5);Row(8,3,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        Begin(item);Expect(0,8,801);mutateAt=0;mutateKind=2;Rand(0);item.PutAttributeWithLevel(1);Done();item.SetCell(0,0);
    }
}
static int Level(const int* weights,int percent) {
    int cumulative=0;for(int i=0;i<5;++i){cumulative+=weights[i];if(percent<=cumulative)return i+1;}return 6;
}
static void LevelCases(CHARACTER& owner) {
    const int expectedHigh[5]={30,40,20,8,2},expectedLow[5]={50,40,10,0,0};
    assert(!std::memcmp(aiItemMagicAttributePercentHigh,expectedHigh,sizeof(expectedHigh))&&!std::memcmp(aiItemMagicAttributePercentLow,expectedLow,sizeof(expectedLow)));
    for(int low=0;low<2;++low)for(int percent=1;percent<=100;++percent) {
        Maps();Row(7,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        int weights[5];std::memcpy(weights,low?expectedLow:expectedHigh,sizeof(weights));
        Begin(item);Expect(0,7,static_cast<short>(700+Level(weights,percent)));Rand(percent-1);Rand(0);item.PutAttribute(weights);Done();
        assert(!std::memcmp(weights,low?expectedLow:expectedHigh,sizeof(weights)));item.SetCell(0,0);
    }
    {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner);int weights[5]={0,0,0,0,0};
        Begin(item);Rand(0);item.PutAttribute(weights);Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    {
        Maps();Row(7,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);int weights[5]={INT_MIN,0,0,0,0};
        Begin(item);Expect(0,7,702);Rand(0);Rand(0);item.PutAttribute(weights);Done();item.SetCell(0,0);
    }
}
static void PlanPut(int percentile) { Rand(percentile-1);Rand(0); }
static void MagicCases(CHARACTER& owner) {
    const int locales[]={0,1,-1,123},types[]={1,2,2},subtypes[]={0,0,1},sets[]={0,1,5};
    for(unsigned l=0;l<4;++l)for(int kind=0;kind<3;++kind)for(int passes=0;passes<4;++passes)for(int change=0;change<2;++change) {
        Maps();Row(7,1,sets[kind],5);Row(8,1,sets[kind],5);Row(9,1,sets[kind],5);
        CItem item;TItemTable proto;Attach(item,proto,owner,types[kind],subtypes[kind]);
        int second,third;
        if(locales[l]==0){second=kind==0?30:kind==1?20:10;third=kind==0?15:kind==1?10:5;}
        else {second=kind==0?20:10;third=kind==0?5:kind==1?2:1;}
        bool doSecond=(passes&1)!=0,doThird=(passes&2)!=0;
        Begin(item);g_iUseLocale=locales[l];mutateLocale=change!=0;Expect(0,7,705);PlanPut(100);
        Rand((doSecond?second:second+1)-1);if(doSecond){Expect(1,8,803);PlanPut(100);}
        Rand((doThird?third:third+1)-1);if(doThird){int key=doSecond?9:8;Expect(doSecond?2:1,key,static_cast<short>(key*100+1));PlanPut(1);}
        item.AlterToMagicItem();Done();item.SetCell(0,0);
    }
    // Existing attributes are retained; full capacity does not bypass any draws.
    {
        Maps();Row(7,1,0,5);Row(8,1,0,5);Row(9,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        for(int i=0;i<5;++i)item.m_aAttr[i].bType=static_cast<BYTE>(i+1);
        TPlayerItemAttribute before[7];std::memcpy(before,item.m_aAttr,sizeof(before));
        Begin(item);g_iUseLocale=0;PlanPut(100);Rand(0);PlanPut(100);Rand(0);PlanPut(1);
        item.AlterToMagicItem();Done(3);assert(!std::memcmp(before,item.m_aAttr,sizeof(before)));item.SetCell(0,0);
    }
    {
        Maps();Row(7,1,0,5);Row(8,1,0,5);Row(9,1,0,5);CItem item;TItemTable proto;Attach(item,proto,owner);
        item.m_aAttr[0].bType=7;item.m_aAttr[0].sValue=42;
        Begin(item);g_iUseLocale=0;Expect(1,8,805);PlanPut(100);Rand(99);Rand(99);
        item.AlterToMagicItem();Done();assert(item.m_aAttr[0].sValue==42);item.SetCell(0,0);
    }
    for(int t=0;t<3;++t) {
        Maps();CItem item;TItemTable proto;Attach(item,proto,owner,t==0?1:t==1?2:99,t==0?6:t==1?7:0);
        Begin(item);item.AlterToMagicItem();Done();assert(g_map_itemAttr.empty());item.SetCell(0,0);
    }
    g_iUseLocale=0;
}
int main() {
    assert(sizeof(DWORD)==4&&sizeof(long)==4&&sizeof(TItemAttrTable)==69);
    assert(g_map_itemAttr.empty()&&g_map_itemRare.empty());
    desc_test_allow_clock();test_server=0;
    Runtime runtime;sql_test_runtime=&runtime;
    LogManager log;logger=&log;runtime.db=log.m_sql.GetSQLHandle();assert(log.Connect("host","user","password","database"));g_table_postfix="_attr";
    quest::CQuestManager quests;ITEM_MANAGER items;manager=&items;CHARACTER owner;owner.m_dwPlayerID=777;owner.m_stName="weighted-owner";owner.m_vid=VID(7000,55);
    SECTREE sector;owner.SetSectree(&sector);
    output=buffer_new(0,8192);LPFDWATCH watch=fdwatch_new(16);DescOutputProbe descriptor;descriptor.Attach(watch,5,output);descriptor.SetPacketState(5);owner.BindDesc(&descriptor);
    thecore_memcpy=Copy;
    QueryGuards(owner);AddAttrCases(owner);WeightedCases(owner);LevelCases(owner);MagicCases(owner);
    g_map_itemAttr.clear();g_map_itemRare.clear();
    thecore_memcpy=std::memcpy;owner.BindDesc(0);owner.SetSectree(0);descriptor.Attach(0,-1,0);buffer_delete(output);fdwatch_delete(watch);event_destroy();
    std::puts("weighted attributes passed: complete maps/record, all selection/locale branches, raw32 quirks and actual packet/save/SQL");
}
'''
(p/'weighted_attribute_regression.cpp').write_text(s)
print('Full real application regression staged; not compiled/run.')
