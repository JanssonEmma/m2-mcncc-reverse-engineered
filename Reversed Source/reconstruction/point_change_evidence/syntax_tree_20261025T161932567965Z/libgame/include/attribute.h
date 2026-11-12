#ifndef __INC_METIN_II_ATTRIBUTE_H__
#define __INC_METIN_II_ATTRIBUTE_H__
#include "../../libthecore/include/typedef.h"

enum EDataType
{
    D_DWORD,
    D_WORD,
    D_BYTE
};

class CAttribute
{
    public:
	CAttribute(int width, int height); 
	CAttribute(DWORD * attr, int width, int height); 
	~CAttribute();
	void Alloc();
	int GetDataType();
	void * GetDataPtr();
	void Set(int x, int y, DWORD attr);
	void Remove(int x, int y, DWORD attr);
	DWORD Get(int x, int y);
	void CopyRow(int y, DWORD * row);

    private:
	void Initialize(int width, int height);

    private:
	int dataType;
	DWORD defaultAttr;
	int width, height;

	void * data;

	BYTE **	bytePtr;
	WORD **	wordPtr;
	DWORD ** dwordPtr;
};

#endif


