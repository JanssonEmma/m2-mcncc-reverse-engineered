/*
 *    Filename: hangul.c
 * Description: 한글 관련 구현 소스
 *
 *      Author: 비엽 aka. Cronan
 */
#define __LIBTHECORE__
#include "../include/hangul.h"
#include "../include/kstbl.h"
#include <ctype.h>
#include <string.h>

int is_hangul(const BYTE * str)
{
    if (str[0] >= 0xb0 && str[0] <= 0xc8 && str[1] >= 0xa1 && str[1] <= 0xfe)
	return 1;

    return 0;
}

int check_han(const char *str)
{
    int code;
    size_t i;
    size_t len;

    if (!str || !*str)
	return 0;

    len = strlen(str);
    for (i = 0; i < len; i += 2) 
    {
	if (isnhspace((unsigned char)str[i]))
	    return 0;

	if (isalpha((unsigned char)str[i]) || isdigit((unsigned char)str[i]))
	    continue;

	code = (unsigned char)str[i];

	if (code < 176 || code > 200)
	    return 0;
    }

    return 1;
}

const char *first_han(const BYTE *str)
{
    unsigned char high, low;
    int len, i;
    char *p = "그외";

    static const char* wansung[] =
    {
	"가", "가", "나", "다", "다",
	"라", "마", "바", "바", "사",
	"사", "아", "자", "자", "차", 
	"카", "타", "파", "하", ""
    };

    static const char* johab[] =
    {
	"늏", "똞", "륾", "봞", "쁝",
	"쏿", "쟞", "쨅", "쮉", "촡",
	"캻", "큑", "퇫", "펈", "픞",
	"훍", "?", "?", "?", "" 
    };

    if (!str)
	return p;
    len = strlen((const char*) str);

    if (len < 2)
	return p;

    high = str[0];
    low  = str[1];

    if (!is_hangul(str))
    {
	return p;
    }

    high = (KStbl[(high - 0xb0) * 94 + low - 0xa1] >> 8) & 0x7c;

    for (i = 0; johab[i][0]; i++) 
    {
	low = (johab[i][0] & 0x7f);

	if (low == high)
	    return (wansung[i]);
    }

    return (p);
}

int under_han(const void * orig)
{
    const BYTE * str = (const BYTE *) orig;
    BYTE high, low;
    int len;

    if (!str)
	return 0;
    len = strlen((const char*) str);

    if (len < 2) 
	return 0;

    if (str[len - 1] == ')')
    {
	while (len && str[len] != '(')
	    len--;
    }

    if (len < 2)
	return 0;

    high = str[len - 2];
    low  = str[len - 1];

    if (!is_hangul(&str[len - 2])) 
	return 0; 

    high = KStbl[(high - 0xb0) * 94 + low - 0xa1] & 0x1f;

    if (high < 2)
	return 0;

    if (high > 28) 
	return 0;

    return 1;
}
