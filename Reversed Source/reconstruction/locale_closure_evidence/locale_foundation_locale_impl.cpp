// Compile the complete real locale.cpp with only C stdio boundaries observed.
#include "../../game/src/locale.hpp"
#include "../../game/src/locale_service.h"
#include "../../game/src/config.h"
#include "../../libthecore/include/log.h"
#include <map>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
extern "C" {
FILE* locale_test_fopen(const char*,const char*);
int locale_test_fseek(FILE*,long,int);
long locale_test_ftell(FILE*);
std::size_t locale_test_fread(void*,std::size_t,std::size_t,FILE*);
int locale_test_fclose(FILE*);
}
namespace std {
    using ::locale_test_fopen;using ::locale_test_fseek;using ::locale_test_ftell;
    using ::locale_test_fread;using ::locale_test_fclose;
}
#define fopen locale_test_fopen
#define fseek locale_test_fseek
#define ftell locale_test_ftell
#define fread locale_test_fread
#define fclose locale_test_fclose
#include "../../game/src/locale.cpp"
#undef fclose
#undef fread
#undef ftell
#undef fseek
#undef fopen
