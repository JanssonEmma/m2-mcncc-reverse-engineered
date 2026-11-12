// Verify the complete original locale record and recovered public signatures.
#include "../../game/src/locale.cpp"
#include <cstddef>
typedef char locale_record_size[(sizeof(CLocaleText)==8)?1:-1];
typedef char locale_record_offset[(offsetof(CLocaleText,m_strings)==0)?1:-1];
typedef char locale_slot_count[(sizeof(((CLocaleText*)0)->m_strings)/sizeof(char*)==2)?1:-1];
void (*locale_add_signature)(const char**)=locale_add;
const char* (*locale_find_signature)(const char*,int)=locale_find;

char* (*fread_string_signature)(const char*)=fread_string;
const char* (*quote_end_signature)(const char*)=quote_find_end;
char* (*locale_convert_signature)(const char*,int)=locale_convert;
void (*locale_init_signature)(const char*)=locale_init;
