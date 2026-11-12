// Recovered old locale storage, lookup and parser interface.
#ifndef MCNCC_GAME_LOCALE_HPP
#define MCNCC_GAME_LOCALE_HPP
extern int g_iUseLocale;
void locale_add(const char** strings);
char* fread_string(const char* filename);
const char* quote_find_end(const char* string);
char* locale_convert(const char* src,int len);
extern "C" void locale_init(const char* filename);
extern "C" const char* locale_find(const char* string,int locale);
#define LC_TEXT(str) locale_find(str,g_iUseLocale)
#endif
