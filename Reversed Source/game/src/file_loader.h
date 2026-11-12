#ifndef MCNCC_FILE_LOADER_H
#define MCNCC_FILE_LOADER_H

#include <string>
#include <vector>
#include "../../libthecore/include/typedef.h"

typedef std::vector<std::string> TTokenVector;

class CMemoryTextFileLoader
{
public:
    CMemoryTextFileLoader();
    virtual ~CMemoryTextFileLoader();
    void Bind(int size, const void* buffer);
    DWORD GetLineCount();
    bool CheckLineIndex(DWORD line);
    bool SplitLine(DWORD line, TTokenVector* tokens, const char* delimiters = " \t");
    const std::string& GetLineString(DWORD line);

protected:
    std::vector<std::string> m_stLineVector;
};

class CMemoryFileLoader
{
public:
    CMemoryFileLoader(int size, const void* memoryFile);
    virtual ~CMemoryFileLoader();
    int GetSize();
    int GetPosition();
    bool IsReadableSize(int size);
    bool Read(int size, void* destination);
    const char* GetCurrentPositionPointer();

private:
    const char* m_pcBase;
    int m_size;
    int m_pos;
};

#endif
