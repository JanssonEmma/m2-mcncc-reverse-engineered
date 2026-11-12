// Target-backed translation storage and lookup (08051680/08051890).
#include "locale.hpp"
#include "locale_service.h"
#include "config.h"
#include "../../libthecore/include/log.h"
#include <map>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
// 08051bb0
void PrintHexa(void* data, int size)
{
    static const char hex[] = "0123456789ABCDEF";
    const auto* bytes = static_cast<const unsigned char*>(data);
    for (int offset = 0; offset < size;) {
        const int count = size - offset < 16 ? size - offset : 16;
        char line[80];
        std::memset(line, ' ', sizeof(line));
        std::snprintf(line, 12, "%08Xh: ", static_cast<unsigned int>(offset));
        for (int index = 0; index < count; ++index) {
            const unsigned char byte = bytes[offset + index];
            char* column = line + 11 + 3 * index;
            column[0] = hex[byte >> 4];
            column[1] = hex[byte & 15];
            const bool lead = byte >= 0xb0 && byte <= 0xc8;
            const bool trail = index > 0 && bytes[offset + index - 1] >= 0xb0 &&
                               bytes[offset + index - 1] <= 0xc8 && byte > 0xa0 && byte != 0xff;
            if ((byte > 0x20 && byte < 0x80) || lead || trail)
                line[60 + index] = static_cast<char>(byte);
        }
        line[59] = '|';
        line[76] = '\0';
        std::fprintf(stderr, "%s\n", line);
        offset += count;
    }
}

struct CLocaleText {
    char* m_strings[2];
    CLocaleText() { m_strings[0]=0; m_strings[1]=0; }
};
std::map<std::string,CLocaleText> mapLocale;
int g_iUseLocale=0;
void locale_add(const char** strings) {
    if(!strings[0]) return;
    if(test_server) std::fprintf(stderr,"Added Locale String %s %s \n",strings[0],strings[1]);
    CLocaleText& text=mapLocale[strings[0]];
    if(strings[1]) {
        // The old implementation overwrites the pointer without reclaiming a prior translation.
        char* buf=new char[std::strlen(strings[1])+1];
        text.m_strings[1]=buf;
        std::strcpy(buf,strings[1]);
    }
}
extern "C" const char* locale_find(const char* string,int locale) {
    if(locale==0 || locale>1 || LC_IsKorea()) {
        if(test_server) _sys_err("locale_find",51,"LOCALE_ERROR: locae == 0  \"%s\";",string);
        return string;
    }
    std::map<std::string,CLocaleText>::iterator i=mapLocale.find(string);
    if(i==mapLocale.end()) {
        static char s_line[1024]="@0949";
        std::strncpy(s_line+5,string,1018);
        _sys_err("locale_find",61,"LOCALE_ERROR: \"%s\";",string);
        return s_line;
    }
    if(i->second.m_strings[locale]) return i->second.m_strings[locale];
    if(test_server) _sys_err("locale_find",70,"LOCALE_ERROR: text is NULL  \"%s\";",string);
    return string;
}

// Old parser helpers recovered from target instructions and DWARF.
// Deliberately preserves original allocator mismatch, FILE leak and partial-record leaks.
char *fread_string(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;
    fseek(fp, 0L, SEEK_END);
    int i = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    ++i;
    char *buf = (char *) malloc(i);
    memset(buf, 0, i);
    fread(buf, i - 1, 1, fp);
    return buf;
}

const char *quote_find_end(const char *string)
{
    const char *tmp = string;
    int quote = 0;
    while (*tmp)
    {
        if (quote && *tmp == '\\' && tmp[1])
        {
            if (tmp[1] == '"') { tmp += 2; continue; }
        }
        else if (*tmp == '"') quote = !quote;
        else if (!quote && *tmp == ';') return tmp;
        ++tmp;
    }
    return NULL;
}

char *locale_convert(const char *src, int len)
{
    if (!len) return NULL;
    char *buf = new char[len + 1];
    char *dest = buf;
    const char *tmp = src;
    int start = 0;
    char last_char = 0;
    int i, j;
    for (j = i = 0; i < len; ++i, ++tmp)
    {
        if (*tmp == '"')
        {
            if (last_char != '\\') start = !start;
            else goto ENCODE;
        }
        else if (*tmp == ';')
        {
            if (last_char != '\\' && !start) break;
            else goto ENCODE;
        }
        else if (start)
        {
ENCODE:
            if (*tmp == '\\' && tmp[1] == 'n')
            {
                *dest++ = '\n';
                ++tmp;
                last_char = '\n';
            }
            else { *dest++ = *tmp; last_char = *tmp; }
            ++j;
        }
    }
    if (!j) { free(buf); return NULL; }
    *dest = '\0';
    return buf;
}

void locale_init(const char *filename)
{
    char *buf = fread_string(filename);
    if (test_server) fprintf(stderr, "%s", buf);
    if (!buf)
    {
        fprintf(stderr, "SYSERR: locale_read: no file %s\n", filename);
        exit(1);
    }
    const char *tmp = buf;
    const char *end;
    char *strings[2];
    int i;
    do
    {
        strings[0] = strings[1] = NULL;
        if (*tmp == '"')
        {
            for (i = 0; i < 2; ++i)
            {
                if (!(end = quote_find_end(tmp))) break;
                strings[i] = locale_convert(tmp, int(end - tmp));
                tmp = ++end;
                while (*tmp == '\n' || *tmp == '\r' || *tmp == ' ') ++tmp;
                if (i == 1) break;
                if (*tmp != '"')
                {
                    fprintf(stderr, "SYSERR: locale_init: invalid format filename %s\n", filename);
                    break;
                }
            }
            if (!strings[0] || !strings[1]) break;
            locale_add((const char **)strings);
            for (i = 0; i < 2; ++i) if (strings[i]) free(strings[i]);
        }
        else
        {
            tmp = strchr(tmp, '\n');
            if (tmp) ++tmp;
        }
    } while (tmp && *tmp);
    free(buf);
}
