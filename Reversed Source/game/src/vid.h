// Old two-word identity, backed by inline copy/reset/comparison and manager CRC checks.
#ifndef MCNCC_VID_H
#define MCNCC_VID_H
#include <cstdint>
#include "../../libthecore/include/typedef.h"
struct VID {
    VID();
    VID(DWORD id, DWORD crc);
    VID(const VID& other);
    const VID& operator=(const VID& other);
    bool operator==(const VID& other) const;
    bool operator!=(const VID& other) const;
    operator DWORD() const;
    void Reset();
private:
    std::uint32_t m_id;
    std::uint32_t m_crc;
};

inline VID::VID() : m_id(0),m_crc(0) {}
inline VID::VID(DWORD id,DWORD crc) : m_id(static_cast<std::uint32_t>(id)),m_crc(static_cast<std::uint32_t>(crc)) {}
inline VID::VID(const VID& other) : m_id(other.m_id),m_crc(other.m_crc) {}
inline const VID& VID::operator=(const VID& other) { m_id=other.m_id; m_crc=other.m_crc; return *this; }
inline bool VID::operator==(const VID& other) const { return m_id==other.m_id && m_crc==other.m_crc; }
inline bool VID::operator!=(const VID& other) const { return !(*this==other); }
inline VID::operator DWORD() const { return m_id; }
inline void VID::Reset() { m_id=0; m_crc=0; }
#endif
