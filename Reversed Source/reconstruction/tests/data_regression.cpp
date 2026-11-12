#include <cassert>
#include <cstring>
#include "../../game/src/crc32.h"
#include "../../game/src/file_loader.h"

static crc_t BitwiseCRC(const char* bytes, std::size_t size)
{
    crc_t value = 0xffffffffUL;
    for (std::size_t i = 0; i < size; ++i)
    {
        value ^= static_cast<unsigned char>(bytes[i]);
        for (int bit = 0; bit < 8; ++bit)
            value = (value >> 1) ^ ((value & 1) ? 0xedb88320UL : 0);
    }
    return value ^ 0xffffffffUL;
}

static void CheckCRC()
{
    assert(get_crc32("", 0) == 0 && get_crc32_case("", 0) == 0);
    assert(get_crc32("123456789", 9) == 0xcbf43926UL);
    const char* lower = "abcdefghijklmnopqrstuvwxyz0123456789";
    const char* upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    assert(get_crc32(lower, 36) == 0xdfc6f27bUL);
    assert(get_crc32_case(lower, 36) == 0x1655ef21UL);
    assert(get_crc32_case(lower, 36) == get_crc32(upper, 36));
    char bytes[256];
    for (int i = 0; i < 256; ++i)
        bytes[i] = static_cast<char>(i);
    assert(get_crc32(bytes, sizeof(bytes)) == 0x29058c73UL);
    const std::size_t boundaries[] = {0, 1, 15, 16, 17, 31, 32, 33, 255, 256};
    for (std::size_t i = 0; i < sizeof(boundaries) / sizeof(boundaries[0]); ++i)
        assert(get_crc32(bytes, boundaries[i]) == BitwiseCRC(bytes, boundaries[i]));
    const char high[] = {static_cast<char>(0x80), static_cast<char>(0xff)};
    assert(get_crc32_case(high, 2) == get_crc32(high, 2));
    assert(get_fast_hash("", 0) == 0 && get_fast_hash("a", 1) == 97);
    assert(get_fast_hash(high, 1) == 0xffffff80UL); // Signed-byte extension.
    assert(get_fast_hash("abc", 3) == 0x84f09160UL);
    assert(get_fast_hash("hello", 5) == 0xec6d6be8UL);
    assert(get_fast_hash(high, 2) == 0x8000c97fUL);
    assert(get_fast_hash(bytes, sizeof(bytes)) == 0xab5f8800UL);
}

static void Bind(CMemoryTextFileLoader& loader, const char* text)
{
    loader.Bind(static_cast<int>(std::strlen(text)), text);
}

static void CheckLoader()
{
    CMemoryTextFileLoader loader;
    TTokenVector tokens(1, "old");
    assert(loader.GetLineCount() == 0 && !loader.CheckLineIndex(0));
    loader.Bind(0, 0);
    assert(loader.GetLineCount() == 1 && loader.GetLineString(0).empty());
    assert(!loader.SplitLine(0, &tokens) && tokens.empty());
    Bind(loader, "a\r\nb\n\rc\n");
    assert(loader.GetLineCount() == 4);
    assert(loader.GetLineString(0) == "a" && loader.GetLineString(1) == "b");
    assert(loader.GetLineString(2) == "c" && loader.GetLineString(3).empty());
    assert(!loader.CheckLineIndex(4));
    Bind(loader, "a\n\nb");
    assert(loader.GetLineCount() == 2 && loader.GetLineString(1) == "b");
    Bind(loader, "a\n\n\nb");
    assert(loader.GetLineCount() == 3 && loader.GetLineString(1).empty());

    Bind(loader, "  one\t\"two three\"tail  ");
    assert(loader.SplitLine(0, &tokens));
    assert(tokens.size() == 3 && tokens[0] == "one");
    assert(tokens[1] == "two three" && tokens[2] == "tail");
    Bind(loader, "\"\"");
    assert(loader.SplitLine(0, &tokens) && tokens.size() == 1 && tokens[0].empty());
    Bind(loader, "first \"unterminated");
    assert(!loader.SplitLine(0, &tokens) && tokens.size() == 1 && tokens[0] == "first");
    Bind(loader, "# comment");
    assert(loader.SplitLine(0, &tokens) && tokens.size() == 2);
    assert(tokens[0] == "#" && tokens[1] == "comment");
    Bind(loader, "a,,b,");
    assert(loader.SplitLine(0, &tokens, ",") && tokens.size() == 2);
    assert(tokens[0] == "a" && tokens[1] == "b");
    Bind(loader, " \t ");
    assert(!loader.SplitLine(0, &tokens) && tokens.empty());

    const char binary[] = {'a', 0, 'b'};
    loader.Bind(3, binary);
    assert(loader.GetLineString(0).size() == 3 && loader.GetLineString(0)[1] == 0);
    const char pair[] = {static_cast<char>(0x81), '\n', 'x'};
    loader.Bind(3, pair);
    assert(loader.GetLineCount() == 1 && loader.GetLineString(0).size() == 3);
    assert(loader.GetLineString(0)[1] == '\n'); // Trail byte is consumed as data.
    const char padded[] = {static_cast<char>(0x81), 'z'};
    loader.Bind(1, padded); // Backing allocation is deliberately larger than logical size.
    assert(loader.GetLineString(0).size() == 2 && loader.GetLineString(0)[1] == 'z');
}

int main()
{
    CheckCRC();
    CheckLoader();
    return 0;
}
