#include <map>
#include <string>
#include <vector>
#define protected public
#define private public
#include "../../game/src/text_file_loader.h"
#undef private
#undef protected
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <fstream>
static int errors,logs;
extern "C" void sys_log(int level,const char* format,...) { assert(level==2 && format); ++logs; }
extern "C" void _sys_err(const char* function,int line,const char* format,...) {
    assert(std::string(function)=="LoadGroup" && (line==86 || line==88 || line==147));
    ++errors; va_list args; va_start(args,format); std::vfprintf(stderr,format,args); va_end(args); std::fputc('\n',stderr);
}
static const char* first_file="_mcncc_text_loader_first.tmp";
static const char* second_file="_mcncc_text_loader_second.tmp";
static const char* bad_file="_mcncc_text_loader_bad.tmp";
static void Cleanup() { std::remove(first_file); std::remove(second_file); std::remove(bad_file); }
static void Write(const char* path,const char* text) { std::ofstream out(path,std::ios::binary); out<<text; out.close(); assert(out); }
static int constructed,live;
struct Probe { int value; Probe():value(++constructed) { ++live; } ~Probe() { --live; } };
static void Pool() {
    CDynamicPool<Probe> pool; assert(!pool.GetCapacity()); pool.SetName("probe");
    Probe* a=pool.Alloc(); Probe* b=pool.Alloc(); Probe* c=pool.Alloc();
    assert(live==3 && pool.GetCapacity()==3 && a->value==1 && b->value==2 && c->value==3);
    b->value=42; pool.Free(b); Probe* reused=pool.Alloc(); assert(reused==b && reused->value==42 && live==3 && pool.GetCapacity()==3);
    pool.FreeAll(); assert(!pool.m_pUsedList && pool.m_pFreeList && live==3);
    reused=pool.Alloc(); assert(reused==a && reused->value==1);
    pool.Clear(); assert(!live && !pool.GetCapacity() && !pool.m_pFreeList && !pool.m_pUsedList && pool.m_stName=="probe");
    pool.Clear();
}
int main(int argc,char**) {
    std::atexit(Cleanup);
    if(argc>1) { Write(bad_file,"Group bad extra\n"); CTextFileLoader loader; loader.Load(bad_file); return 99; }
    Pool();
    Write(first_file,
        "MiXeD FirstValue\n"
        "mixed ignored\n"
        "Integer -17 tail\n"
        "Bool -2\nByte -1\nWord 65537\nDword -1\nHex 0x10\nFloat 2.5junk\n"
        "Quoted \"Keep Mixed Case\"\n"
        "Two 1.25 -2\nThree 1 2 3\nFour 1 2 3 4\nColor -1 255 0.5 2\n"
        "List Values\n{\nOne Two\n\"Three Four\"\n}\n"
        "List Empty\n{\n}\n"
        "Group Group01\n{\nKey ChildValue\nGroup Nested\n{\nInside 7\n}\n}\n"
        "Group group01\n{\nKey SecondChild\n}\n"
        "Group \"Spaced Name\"\n{\nKey ThirdChild\n}\n");
    {
        CTextFileLoader loader;
        assert(!loader.m_dwcurLineIndex && !loader.mc_pData && loader.m_pcurNode==&loader.m_globalNode && loader.m_globalNode.strGroupName=="global" && !loader.m_globalNode.pParentNode);
        assert(loader.Load(first_file) && std::string(loader.GetFileName())==first_file && loader.GetChildNodeCount()==3);
        std::string value="retained"; assert(!loader.GetCurrentNodeName(&value) && value=="retained");
        assert(loader.IsToken("mixed") && !loader.IsToken("MiXeD"));
        assert(loader.GetTokenString("mixed",&value) && value=="FirstValue");
        assert(loader.GetTokenString("quoted",&value) && value=="Keep Mixed Case");
        int integer=88; BOOL boolean=88; BYTE byte=88; WORD word=88; DWORD dword=88; float real=88;
        assert(loader.GetTokenInteger("integer",&integer) && integer==-17);
        assert(loader.GetTokenBoolean("bool",&boolean) && boolean==-2);
        assert(loader.GetTokenByte("byte",&byte) && byte==255 && loader.GetTokenWord("word",&word) && word==1);
        assert(loader.GetTokenDoubleWord("dword",&dword) && dword==0xffffffffUL);
        assert(loader.GetTokenInteger("hex",&integer) && integer==0 && loader.GetTokenFloat("float",&real) && real==2.5f);
        TTokenVector* values=0; assert(loader.GetTokenVector("values",&values) && values->size()==3 && values->at(0)=="One" && values->at(2)=="Three Four");
        values->at(0)="Changed"; assert(loader.GetTokenString("values",&value) && value=="Changed");
        TTokenVector* missing=reinterpret_cast<TTokenVector*>(1); assert(!loader.GetTokenVector("missing",&missing) && missing==reinterpret_cast<TTokenVector*>(1));
        integer=88; assert(loader.IsToken("empty") && !loader.GetTokenInteger("empty",&integer) && integer==88);
        D3DXVECTOR2 v2={8,9}; D3DXVECTOR3 v3={8,9,10}; D3DXVECTOR4 v4={8,9,10,11}; D3DXQUATERNION q={0,0,0,0}; D3DXCOLOR color={0,0,0,0}; D3DCOLORVALUE color2={0,0,0,0};
        assert(loader.GetTokenVector2("two",&v2) && v2.x==1.25f && v2.y==-2);
        assert(!loader.GetTokenVector2("three",&v2) && v2.x==1.25f && v2.y==-2);
        assert(loader.GetTokenVector3("three",&v3) && v3.x==1 && v3.y==2 && v3.z==3);
        assert(loader.GetTokenPosition("three",&v3) && loader.GetTokenDirection("three",&v3) && v3.z==3);
        assert(loader.GetTokenVector4("four",&v4) && v4.w==4 && loader.GetTokenQuaternion("four",&q) && q.w==4);
        assert(!loader.GetTokenVector4("three",&v4) && v4.w==4 && !loader.GetTokenQuaternion("three",&q) && q.w==4);
        assert(loader.GetTokenColor("color",&color) && loader.GetTokenColor("color",&color2));
        assert(color.r==-1 && color.g==255 && color.b==0.5f && color.a==2 && color2.a==2);
        assert(!loader.SetChildNode("Group01") && loader.SetChildNode(std::string("group"),1));
        assert(loader.GetCurrentNodeName(&value) && value=="group01" && loader.GetChildNodeCount()==1);
        assert(loader.GetTokenString("key",&value) && value=="ChildValue" && loader.SetChildNode(static_cast<DWORD>(0)));
        assert(loader.GetTokenInteger("inside",&integer) && integer==7 && loader.SetParentNode() && loader.SetParentNode());
        assert(loader.SetChildNode(static_cast<DWORD>(1)) && loader.GetTokenString("key",&value) && value=="SecondChild");
        loader.SetTop(); assert(loader.SetChildNode("spaced name") && loader.GetTokenString("key",&value) && value=="ThirdChild");
        Write(second_file,"mixed replacement\nnewkey Added\nGroup Extra\n{\nLeaf End\n}\n");
        assert(loader.Load(second_file) && loader.GetCurrentNodeName(&value) && value=="spaced name"); // Load does not reset the navigation cursor.
        loader.SetTop(); assert(loader.GetChildNodeCount()==4 && loader.GetTokenString("mixed",&value) && value=="FirstValue" && loader.IsToken("newkey"));
        assert(!loader.Load("_mcncc_text_loader_does_not_exist.tmp") && std::string(loader.GetFileName())=="_mcncc_text_loader_does_not_exist.tmp" && loader.IsToken("newkey"));
    }
    // A loader's destructor does not release its shared pooled child nodes.
    assert(CTextFileLoader::ms_groupNodePool.GetCapacity()==5);
    CTextFileLoader::DestroySystem(); assert(!CTextFileLoader::ms_groupNodePool.GetCapacity());
    Write(bad_file,"before 1\nbare\nafter 2\n");
    { CTextFileLoader loader; assert(loader.Load(bad_file) && loader.IsToken("before") && !loader.IsToken("after") && errors==1); }
    Write(bad_file,"Group open\n{\nleaf 3\n");
    { CTextFileLoader loader; assert(loader.Load(bad_file) && loader.SetChildNode("open")); int v=0; assert(loader.GetTokenInteger("leaf",&v) && v==3); }
    CTextFileLoader::DestroySystem();
    Write(bad_file,"before 1\n}\nafter 2\n");
    { CTextFileLoader loader; assert(loader.Load(bad_file) && loader.IsToken("before") && !loader.IsToken("after")); }
    assert(logs>=5); CTextFileLoader::DestroySystem();
    std::puts("text loader passed: real files/groups/lists, old conversions, retained reload state and shared pool ownership");
}
