#include "stdafx.h"
#include "PeerBase.h"
#include <cstdint>

namespace
{
void CompactBuffer(LPBUFFER buffer)
{
	const int pending = buffer->length;
	memmove(buffer->mem_data, buffer_read_peek(buffer), pending);
	buffer_reset(buffer);
	buffer_write_proceed(buffer, pending);
}

bool ReserveBufferSpace(LPBUFFER buffer, int length, int limit)
{
    if (buffer_has_space(buffer) >= length)
        return true;
    CompactBuffer(buffer);
    if (buffer_has_space(buffer) >= length)
        return true;

    const int capacity = std::min(limit, std::max(buffer->mem_size * 2, buffer->length + length));
    char* storage = static_cast<char*>(realloc(buffer->mem_data, capacity));
    if (!storage)
        return false;
    buffer->mem_data = storage;
    buffer->mem_size = capacity;
    buffer->read_point = storage;
    buffer->write_point = storage + buffer->write_point_pos;
    return true;
}
}

CPeerBase::CPeerBase() : m_host{}, m_fd(static_cast<socket_t>(-1)), m_BytesRemain(0), m_outBuffer(NULL), m_inBuffer(NULL), m_ioError(false)
{
}

CPeerBase::~CPeerBase()
{
	Destroy();
}

void CPeerBase::Disconnect()
{
	if (m_fd != -1)
	{
		if (m_fdWatcher && m_fd < static_cast<socket_t>(m_fdWatcher->nfiles))
			fdwatch_forget_closed_fd(m_fdWatcher, m_fd);

		socket_close(m_fd);
		m_fd = static_cast<socket_t>(-1);
	}
}

void CPeerBase::Destroy()
{
	Disconnect();

	if (m_outBuffer)
	{
		buffer_delete(m_outBuffer);
		m_outBuffer = NULL;
	}

	if (m_inBuffer)
	{
		buffer_delete(m_inBuffer);
		m_inBuffer = NULL;
	}
	m_BytesRemain = 0;
}

bool CPeerBase::Accept(socket_t fd_accept, const std::set<std::string>& allowedAddresses)
{
	if (!m_fdWatcher || m_fd != -1)
		return false;

	Destroy();
	struct sockaddr_in peer;

	if ((m_fd = socket_accept(fd_accept, &peer)) == -1)
	{
		Destroy();
		return false;
	}

	if (m_fd >= static_cast<socket_t>(m_fdWatcher->nfiles))
	{
		Destroy();
		return false;
	}

	const char* address = inet_ntoa(peer.sin_addr);
	const std::string host = address ? address : "";
	if (host.empty() || !allowedAddresses.contains(host))
	{
		socket_close(m_fd);
		m_fd = static_cast<socket_t>(-1);
		return false;
	}

	m_ioError = false;
	socket_sndbuf(m_fd, 233016);
	const auto copied = host.copy(m_host, MAX_HOST_LENGTH);
	m_host[copied] = '\0';
	m_outBuffer = buffer_new(NULL, DEFAULT_PACKET_BUFFER_SIZE);
	m_inBuffer = buffer_new(NULL, DEFAULT_PACKET_BUFFER_SIZE);

	if (!m_outBuffer || !m_inBuffer)
	{
		Destroy();
		return false;
	}

	fdwatch_add_fd(m_fdWatcher, m_fd, this, FDW_READ, false);

	OnAccept();

	return true;
}

bool CPeerBase::Connect(const char* host, WORD port)
{
	if (!m_fdWatcher || m_fd != -1 || !host || !*host)
		return false;

	Destroy();
	strncpy(m_host, host, MAX_HOST_LENGTH);
	m_host[MAX_HOST_LENGTH] = '\0';

	if ((m_fd = socket_connect(host, port)) == -1)
		return false;

	if (m_fd >= static_cast<socket_t>(m_fdWatcher->nfiles))
	{
		Destroy();
		return false;
	}

	m_ioError = false;
	socket_nonblock(m_fd);
	m_outBuffer = buffer_new(NULL, DEFAULT_PACKET_BUFFER_SIZE);
	m_inBuffer = buffer_new(NULL, DEFAULT_PACKET_BUFFER_SIZE);

	if (!m_outBuffer || !m_inBuffer)
	{
		Destroy();
		return false;
	}

	fdwatch_add_fd(m_fdWatcher, m_fd, this, FDW_READ, false);

	OnConnect();
	return true;
}

