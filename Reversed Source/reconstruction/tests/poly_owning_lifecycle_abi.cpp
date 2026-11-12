#include <string>
#include <vector>
#include <list>
#include <map>
#define protected public
#include "../../game/src/skill.h"
#undef protected
static int (CPoly::*setvar)(const std::string&,double)=&CPoly::SetVar;
static int (CPoly::*findvar)(const std::string&)=&CPoly::find;
static int (CPoly::*insertvar)(const std::string&,int)=&CPoly::insert;
static void (CPoly::*clear)()=&CPoly::Clear;
static double CSymTable::*symbol_value=&CSymTable::dVal;
static int CSymTable::*symbol_token=&CSymTable::token;
static DWORD CSkillProto::*skill_type=&CSkillProto::bType;
// Old STL sizeof20/88/1184/manager16 are recorded evidence, not current-host ABI assertions.
