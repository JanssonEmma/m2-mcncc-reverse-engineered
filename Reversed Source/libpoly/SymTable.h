// Complete original CSymTable data/API. No DWARF available for libpoly: names
// reference-guided; types/offsets/body/virtual interface corroborated by original
// 081be8e0/081be950. Old sizeof20: vptr0,double4,int12,string16.
#ifndef MCNCC_POLY_SYMTABLE_H
#define MCNCC_POLY_SYMTABLE_H
#include <string>
class CSymTable {
public:
    CSymTable(int token,std::string text);
    virtual ~CSymTable();
    double dVal;
    int token;
    std::string strlex;
};
#endif
