// Full original spatial-manager DATA and API recovered from target DWARF.
// Map loading and owning SECTREE_MAP destruction remain pending.
#ifndef MCNCC_GAME_SECTREE_MANAGER_H
#define MCNCC_GAME_SECTREE_MANAGER_H
#include "sectree.h"
#include "../../common/singleton.h"
#include <map>
#include <vector>
#include <string>
class CHARACTER;
struct SMapRegion {
    int index,sx,sy,ex,ey;
    PIXEL_POSITION posSpawn;
    bool bEmpireSpawnDifferent;
    PIXEL_POSITION posEmpire[3];
    std::string strMapName;
};
typedef SMapRegion TMapRegion;
struct TAreaInfo {
    int sx,sy,ex,ey,dir;
    TAreaInfo(int,int,int,int,int);
};
struct npc_info {
    BYTE bType;
    const char* name;
    long x,y;
    npc_info(BYTE,const char*,long,long);
};
typedef std::map<std::string,TAreaInfo> TAreaMap;
struct SSetting {
    int iIndex,iCellScale,iBaseX,iBaseY,iWidth,iHeight;
    PIXEL_POSITION posSpawn;
};
typedef SSetting TMapSetting;
enum EAttrRegionMode { ATTR_REGION_MODE_SET=0, ATTR_REGION_MODE_REMOVE=1, ATTR_REGION_MODE_CHECK=2 };
class SECTREE_MAP;
typedef SECTREE_MAP* LPSECTREE_MAP;
class SECTREE_MAP:public std::map<DWORD,SECTREE*> {
public:
    SECTREE_MAP();
    SECTREE_MAP(SECTREE_MAP&);
    virtual ~SECTREE_MAP();
    LPSECTREE Find(long unsigned int);
    LPSECTREE Find(long unsigned int,long unsigned int);
    void Build();
    TMapSetting m_setting;
};
class SECTREE_MANAGER:public singleton<SECTREE_MANAGER> {
public:
    SECTREE_MANAGER();
    virtual ~SECTREE_MANAGER();
    SECTREE_MAP* GetMap(long int);
    LPSECTREE Get(long unsigned int,long unsigned int);
    LPSECTREE Get(long unsigned int,long unsigned int,long unsigned int);
    int LoadSettingFile(long int,const char*,TMapSetting&);
    bool LoadMapRegion(const char*,TMapSetting&,const char*);
    int Build(const char*,const char*);
    SECTREE_MAP* BuildSectreeFromSetting(TMapSetting&);
    bool LoadAttribute(SECTREE_MAP*,const char*,TMapSetting&);
    void LoadDungeon(int,const char*);
    bool GetValidLocation(long int,long int,long int,long int&,PIXEL_POSITION&);
    bool GetSpawnPosition(long int,long int,PIXEL_POSITION&);
    bool GetSpawnPositionByMapIndex(long int,PIXEL_POSITION&);
    bool GetRecallPositionByEmpire(int,unsigned char,PIXEL_POSITION&);
    const TMapRegion* GetMapRegion(long int);
    int GetMapIndex(long int,long int);
    const TMapRegion* FindRegionByPartialName(const char*);
    bool GetMapBasePosition(long int,long int,PIXEL_POSITION&);
    bool GetMapBasePositionByMapIndex(long int,PIXEL_POSITION&);
    bool GetMovablePosition(long int,long int,long int,PIXEL_POSITION&);
    bool IsMovablePosition(long int,long int,long int);
    bool GetCenterPositionOfMap(unsigned char,PIXEL_POSITION&);
    bool GetRandomLocation(long int,PIXEL_POSITION&,long unsigned int,long unsigned int,int);
    long int CreatePrivateMap(long int);
    void DestroyPrivateMap(long int);
    TAreaMap& GetDungeonArea(long int);
    void SendNPCPosition(CHARACTER*);
    void InsertNPCPosition(long int,unsigned char,const char*,long int,long int);
    BYTE GetEmpireFromMapIndex(long int);
    bool ForAttrRegion(long int,long int,long int,long int,long int,long int,long unsigned int,EAttrRegionMode);
    bool SaveAttributeToImage(int,const char*,SECTREE_MAP*);
private:
    bool ForAttrRegionRightAngle(long int,long int,long int,long int,long int,long int,long unsigned int,EAttrRegionMode);
    bool ForAttrRegionFreeAngle(long int,long int,long int,long int,long int,long int,long unsigned int,EAttrRegionMode);
    bool ForAttrRegionCell(long int,long int,long int,long unsigned int,EAttrRegionMode);
    std::map<DWORD,SECTREE_MAP*> m_map_pkSectree;
    std::map<int,std::map<std::string,TAreaInfo> > m_map_pkArea;
    std::vector<SMapRegion> m_vec_mapRegion;
    std::map<DWORD,std::vector<npc_info> > m_mapNPCPosition;
};
#endif
