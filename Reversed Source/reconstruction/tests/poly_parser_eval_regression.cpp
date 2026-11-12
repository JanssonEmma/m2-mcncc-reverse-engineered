#include <string>
#include <vector>
#include <list>
#include <map>
#include <cmath>
#include <limits>
#include <cstring>
#include <cstdio>
#include <cassert>
#define protected public
#include "../../libpoly/Poly.h"
#undef protected
#include "../../libpoly/Constants.h"
extern double _random();
struct CtypeCall{int kind,value;CtypeCall(int k,int v):kind(k),value(v){}};
static std::vector<CtypeCall> ctypeCalls;
extern "C" int poly_test_isdigit(int n){ctypeCalls.push_back(CtypeCall('d',n));return n>='0'&&n<='9';}
extern "C" int poly_test_isalpha(int n){ctypeCalls.push_back(CtypeCall('a',n));return (n>='A'&&n<='Z')||(n>='a'&&n<='z');}
static long randomValues[32];static unsigned randomCount,randomUsed;
extern "C" long random(void){assert(randomUsed<randomCount);return randomValues[randomUsed++];}
static void Random(long n){randomValues[0]=n;randomCount=1;randomUsed=0;}
static bool Close(double a,double b){return std::fabs(a-b)<1e-10;}
static bool NegativeZero(double x){unsigned char b[sizeof(double)];std::memcpy(b,&x,sizeof x);return x==0&&(b[7]&0x80)!=0;}
static double Eval(CPoly& p){const std::vector<int> code=p.tokenBase;const std::vector<double> nums=p.numBase;bool error=p.ErrorOccur;double result=p.Eval();assert(code==p.tokenBase&&nums.size()==p.numBase.size()&&error==p.ErrorOccur);if(!nums.empty())assert(!std::memcmp(&nums[0],&p.numBase[0],nums.size()*sizeof(double)));return result;}
static void Expr(const char* text,double value){CPoly p;assert(p.Analyze(text));assert(Close(Eval(p),value));}
static void Invalid(const char* text){CPoly p;assert(!p.Analyze(text));assert(p.ErrorOccur&&Eval(p)==0);}
// Coherent full-object bytecode fixtures exercise complete Eval opcodes even
// where the original letters-only lexer cannot reach the predefined log10 name.
static double Unary(int op,double x){CPoly p;p.ErrorOccur=false;p.tokenBase.push_back(NUM);p.tokenBase.push_back(op);p.numBase.push_back(x);return Eval(p);}
static double Binary(int op,double a,double b){CPoly p;p.ErrorOccur=false;p.tokenBase.push_back(NUM);p.tokenBase.push_back(NUM);p.tokenBase.push_back(op);p.numBase.push_back(a);p.numBase.push_back(b);return Eval(p);}
int main(){
 assert(sizeof(int)==4&&sizeof(double)==8&&(char)0x80<0);
 {CPoly p;assert(p.Analyze("")==1&&p.ErrorOccur&&Eval(p)==0);p.SetStr("1+2");assert(p.ErrorOccur&&p.tokenBase.empty());assert(p.Analyze()==1&&Eval(p)==3);const unsigned n=p.tokenBase.size();p.SetStr("");assert(!p.ErrorOccur&&p.Analyze()==1&&p.tokenBase.size()==n&&Eval(p)==3);p.SetStr(p.strData);assert(p.strData.empty());}
 {CPoly p;assert(!p.Analyze(" \t")&&!p.ErrorOccur&&p.tokenBase.empty());} // Eval would read indeterminate stack; never execute.
 {CPoly p;p.SetStr(std::string("1\0+2",4));assert(p.strData.size()==4&&!p.Analyze()&&p.ErrorOccur);}
 Expr("2+3*4",14);Expr("(2+3)*4",20);Expr("2^3^2",64);Expr("8/2/2",2);Expr("7%4",3);Expr("mod(7,4)",3);Expr("-3+4",1);Expr("+2",2);Expr("1.",1);Expr("1.25",1.25);
 Expr("2147483648",-2147483648.0);Expr("4294967295",-1);Expr("4294967296",0);Expr("42949672960",0);
 Expr("1)ignored",1);Expr("1,2",1); // Original parser accepts terminal ')' or ',' without whole-string consumption.
 Expr("sqrt(9)",3);Expr("rt(4)",2);Expr("sin(0)",0);Expr("cos(0)",1);Expr("tan(0)",0);Expr("csc(0)",0);Expr("cosec(0)",0);Expr("sec(0)",1);Expr("cot(0)",0);Expr("abs(-3)",3);Expr("floor(2.9)",2);Expr("sign(-3)",-1);Expr("ln(1)",0);Expr("log(2,8)",3);Expr("min(4,2)",2);Expr("max(4,2)",4);
 Expr("pi+e",0); // Constructor's ErrorOccur=true rejects init's SetVar constants.
 Expr("sqrt(-1)",0);Expr("ln(-1)",0);Expr("log(1,8)",0);Expr("log(2,-1)",0);Expr("1/0",0);Expr("1%0",0);
 Invalid("@");Invalid(".5");Invalid("2(3)");Invalid("x1");Invalid("a_b");Invalid("SIN(0)");Invalid("log10(100)");Invalid("1.2.3");
 {CPoly p;assert(p.Analyze("1")&&p.Analyze("2")&&p.tokenBase.size()==2&&p.numBase.size()==2&&Eval(p)==2);}
 {CPoly p;assert(p.Analyze("X")&&!p.SetVar("x",2)&&p.SetVar("X",7)&&Eval(p)==7);}
 // Whitespace policy is exactly space/tab, not isspace. Newline raw10 collides with ID.
 {CPoly p;assert(p.Analyze("\n")&&p.tokenBase.size()==2&&p.tokenBase[0]==ID&&p.tokenBase[1]==0&&Eval(p)==0);}
 {CPoly p;assert(!p.Analyze("\v")&&!p.ErrorOccur&&p.tokenBase.empty());} // raw11 collides with EOS, no Eval.
 // Signed-char ctype arguments/returned raw byte preserved. Scripted external
 // classification makes this defined without claiming native ctype(-128) policy.
 const int raw[]={0x80,0xe9,0xff};for(unsigned i=0;i<3;++i){CPoly p;p.SetStr(std::string(1,(char)raw[i]));ctypeCalls.clear();assert(p.lexan()==raw[i]-256&&p.iToken==0&&p.uiLookPos==1);assert(ctypeCalls.size()==2&&ctypeCalls[0].kind=='d'&&ctypeCalls[1].kind=='a'&&ctypeCalls[0].value==raw[i]-256&&ctypeCalls[1].value==raw[i]-256);}
 {CPoly p;p.SetStr("abc12");assert(p.lexan()==ID&&p.uiLookPos==3&&p.lSymbol[p.iToken]->strlex=="abc");assert(p.lexan()==NUM&&p.iToken==12&&p.iNumToken==12&&p.lexan()==EOS);}
 {CPoly p;p.SetStr("12+3");p.uiLookPos=0;p.iLookAhead='+';p.ErrorOccur=false;ctypeCalls.clear();p.match('-');assert(p.ErrorOccur&&p.iErrorPos==0&&p.uiLookPos==0&&ctypeCalls.empty());p.match('+');assert(p.iLookAhead==NUM&&p.iToken==12&&p.uiLookPos==2&&p.ErrorOccur);}
 {CPoly p;p.uiLookPos=0xffffffffU;p.ErrorOccur=false;p.error();assert(p.ErrorOccur&&p.iErrorPos==-1&&p.uiLookPos==0xffffffffU);}
 // Complete emit mapping, including unknown-token error->Clear ownership path.
 {CPoly p;p.iNumToken=1.25;p.emit(NUM,999);assert(p.tokenBase.size()==1&&p.numBase.size()==1&&p.numBase[0]==1.25);p.emit(ID,17);assert(p.tokenBase[1]==ID&&p.tokenBase[2]==17);const int chars[]={'%','*','+','-','/','^'};const int ops[]={MOD,MUL,PLU,MIN,DIV,POW};for(unsigned i=0;i<6;++i){p.emit(chars[i],0);assert(p.tokenBase.back()==ops[i]);}for(int i=12;i<=28;++i){p.emit(i,0);assert(p.tokenBase.back()==i);}p.uiLookPos=77;p.strData="retained";p.emit(9999,0);assert(p.ErrorOccur&&p.iErrorPos==77&&p.STSize==0&&p.tokenBase.empty()&&p.numBase.empty()&&p.strData=="retained");}
 assert(Binary(MUL,3,4)==12&&Binary(PLU,3,4)==7&&Binary(MIN,3,4)==-1&&Binary(DIV,8,4)==2&&Binary(POW,2,3)==8&&Binary(MOD,7,4)==3);
 assert(Unary(ROOT,9)==3&&Unary(ROOT,-1)==0&&Unary(COS,0)==1&&Unary(SIN,0)==0&&Unary(TAN,0)==0&&Unary(CSC,0)==0&&Unary(SEC,0)==1&&Unary(COT,0)==0);
 assert(Unary(SIGN,-2)==-1&&Unary(SIGN,0)==0&&Unary(SIGN,2)==1&&!NegativeZero(Unary(SIGN,-0.0)));assert(Unary(ABS,-2)==2&&Unary(FLOOR,-1.25)==-2&&Unary(LN,1)==0&&Unary(LOG10,100)==2&&Unary(LOG10,-1)==0);
 assert(Close(Binary(LOG,2,8),3)&&Binary(LOG,1,8)==0&&Binary(LOG,2,0)==0&&Binary(MINF,3,4)==3&&Binary(MAXF,3,4)==4);
 assert(NegativeZero(Binary(MINF,0.0,-0.0))&&NegativeZero(Binary(MAXF,0.0,-0.0))&&!NegativeZero(Binary(MINF,-0.0,0.0)));
 const double nan=std::numeric_limits<double>::quiet_NaN();assert(Unary(SIGN,nan)==1&&Binary(MINF,nan,2)==2&&Binary(MAXF,nan,2)==2);double n=Binary(MINF,2,nan);assert(n!=n);n=Unary(ROOT,nan);assert(n!=n);n=Unary(TAN,nan);assert(n!=n);n=Binary(POW,-1,0.5);assert(n!=n);
 {CPoly p;p.ErrorOccur=false;p.tokenBase.push_back(255);assert(Eval(p)==0&&!p.ErrorOccur);}
 Random(0);assert(_random()==0&&randomUsed==1);Random(1073741824L);assert(_random()==0.5&&randomUsed==1);Random(-1);assert(_random()==-1.0/2147483648.0&&randomUsed==1);
 Random(0);assert(Binary(IRAND,2,4)==2&&randomUsed==1);Random(2147483647L);assert(Binary(IRAND,2,4)==4&&randomUsed==1);Random(1073741824L);assert(Binary(FRAND,2,6)==4&&randomUsed==1);
 Random(0);{CPoly p;assert(p.my_irandom(-2,-1)==-1&&randomUsed==1);}Random(1073741824L);{CPoly p;assert(p.my_irandom(2,-2)==1&&randomUsed==1);}Random(1073741824L);{CPoly p;assert(p.my_frandom(2,-2)==0&&randomUsed==1);}
 const char* integers[]={"number(2,4)","irandom(2,4)","irand(2,4)"};for(unsigned i=0;i<3;++i){Random(0);Expr(integers[i],2);assert(randomUsed==1);}const char* floats[]={"frandom(2,6)","frand(2,6)"};for(unsigned i=0;i<2;++i){Random(1073741824L);Expr(floats[i],4);assert(randomUsed==1);}
 std::puts("poly parser/evaluator passed: full real methods, old grammar/byte quirks, all opcodes, signed wrap and scripted OS RNG/ctype");
}
