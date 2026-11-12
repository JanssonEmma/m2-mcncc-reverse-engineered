#include "SymTable.h"
CSymTable::CSymTable(int kind,std::string text) : dVal(0.0),token(kind),strlex(text) {}
CSymTable::~CSymTable() {}
