#include "empire_text_convert.h"
#include "locale.hpp"
#include "../../libthecore/include/utils.h"
#include <cstdio>

namespace {
struct STextConvertTable {
    char acUpper[26];
    char acLower[26];
    char aacHan[5000][2];
};

STextConvertTable g_aTextConvTable[3];
}

bool LoadEmpireTextConvertTable(DWORD empire, const char* filename)
{
    if (empire<1 || empire>3) return false;
    std::FILE* file=std::fopen(filename,"rb");
    if (!file) return false;
    STextConvertTable& table=g_aTextConvTable[empire-1];
    std::fread(table.acUpper,1,26,file);
    std::fread(table.acLower,1,26,file);
    std::fread(table.aacHan,1,4700,file);
    std::fclose(file);
    return true;
}

void ConvertEmpireText(DWORD empire, char* text, int percent)
{
    if (empire<1 || empire>3) return;
    STextConvertTable& table=g_aTextConvTable[empire-1];
    static const char china[5][3]={
        "\241\362","\243\243","\243\244","\241\371","\241\360"
    };
    while (*text) {
        bool twist=percent<number_ex(1,100,"empire_text_convert.cpp",0x34);
        unsigned char first=static_cast<unsigned char>(*text);
        if (first>=128) {
            if (twist) {
                if (g_iUseLocale==0) {
                    unsigned char second=static_cast<unsigned char>(text[1]);
                    if (first>=0xb0 && first<=0xc8 && second>=0xa1 && second<=0xfe) {
                        int index=(first-0xb0)*94+second-0xa1;
                        text[0]=table.aacHan[index][0];
                        text[1]=table.aacHan[index][1];
                    }
                } else {
                    int index=number_ex(0,4,"empire_text_convert.cpp",0x3e);
                    text[0]=china[index][0];
                    text[1]=china[index][1];
                }
            }
            text+=2;
        } else {
            if (twist && first>='a' && first<='z') *text=table.acLower[first-'a'];
            else if (twist && first>='A' && first<='Z') *text=table.acUpper[first-'A'];
            ++text;
        }
    }
}
