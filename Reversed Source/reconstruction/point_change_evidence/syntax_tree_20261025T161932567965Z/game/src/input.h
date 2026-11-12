// Partial old input API: base frame dispatch and closed processor recovered.
#ifndef MCNCC_GAME_INPUT_H
#define MCNCC_GAME_INPUT_H
#include "packet_info.h"
class DESC;
class CInputProcessor {
public:
    CInputProcessor();
    virtual ~CInputProcessor();
    virtual bool Process(DESC* desc, const void* data, int bytes, int& consumed);
    virtual BYTE GetType()=0;
    void BindPacketInfo(CPacketInfo* info);
    void Pong(DESC* desc);
    void Handshake(DESC* desc, const char* data);
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data)=0;
    CPacketInfo* m_pPacketInfo;
    int m_iBufferLeft;
};
class CInputClose : public CInputProcessor {
public:
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};
// Complete old concrete processor data/virtual interface; Analyze behavior is pending.
// Append before input.h's final #endif. Default constructors for data-less
// Login/Main/Dead remain implicit, matching the target's lack of emitted symbols.
class CInputHandshake : public CInputProcessor {
public:
    CInputHandshake();
    virtual ~CInputHandshake();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
    void GuildMarkLogin(DESC* desc, const char* data);
    CPacketInfo* m_pMainPacketInfo;
};
class CInputLogin : public CInputProcessor {
public:
    virtual ~CInputLogin();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};
class CInputMain : public CInputProcessor {
public:
    virtual ~CInputMain();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};
class CInputDead : public CInputMain {
public:
    virtual ~CInputDead();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};
class CInputAuth : public CInputProcessor {
public:
    CInputAuth();
    virtual ~CInputAuth();
    virtual BYTE GetType();
protected:
    virtual int Analyze(DESC* desc, BYTE header, const char* data);
};

#endif
