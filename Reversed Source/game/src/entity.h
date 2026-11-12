// Complete old data/virtual interface; lifecycle, view graph and basic accessors recovered.
#ifndef MCNCC_GAME_ENTITY_H
#define MCNCC_GAME_ENTITY_H
#include <map>
#include "typedef.h"
class CEntity {
public:
    typedef std::map<LPENTITY, int> ENTITY_MAP;
    CEntity();
    virtual ~CEntity();
    virtual void EncodeInsertPacket(LPENTITY entity) = 0;
    virtual void EncodeRemovePacket(LPENTITY entity) = 0;
protected:
    void Initialize(int type = -1);
    void Destroy();
public:
    void SetType(int type);
    int GetType();
    bool IsType(int type);
    void ViewCleanup();
    void ViewInsert(LPENTITY entity, bool recursive = true);
    void ViewRemove(LPENTITY entity, bool recursive = true);
    void ViewReencode();
    int GetViewAge();
    long GetX();
    long GetY();
    long GetZ();
    const PIXEL_POSITION& GetXYZ();
    void SetXYZ(long x, long y, long z);
    void SetXYZ(const PIXEL_POSITION& position);
    LPSECTREE GetSectree();
    void SetSectree(LPSECTREE tree);
    void UpdateSectree();
    void PacketAround(const void* data, int bytes, LPENTITY except = 0);
    void PacketView(const void* data, int bytes, LPENTITY except = 0);
    void BindDesc(LPDESC desc);
    LPDESC GetDesc();
    void SetMapIndex(long index);
    long GetMapIndex();
    void SetObserverMode(bool value);
    bool IsObserverMode();
protected:
    bool m_bIsObserver;
    bool m_bObserverModeChange;
    ENTITY_MAP m_map_view;
    long m_lMapIndex;
private:
    LPDESC m_lpDesc;
    int m_iType;
    bool m_bIsDestroyed;
    PIXEL_POSITION m_pos;
    int m_iViewAge;
    LPSECTREE m_pSectree;
};

// Basic inline field access, corroborated by caller instructions and old DWARF signatures.
inline int CEntity::GetViewAge() { return m_iViewAge; }
inline long CEntity::GetX() { return m_pos.x; }
inline long CEntity::GetY() { return m_pos.y; }
inline long CEntity::GetZ() { return m_pos.z; }
inline const PIXEL_POSITION& CEntity::GetXYZ() { return m_pos; }
inline void CEntity::SetXYZ(long x,long y,long z) { m_pos.x=x; m_pos.y=y; m_pos.z=z; }
inline void CEntity::SetXYZ(const PIXEL_POSITION& position) { m_pos=position; }
inline LPSECTREE CEntity::GetSectree() { return m_pSectree; }
inline void CEntity::SetSectree(LPSECTREE tree) { m_pSectree=tree; }
inline void CEntity::BindDesc(LPDESC desc) { m_lpDesc=desc; }
inline LPDESC CEntity::GetDesc() { return m_lpDesc; }
inline void CEntity::SetMapIndex(long index) { m_lMapIndex=index; }
inline long CEntity::GetMapIndex() { return m_lMapIndex; }
inline bool CEntity::IsObserverMode() { return m_bIsObserver; }
#endif
