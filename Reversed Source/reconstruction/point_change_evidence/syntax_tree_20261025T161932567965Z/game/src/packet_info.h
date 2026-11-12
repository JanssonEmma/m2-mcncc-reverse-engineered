// Recovered old packet registry; full target/container ABI limits in packet-info-recovery.md.
#ifndef MCNCC_GAME_PACKET_INFO_H
#define MCNCC_GAME_PACKET_INFO_H
#include <map>
#include <string>
#include "../../libthecore/include/typedef.h"
typedef struct SPacketElement {
    int iSize;
    std::string stName;
    int iCalled;
    DWORD dwLoad;
    bool bSequencePacket;
} TPacketElement;
class CPacketInfo {
public:
    CPacketInfo();
    virtual ~CPacketInfo();
    void Set(int header, int size, const char* name, bool sequence=false);
    bool Get(int header, int* size, const char** name);
    bool IsSequencePacket(int header);
    void Start();
    void End();
    void Log(const char* filename);
    TPacketElement* GetElement(int header);
protected:
    std::map<int, TPacketElement*> m_pPacketMap;
    TPacketElement* m_pCurrentPacket;
    DWORD m_dwStartTime;
};
class CPacketInfoCG : public CPacketInfo {
public:
    CPacketInfoCG();
    virtual ~CPacketInfoCG();
};
class CPacketInfoGG : public CPacketInfo {
public:
    CPacketInfoGG();
    virtual ~CPacketInfoGG();
};
class CPacketInfoUDP : public CPacketInfo {
public:
    CPacketInfoUDP();
    virtual ~CPacketInfoUDP();
};
#endif
