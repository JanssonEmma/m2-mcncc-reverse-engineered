// Game utility functions reconstructed from the 2009 server.
#include "utils.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
#include <cstdio>
#include <cmath>
#include <cctype>
#include <cstring>
#include <strings.h>

// 08146cc0
int dice(int number, int size)
{
    if (number < 1 || size < 1)
        return 0;
    unsigned int sum = 0;
    for (int roll = 0; roll < number; ++roll)
        sum += 1 + thecore_random() % static_cast<unsigned int>(size);
    return static_cast<int>(sum);
}

// 08146d00
int string_dice(const char* text)
{
    int number = 0;
    int addition = 0;
    std::sscanf(text, " %dd%d ", &number, &addition);
    return static_cast<int>(static_cast<unsigned int>(dice(number, 6)) + addition);
}

// 08146d60
int decision(int roll, int target)
{
    if (roll == 3 || roll == 4 || (target == 15 && roll == 5) ||
        (target > 15 && roll < 7))
        return 2;
    const int margin = static_cast<int>(static_cast<unsigned int>(roll) - target);
    if (roll == 18 || (roll == 17 && target < 16) || margin > 9)
        return -1;
    return roll < target;
}

// 08146dc0
const char* get_filename_ext(const char* filename)
{
    const char* dot = std::strrchr(filename, '.');
    return dot ? dot + 1 : nullptr;
}

// 08146df0
bool is_filename_ext(const char* filename, const char* extension)
{
    const char* actual = get_filename_ext(filename);
    return actual && strcasecmp(extension, actual) == 0;
}

// 08147110
char* first_cmd(char* argument, char* first, int limit, int* length)
{
    skip_spaces(&argument);
    int copied = 0;
    while (*argument && copied < limit) {
        const auto byte = static_cast<unsigned char>(*argument);
        if ((byte & 0xe0) <= 0x90 && std::isspace(byte))
            break;
        first[copied++] = byte >= 'A' && byte <= 'Z' ? byte + ('a' - 'A') : byte;
        ++argument;
    }
    first[copied] = '\0';
    *length = copied;
    return argument;
}

// 08147070
char* last_cmd(char* argument, char* last)
{
    char* split = std::strrchr(argument, ' ');
    if (!split) {
        std::strcpy(last, argument);
        *argument = '\0';
        return argument;
    }

    *split++ = '\0';
    while (*split) {
        const auto byte = static_cast<unsigned char>(*split++);
        *last++ = byte >= 'A' && byte <= 'Z' ? byte + ('a' - 'A') : byte;
    }
    *last = '\0';
    return argument;
}

// 08147200
char* text_gauge(int current, int maximum, int length)
{
    static char gauge[128];
    const int width = length < 0 ? 0 : (length > 100 ? 100 : length);
    const int product = static_cast<int>(static_cast<unsigned int>(current) * width);
    const long long count = maximum ? static_cast<long long>(product) / maximum : 0;
    const int filled = count < 0 ? 0 : (count > width ? width : static_cast<int>(count));
    gauge[0] = '[';
    std::memset(gauge + 1, '=', filled);
    std::memset(gauge + 1 + filled, ' ', width - filled);
    std::snprintf(gauge + width + 1, sizeof(gauge) - width - 1, "] %2d/%2d", current, maximum);
    return gauge;
}

// 081472b0
char* get_time_string(int milliseconds)
{
    static char buffer[256];
    const int seconds = milliseconds / 1000;
    const int days = seconds / 86400;
    const int hours = seconds % 86400 / 3600;
    const int remainder = seconds % 3600;
    int offset = 0;
    if (days > 0)
        offset += std::snprintf(buffer + offset, sizeof(buffer) - offset, "%dd ", days);
    if (hours > 0)
        offset += std::snprintf(buffer + offset, sizeof(buffer) - offset, "%dh ", hours);
    std::snprintf(buffer + offset, sizeof(buffer) - offset, "%dm %ds", remainder / 60, remainder % 60);
    return buffer;
}

// 08147560
int parse_time_str(const char* text)
{
    unsigned int total = 0;
    unsigned int number = 0;
    for (; *text; ++text) {
        if (*text >= '0' && *text <= '9') {
            number = number * 10 + (*text - '0');
            continue;
        }
        switch (*text) {
            case 'D': case 'd': total += number * 86400u; break;
            case 'H': case 'h': total += number * 3600u; break;
            case 'M': case 'm': total += number * 60u; break;
            case 'S': case 's': total += number; break;
            default: return -1;
        }
        number = 0;
    }
    return static_cast<int>(total + number);
}