void CPeerBase::Close()
{
	OnClose();
}

void CPeerBase::EncodeBYTE(BYTE b)
{
	Encode(&b, sizeof(b));
}

void CPeerBase::EncodeWORD(WORD w)
{
	Encode(&w, sizeof(w));
}

void CPeerBase::EncodeDWORD(DWORD dw)
{
	const auto value = static_cast<std::uint32_t>(dw);
	Encode(&value, sizeof(value));
}

void CPeerBase::Encode(const void* data, DWORD size)
{
	if (size == 0 || m_ioError || m_fd == -1)
		return;

	if (!m_outBuffer || !m_fdWatcher || !data ||
		size > MAX_OUTPUT_LEN - buffer_size(m_outBuffer))
	{
		sys_err("Peer output buffer limit or invalid write");
		FailIO();
		return;
	}

	const int length = static_cast<int>(size);
	if (!ReserveBufferSpace(m_outBuffer, length, MAX_OUTPUT_LEN))
	{
		sys_err("Could not grow peer output buffer");
		FailIO();
		return;
	}
	memcpy(buffer_write_peek(m_outBuffer), data, size);
	buffer_write_proceed(m_outBuffer, length);
	fdwatch_add_fd(m_fdWatcher, m_fd, this, FDW_WRITE, true);
}

void CPeerBase::FailIO()
{
	if (m_ioError)
		return;

	m_ioError = true;
	if (m_fd != -1)
#ifdef _WIN32
		shutdown(m_fd, SD_BOTH);
#else
		shutdown(m_fd, SHUT_RDWR);
#endif
}

int CPeerBase::Recv()
{
	if (m_ioError || m_fd == -1 || !m_inBuffer)
		return -1;

	const int remaining = MAX_INPUT_LEN - m_BytesRemain;
	if (remaining <= 0)
		return -1;

	if (!ReserveBufferSpace(m_inBuffer, std::min(remaining, static_cast<int>(DEFAULT_PACKET_BUFFER_SIZE)), MAX_INPUT_LEN))
		return -1;

	const int bytes_to_read = std::min(remaining, buffer_has_space(m_inBuffer));
	int bytes_read = socket_read(m_fd, (char *) buffer_write_peek(m_inBuffer), bytes_to_read);

	if (bytes_read < 0)
	{
		sys_err("socket_read failed %s", strerror(errno));
		return -1;
	}
	else if (bytes_read == 0)
		return 0;

	buffer_write_proceed(m_inBuffer, bytes_read);
	m_BytesRemain = buffer_size(m_inBuffer);
	return 1;
}

void CPeerBase::RecvEnd(int proceed_bytes)
{
	if (!m_inBuffer || proceed_bytes < 0 || proceed_bytes > m_BytesRemain)
	{
		FailIO();
		return;
	}
	buffer_read_proceed(m_inBuffer, proceed_bytes);
	m_BytesRemain = buffer_size(m_inBuffer);
}

int CPeerBase::GetRecvLength()
{
	return m_BytesRemain;
}

const void * CPeerBase::GetRecvBuffer()
{
	return m_inBuffer ? buffer_read_peek(m_inBuffer) : NULL;
}

int CPeerBase::GetSendLength()
{
	return m_outBuffer ? buffer_size(m_outBuffer) : 0;
}

int CPeerBase::Send()
{
	if (m_ioError || m_fd == -1 || !m_fdWatcher || !m_outBuffer)
		return -1;
	const int pending = GetSendLength();
	if (pending == 0)
		return 0;

	const int bytesToWrite = std::min(pending, 64 * 1024);
	const int written = socket_write_tcp(m_fd, static_cast<const char*>(buffer_read_peek(m_outBuffer)), bytesToWrite);
	if (written < 0 && errno != EINTR)
		return -1;
	if (written > 0)
		buffer_read_proceed(m_outBuffer, written);
	if (GetSendLength() != 0)
		fdwatch_add_fd(m_fdWatcher, m_fd, this, FDW_WRITE, true);
	return 0;
}
