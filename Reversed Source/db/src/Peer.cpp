#include "stdafx.h"
#include "Peer.h"

CPeer::CPeer()
{
	m_state = 0;
	m_bChannel = 0;
	m_dwHandle = 0;
	m_dwUserCount = 0;
	m_wListenPort = 0;
	m_wP2PPort = 0;

	memset(m_abMaps, 0, sizeof(m_abMaps));

	m_kItemRange.Init();
}

CPeer::~CPeer()
{
	Close();
}

void CPeer::OnAccept()
{
	m_state = STATE_PLAYING;

	static DWORD current_handle = 0;
	m_dwHandle = ++current_handle;

	sys_log(0, "Connection accepted. (host: %s handle: %lu fd: %d)", m_host, m_dwHandle, m_fd);
}

void CPeer::OnConnect()
{
	sys_log(0, "Connection established. (host: %s handle: %lu fd: %d)", m_host, m_dwHandle, m_fd);
	m_state = STATE_PLAYING;
}

void CPeer::OnClose()
{
	m_state = STATE_CLOSE;

	sys_log(0, "Connection closed. (host: %s)", m_host);
}

DWORD CPeer::GetHandle()
{
	return m_dwHandle;
}

DWORD CPeer::GetUserCount()
{
	return m_dwUserCount;
}

void CPeer::SetUserCount(DWORD dwCount)
{
	m_dwUserCount = dwCount;
}

bool CPeer::PeekPacket(int & iBytesProceed, BYTE & header, DWORD & dwHandle, DWORD & dwLength, const char ** data)
{
	static_assert(sizeof(HEADER) == 9, "DB packet header must be nine bytes");
	constexpr int headerSize = sizeof(HEADER);
	const int received = GetRecvLength();
	if (HasIOError())
		return false;

	if (iBytesProceed < 0 || iBytesProceed > received || !data)
	{
		FailIO();
		return false;
	}

	const int available = received - iBytesProceed;
	if (available < headerSize)
		return false;

	const char* buf = static_cast<const char*>(GetRecvBuffer()) + iBytesProceed;
	HEADER frame;
	memcpy(&frame, buf, sizeof(frame));
	header = frame.bHeader;
	dwHandle = frame.dwHandle;
	dwLength = frame.dwSize;

	if (dwLength > static_cast<DWORD>(MAX_INPUT_LEN - headerSize))
	{
		sys_err("Peer packet too large: %lu bytes", static_cast<unsigned long>(dwLength));
		FailIO();
		return false;
	}
	if (dwLength > static_cast<DWORD>(available - headerSize))
		return false;

	*data = buf + headerSize;
	iBytesProceed += headerSize + static_cast<int>(dwLength);
	return true;
}

void CPeer::EncodeHeader(BYTE header, DWORD dwHandle, DWORD dwSize)
{
	sys_log(1, "EncodeHeader %u handle %lu size %lu", header, dwHandle, dwSize);

	const HEADER frame = {header, static_cast<std::uint32_t>(dwHandle), static_cast<std::uint32_t>(dwSize)};
	Encode(&frame, sizeof(frame));
}

void CPeer::EncodeReturn(BYTE header, DWORD dwHandle)
{
	EncodeHeader(header, dwHandle, 0);
}

int CPeer::Send()
{
	if (m_state == STATE_CLOSE)
		return -1;

	return (CPeerBase::Send());
}

void CPeer::SetP2PPort(WORD wPort)
{
	m_wP2PPort = wPort;
}

void CPeer::SetMaps(BYTE* maps)
{
	thecore_memcpy(m_abMaps, maps, sizeof(m_abMaps));
}

bool CPeer::SetItemRange(DWORD start, DWORD end)
{
	if (end < start)
		return false;

	m_kItemRange.start = start;
	m_kItemRange.end = end;
	return true;
}

bool CPeer::IsItemRangeCollision(DWORD start, DWORD end)
{
	if (end < start)
		return true;

	const int rangeStart = static_cast<int>(m_kItemRange.start);
	const int rangeEnd = static_cast<int>(m_kItemRange.end);
	const int candidateStart = static_cast<int>(start);
	const int candidateEnd = static_cast<int>(end);

	if (candidateEnd <= rangeStart || candidateStart >= rangeEnd)
		return false;

	sys_err("ITEM_RANGE_COLLISION: host: %s handle: %lu cur_range: (%d ~ %d) src_range: (%d ~ %d)",
			m_host, m_dwHandle, rangeStart, rangeEnd, candidateStart, candidateEnd);
	return true;
}