static int global_time_gap = 0;
time_t get_global_time()
{
    return static_cast<time_t>(static_cast<int>(
        static_cast<unsigned int>(time(NULL)) + static_cast<unsigned int>(global_time_gap)));
}
void set_global_time(time_t t)
{
    global_time_gap = static_cast<int>(static_cast<unsigned int>(t) -
                                      static_cast<unsigned int>(time(NULL)));
    char time_str_buf[33];
    snprintf(time_str_buf, 32, "%s", time_str(get_global_time()));
    sys_log(0, "GLOBAL_TIME: %s time_gap %d", time_str_buf, global_time_gap);
}

// 08147420: preserve x86 signed32 register wrapping before signed division.
int CalculateDuration(int speed,int duration) {
    int delta=static_cast<int>(100U-static_cast<unsigned int>(speed));
    int factor;
    if(delta>0) factor=static_cast<int>(static_cast<unsigned int>(delta)+100U);
    else if(delta==0) factor=100;
    else factor=10000/static_cast<int>(100U-static_cast<unsigned int>(delta));
    return static_cast<int>(static_cast<unsigned int>(factor)*static_cast<unsigned int>(duration))/100;
}

// Append at original game/src/utils.cpp; exact old08146e30.
void str_lower(const char* src,char* dest,int len)
{
    int i=0;
    while(*src && i<len) {
        unsigned char c=static_cast<unsigned char>(*src);
        if(c>='A' && c<='Z') c=static_cast<unsigned char>(c+('a'-'A'));
        *dest++=static_cast<char>(c);++src;++i;
    }
    *dest=0;
}

// 08147480. Include <cmath>. Exact zero rejection (not reference epsilon).
long double gauss_random_extended(float avg,float sigma) {
    static bool haveNextGaussian=false;
    static float nextGaussian=0.0f;
    if(haveNextGaussian) {
        haveNextGaussian=false;
        return static_cast<long double>(nextGaussian) * sigma + avg;
    }
    double v1,v2,s;
    do {
        v1=(static_cast<int>(thecore_random()&0x1fffffUL)-0x100000)*0.00000095367431640625f;
        v2=(static_cast<int>(thecore_random()&0x1fffffUL)-0x100000)*0.00000095367431640625f;
        s=v1*v1+v2*v2;
    } while(s>=1.0 || s==0.0);
    // Original logf(float(s)), sqrtf(float(...)); v1/v2/s are doubles.
    const float logRadius = ::logf(static_cast<float>(s));
    const float multiplier = ::sqrtf(static_cast<float>(-2.0L * logRadius / s));
    nextGaussian=static_cast<float>(v2*multiplier);
    haveNextGaussian=true;
    return static_cast<float>(static_cast<long double>(v1) * multiplier * sigma + avg);
}

float gauss_random(float avg,float sigma) {
    return static_cast<float>(gauss_random_extended(avg, sigma));
}

// 08146ea0
void skip_spaces(char** text)
{
    auto byte = static_cast<unsigned char>(**text);
    while ((byte & 0xe0) <= 0x90 && std::isspace(byte)) {
        ++*text;
        byte = static_cast<unsigned char>(**text);
    }
}

// 08146fd0
char* any_one_arg(char* argument, char* first)
{
    skip_spaces(&argument);
    while (*argument && !std::isspace(static_cast<unsigned char>(*argument)))
        *first++ = *argument++;
    *first = '\0';
    return argument;
}

// 08147030
void half_chop(char* argument, char* first, char* rest)
{
    char* remaining = any_one_arg(argument, first);
    skip_spaces(&remaining);
    std::strcpy(rest, remaining);
}

// 081471a0
float linear_interpolate(float begin, float end, float rate)
{
    return static_cast<float>(begin + (static_cast<long double>(end) - begin) * rate);
}

// 081471c0
int highest_array_element(int* array, int size)
{
    if (size <= 0)
        return 0;

    int highest = 0;
    int highestValue = array[0];
    for (int index = 1; index < size; ++index) {
        const int value = array[index];
        if (highestValue <= value) {
            highest = index;
            highestValue = value;
        }
    }
    return highest;
}

char* one_argument(char* argument, char* first)
{
    if (!argument) {
        _sys_err("one_argument", 96, "one_argument received a NULL pointer!");
        *first = 0;
        return nullptr;
    }

    skip_spaces(&argument);
    bool quoted = false;
    while (*argument) {
        const auto byte = static_cast<unsigned char>(*argument);
        if (byte == '"')
            quoted = !quoted;
        else {
            if (!quoted && (byte & 0xe0) <= 0x90 && std::isspace(byte))
                break;
            *first++ = *argument;
        }
        ++argument;
    }
    *first = 0;
    skip_spaces(&argument);
    return argument;
}

char* two_arguments(char* argument, char* first, char* second)
{
    return one_argument(one_argument(argument, first), second);
}
