// vim: ts=8 sw=4
#ifndef __INC_PEER_H__
#define __INC_PEER_H__

#include "PeerBase.h"
#include "../../common/tables.h"

class CPeer : public CPeerBase
{
    protected:
	virtual void OnAccept();
	virtual void OnClose();
	virtual void OnConnect();

    public:
	using HEADER = TPacketDBHeader;
	enum EState
	{
	    STATE_CLOSE = 0,
	    STATE_PLAYING = 1
	};

	CPeer();
	virtual ~CPeer();

	void	EncodeHeader(BYTE header, DWORD dwHandle, DWORD dwSize);
	bool 	PeekPacket(int & iBytesProceed, BYTE & header, DWORD & dwHandle, DWORD & dwLength, const char ** data);
	void	EncodeReturn(BYTE header, DWORD dwHandle);

	void	ProcessInput();
	int	Send();

	DWORD	GetHandle();
	DWORD	GetUserCount();
	void	SetUserCount(DWORD dwCount);

	void	SetPublicIP(const char * ip)	{ m_stPublicIP = ip; }
	const char * GetPublicIP()		{ return m_stPublicIP.c_str(); }

	void	SetChannel(BYTE bChannel)	{ m_bChannel = bChannel; }
	BYTE	GetChannel()			{ return m_bChannel; }

	void	SetListenPort(WORD wPort) { m_wListenPort = wPort; }
	WORD	GetListenPort() { return m_wListenPort; }

	void	SetP2PPort(WORD wPort);
	WORD	GetP2PPort() { return m_wP2PPort; }

	void    SetMaps(BYTE* maps);
	BYTE*   GetMaps() { return m_abMaps; }

	bool    SetItemRange(DWORD start, DWORD end);
	bool    IsItemRangeCollision(DWORD start, DWORD end);

    private:
	int	m_state;

	BYTE	m_bChannel;
	DWORD	m_dwHandle;
	DWORD	m_dwUserCount;
	WORD	m_wListenPort;
	WORD	m_wP2PPort;
	BYTE    m_abMaps[32];

	struct ItemRange
	{
	    DWORD start;
	    DWORD end;

	    void Init()
	    {
		start = 0;
		end = 0;
	    }
	} m_kItemRange;

	std::string m_stPublicIP;
};

#endif
