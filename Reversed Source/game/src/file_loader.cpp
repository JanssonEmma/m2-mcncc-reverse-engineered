#include <cassert>
#include <cstring>
#include "file_loader.h"

// 0x080774c0 / 0x080774f0
CMemoryTextFileLoader::CMemoryTextFileLoader() {}

// 0x08077520 / 0x080775b0 / 0x08077640
CMemoryTextFileLoader::~CMemoryTextFileLoader() {}

// 0x080776e0: '#' has no special meaning in this version.
bool CMemoryTextFileLoader::SplitLine(DWORD index, TTokenVector* tokens,
                                     const char* delimiters)
{
    tokens->clear();
    const std::string& line = GetLineString(index);
    DWORD base = 0;
    do
    {
        int begin = static_cast<int>(line.find_first_not_of(delimiters, base));
        if (begin < 0)
            return false;
        int end;
        if (line[begin] == '"')
        {
            ++begin;
            end = static_cast<int>(line.find_first_of("\"", begin));
            if (end < 0)
                return false;
            base = static_cast<DWORD>(end + 1);
        }
        else
        {
            end = static_cast<int>(line.find_first_of(delimiters, begin));
            base = static_cast<DWORD>(end);
        }
        tokens->push_back(line.substr(begin, end - begin));
        if (static_cast<int>(line.find_first_not_of(delimiters, base)) < 0)
            break;
    } while (base < line.length());
    return true;
}

// 0x08077930
DWORD CMemoryTextFileLoader::GetLineCount()
{
    return static_cast<DWORD>(m_stLineVector.size());
}

// 0x08077950
bool CMemoryTextFileLoader::CheckLineIndex(DWORD line)
{
    return line < m_stLineVector.size();
}

// 0x08077970
const std::string& CMemoryTextFileLoader::GetLineString(DWORD line)
{
    assert(CheckLineIndex(line));
    return m_stLineVector[line];
}

// 0x080779e0. Adjacent newline bytes coalesce in pairs, regardless of their order.
void CMemoryTextFileLoader::Bind(int size, const void* buffer)
{
    m_stLineVector.clear();
    const char* bytes = static_cast<const char*>(buffer);
    std::string line;
    int position = 0;
    while (position < size)
    {
        char byte = bytes[position++];
        if (byte == '\n' || byte == '\r')
        {
            if (position < size && (bytes[position] == '\n' || bytes[position] == '\r'))
                ++position;
            m_stLineVector.push_back(line);
            line = "";
        }
        else if (static_cast<signed char>(byte) < 0)
        {
            // Observed two-byte character handling has no remaining-size guard.
            line.append(bytes + position - 1, 2);
            ++position;
        }
        else
            line += byte;
    }
    m_stLineVector.push_back(line);
}

CMemoryFileLoader::CMemoryFileLoader(int size, const void* memoryFile)
    : m_pcBase(static_cast<const char*>(memoryFile)), m_size(size), m_pos(0)
{
    assert(memoryFile);
}

CMemoryFileLoader::~CMemoryFileLoader() = default;

int CMemoryFileLoader::GetSize()
{
    return m_size;
}

int CMemoryFileLoader::GetPosition()
{
    return m_pos;
}

bool CMemoryFileLoader::IsReadableSize(int size)
{
    return size >= 0 && m_pos <= m_size && size <= m_size - m_pos;
}

bool CMemoryFileLoader::Read(int size, void* destination)
{
    if (!IsReadableSize(size))
        return false;
    if (size != 0)
        std::memcpy(destination, GetCurrentPositionPointer(), size);
    m_pos += size;
    return true;
}

const char* CMemoryFileLoader::GetCurrentPositionPointer()
{
    assert(m_pcBase);
    return m_pcBase + m_pos;
}
