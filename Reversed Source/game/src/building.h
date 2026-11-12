#ifndef MCNCC_GAME_BUILDING_H
#define MCNCC_GAME_BUILDING_H
#include "entity.h"
#include "../../common/building.h"
#include "../../common/singleton.h"
#include <map>
#include <vector>
class CHARACTER;
namespace building {
class CLand;
class CObject : public CEntity {
    friend class CLand;

public:
    CObject(TObject* data, TObjectProto* proto);
    ~CObject() override;
    void EncodeInsertPacket(LPENTITY entity) override;
    void EncodeRemovePacket(LPENTITY entity) override;
    DWORD GetID() const { return m_data.dwID; }
    DWORD GetVID() const { return m_dwVID; }
    void SetVID(DWORD vid);
    CLand* GetLand() { return m_pkLand; }
    bool Show(long map, long x, long y);
    void Destroy();
    void RegenNPC();
    void ApplySpecialEffect();
    void RemoveSpecialEffect();
    void Reconstruct(DWORD vnum);

private:
    void Save();

    TObjectProto* m_pProto;
    TObject m_data;
    DWORD m_dwVID;
    CLand* m_pkLand;
    ::CHARACTER* m_chNPC;
};

class CLand {
public:
    explicit CLand(TLand* data);
    ~CLand();
    void Destroy();
    CObject* FindObject(DWORD id);
    CObject* FindObjectByVnum(DWORD vnum);
    CObject* FindObjectByVID(DWORD vid);
    const TLand& GetData();
    void InsertObject(CObject* object);
    void PutData(TLand* data);
    void DeleteObject(DWORD id);
    void ClearLand();
    void SetOwner(DWORD guild);
    CObject* FindObjectByNPC(::CHARACTER* npc);
    CObject* FindObjectByGroup(DWORD group);
    void RequestDeleteObjectByVID(DWORD vid);
    bool RequestCreateWall(long map, float rotation);
    void RequestDeleteWall();
    bool RequestCreateWallBlocks(DWORD vnum, long map, char wallSize, bool east, bool west, bool south, bool north);
    void RequestDeleteWallBlocks(DWORD vnum);
    void RequestDeleteObject(DWORD id);
    bool RequestCreateObject(DWORD vnum, long map, long x, long y,
                             float xRot, float yRot, float zRot, bool checkAnother);

private:
    void RequestUpdate(DWORD guild);
    void DrawWall(DWORD vnum, long map, long& x, long& y, char length, float rotation);
    TLand m_data;
    std::map<DWORD, CObject*> m_map_pkObject;
    std::map<DWORD, CObject*> m_map_pkObjectByVID;
};
class CManager : public singleton<CManager> {
public:
    CManager();
    virtual ~CManager();
    void GracefulShutdown();
    void Destroy();
    void FinalizeBoot();
    void UpdateLand(TLand* table);
    void DeleteObject(DWORD id);
    void UnregisterObject(CObject* object);
    CObject* FindObjectByVID(DWORD vid);
    bool LoadLand(TLand* table);
    bool LoadObjectProto(TObjectProto* table, int size);
    bool LoadObject(TObject* table, bool boot);
    CLand* FindLand(DWORD id);
    CLand* FindLand(long map, long x, long y);
    void ClearLand(DWORD id);
    void ClearLandByGuildID(DWORD guild);
    CLand* FindLandByGuild(DWORD guild);
    void SendLandList(LPDESC desc, long map);
    TObjectProto* GetObjectProto(DWORD vnum);
private:
    std::vector<SObjectProto> m_vec_kObjectProto;
    std::map<DWORD, SObjectProto*> m_map_pkObjectProto;
    std::map<DWORD, CLand*> m_map_pkLand;
    std::map<DWORD, CObject*> m_map_pkObjByID;
    std::map<DWORD, CObject*> m_map_pkObjByVID;
};
}
#endif
