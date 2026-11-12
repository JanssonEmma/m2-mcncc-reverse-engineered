#include <string>
#include <vector>
#include <list>
#define protected public
#include "../../libpoly/Poly.h"
#undef protected
static int (CPoly::*analyze)(const char*)=&CPoly::Analyze;
static double (CPoly::*eval)()=&CPoly::Eval;
static void (CPoly::*setstr)(const std::string&)=&CPoly::SetStr;
static int (CPoly::*lexan)()=&CPoly::lexan;
static int (CPoly::*irandom)(double,double)=&CPoly::my_irandom;
static double (CPoly::*frandom)(double,double)=&CPoly::my_frandom;
static void (CPoly::*emit)(int,int)=&CPoly::emit;
static void (CPoly::*match)(int)=&CPoly::match;
static void (CPoly::*expr)()=&CPoly::expr;
static void (CPoly::*term)()=&CPoly::term;
static void (CPoly::*factor)()=&CPoly::factor;
static void (CPoly::*expo)()=&CPoly::expo;
static void (CPoly::*error)()=&CPoly::error;
typedef char integer_is32[sizeof(int)==4?1:-1];
typedef char fp_is64[sizeof(double)==8?1:-1];

typedef char original_signed_char[((char)0x80<0)?1:-1];
