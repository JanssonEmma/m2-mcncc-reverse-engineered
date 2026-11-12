#include "../../libthecore/include/utils.h"
#include <cstring>


namespace {
constexpr unsigned matrixLength = 96;
constexpr char matrixHex[] = "0123456789ABCDEF";
}

bool EncodeMatrix(const char* source, const char* password, char* result, unsigned size)
{
    if (!source || !password || !result || size <= matrixLength * 2 ||
        std::strlen(source) != matrixLength || std::strlen(password) != matrixLength)
        return false;

    std::memset(result, 0, size);
    for (unsigned i = 0; i < matrixLength; ++i) {
        const auto encoded = static_cast<unsigned char>(source[i] ^ password[i]);
        result[2 * i] = matrixHex[encoded >> 4];
        result[2 * i + 1] = matrixHex[encoded & 15];
    }
    return true;
}

bool DecodeMatrix(const char* source, const char* password, char* result, unsigned size)
{
    if (!source || !password || !result || size <= matrixLength ||
        std::strlen(source) != matrixLength * 2 || std::strlen(password) != matrixLength)
        return false;

    std::memset(result, 0, size);
    for (unsigned i = 0; i < matrixLength; ++i) {
        const char* high = std::strchr(matrixHex, source[2 * i]);
        const char* low = std::strchr(matrixHex, source[2 * i + 1]);
        if (!high || !low)
            return false;
        const int encoded = (high - matrixHex) * 16 + (low - matrixHex);
        result[i] = static_cast<char>(encoded ^ password[i]);
    }
    return true;
}

bool ChkCoordinate(unsigned long rows, unsigned long cols, const char* matrix, const char* answer)
{
    if (!matrix || !answer || std::strlen(matrix) != matrixLength || std::strlen(answer) != 8)
        return false;

    for (unsigned index = 0; index < 4; ++index) {
        const unsigned shift = 24 - 8 * index;
        const unsigned row = (rows >> shift) & 0xff;
        const unsigned col = (cols >> shift) & 0xff;
        if (row >= 6 || col >= 8 || std::memcmp(matrix + 2 * (row * 8 + col), answer + 2 * index, 2))
            return false;
    }
    return true;
}


bool MatrixCardCheck(const char* source, const char* answer, unsigned long rows, unsigned cols)
{
    constexpr char decodeKey[] =
        "xEwx3Lb5fH2mnPaMh215cHTbCrFCSmh9yQ3FrybwPnD89QkNX4UTA8UdH41LnU4P94UnaeXDTk17dY5DLaSDPAwvEpMUNTxV";
    static_assert(sizeof(decodeKey) == 97, "original matrix decode key length");

    char matrix[matrixLength + 1];
    return DecodeMatrix(source, decodeKey, matrix, sizeof(matrix)) &&
           ChkCoordinate(rows, cols, matrix, answer);
}

// 080c8df0
void MatrixCardRndCoordinate(unsigned long& rows, unsigned long& cols)
{
    for (int shift = 24; shift >= 0; shift -= 8) {
        rows |= (thecore_random() % 6) << shift;
        cols |= (thecore_random() & 7) << shift;
    }
}
