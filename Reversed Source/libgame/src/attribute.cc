#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include "../include/attribute.h"

#define SET_BIT(var,bit)                ((var) |= (bit))
#define REMOVE_BIT(var,bit)             ((var) &= ~(bit))

void CAttribute::Initialize(int width, int height)
{
    dataType = D_DWORD;
    defaultAttr = 0;
    this->width = width;
    this->height = height;
    data = NULL;
    bytePtr = NULL;
    wordPtr = NULL;
    dwordPtr = NULL;
}

void CAttribute::Alloc()
{
    size_t memSize;

    switch (dataType)
    {
	case D_DWORD:
	    memSize = width * height * sizeof(uint32_t);
	    break;

	case D_WORD:
	    memSize = width * height * sizeof(WORD);
	    break;

	case D_BYTE:
	    memSize = width * height;
	    break;

	default:
	    assert(!"dataType error!");
	    return;
    }

    
    data = malloc(memSize);

    switch (dataType)
    {
	case D_DWORD:
	    dwordPtr = new uint32_t * [height];
	    dwordPtr[0] = static_cast<uint32_t*>(data);

	    for (int y = 1; y < height; ++y)
		dwordPtr[y] = dwordPtr[y - 1] + width;

	    for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		    dwordPtr[y][x] = defaultAttr;

	    break;

	case D_WORD:
	    wordPtr = new WORD * [height];
	    wordPtr[0] = (WORD *) data;

	    for (int y = 1; y < height; ++y)
		wordPtr[y] = wordPtr[y - 1] + width;

	    for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		    wordPtr[y][x] = defaultAttr;

	    break;

	case D_BYTE:
	    bytePtr = new BYTE * [height];
	    bytePtr[0] = (BYTE *) data;

	    for (int y = 1; y < height; ++y)
		bytePtr[y] = bytePtr[y - 1] + width;

	    for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		    bytePtr[y][x] = defaultAttr;

	    break;
    }
}

CAttribute::CAttribute(int width, int height) 
{
    Initialize(width, height);
    Alloc();
}

CAttribute::CAttribute(DWORD * attr, int width, int height) 
{
    Initialize(width, height);

    int i;
    int size = width * height;

    for (i = 0; i < size; ++i)
	if (static_cast<uint32_t>(attr[0]) != static_cast<uint32_t>(attr[i]))
	    break;

    
    if (i == size)
	defaultAttr = attr[0];
    else
    {
	uint32_t allAttr = 0;

	for (i = 0; i < size; ++i)
	    allAttr |= static_cast<uint32_t>(attr[i]);

	
	if (!(allAttr & 0xffffff00))
	    dataType = D_BYTE;
	
	else if (!(allAttr & 0xffff0000))
	    dataType = D_WORD;
	else 
	    dataType = D_DWORD;

	Alloc();

	if (dataType == D_DWORD) 
	    std::copy(attr, attr + size, static_cast<uint32_t*>(data));
	else
	{
	    
	    DWORD * pdw = (DWORD *) attr;

	    if (dataType == D_BYTE)
	    {
		for (int y = 0; y < height; ++y)
		    for (int x = 0; x < width; ++x)
			bytePtr[y][x] = *(pdw++);
	    }
	    else if (dataType == D_WORD)
	    {
		for (int y = 0; y < height; ++y)
		    for (int x = 0; x < width; ++x)
			wordPtr[y][x] = *(pdw++);
	    }
	}
    }
}

CAttribute::~CAttribute()
{
    if (data)
	free(data);

    if (bytePtr)
	delete [] bytePtr;

    if (wordPtr)
	delete [] wordPtr;

    if (dwordPtr)
	delete [] dwordPtr;
}

int CAttribute::GetDataType()
{
    return dataType;
}

void * CAttribute::GetDataPtr()
{
    return data;
}

void CAttribute::Set(int x, int y, DWORD attr)
{
    if (x > width || y > height)
	return;

    if (!data)
	Alloc();

    if (bytePtr)
    {
	SET_BIT(bytePtr[y][x], attr);
	return;
    }

    if (wordPtr)
    {
	SET_BIT(wordPtr[y][x], attr);
	return;
    }

    SET_BIT(dwordPtr[y][x], attr);
}

void CAttribute::Remove(int x, int y, DWORD attr)
{
    if (x > width || y > height)
	return;

    if (!data) 
	return;

    if (bytePtr)
    {
	REMOVE_BIT(bytePtr[y][x], attr);
	return;
    }

    if (wordPtr)
    {
	REMOVE_BIT(wordPtr[y][x], attr);
	return;
    }

    REMOVE_BIT(dwordPtr[y][x], attr);
}

DWORD CAttribute::Get(int x, int y)
{
    if (x > width || y > height)
	return 0;

    if (!data)
	return defaultAttr;

    if (bytePtr)
	return bytePtr[y][x];

    if (wordPtr)
	return wordPtr[y][x];

    return dwordPtr[y][x];
}

void CAttribute::CopyRow(int y, DWORD * row)
{
    if (!data)
    {
	for (int x = 0; x < width; ++x)
	    row[x] = defaultAttr;

	return;
    }

    if (dwordPtr)
	std::copy(dwordPtr[y], dwordPtr[y] + width, row);
    else
    {
	if (bytePtr)
	{
	    for (int x = 0; x < width; ++x)
		row[x] = bytePtr[y][x];
	}
	else if (wordPtr)
	{
	    for (int x = 0; x < width; ++x)
		row[x] = wordPtr[y][x];
	}
    }
}


