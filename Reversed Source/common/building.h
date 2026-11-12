#ifndef MCNCC_COMMON_BUILDING_H
#define MCNCC_COMMON_BUILDING_H
#include "../libthecore/include/typedef.h"
#include <cstddef>
#include <cstdint>
namespace building {
typedef struct SLand {
    std::uint32_t dwID;
    std::int32_t lMapIndex;
    std::int32_t x, y;
    std::int32_t width, height;
    std::uint32_t dwGuildID;
    BYTE bGuildLevelLimit;
    std::uint32_t dwPrice;
} TLand;
static_assert(sizeof(TLand) == 36 && offsetof(TLand, dwPrice) == 32, "2009 building land record");

typedef struct SObjectMaterial {
    std::uint32_t dwItemVnum;
    std::uint32_t dwCount;
} TObjectMaterial;
static_assert(sizeof(TObjectMaterial) == 8, "2009 building record");

typedef struct SObjectProto {
    std::uint32_t dwVnum;
    std::uint32_t dwPrice;
    TObjectMaterial kMaterials[5];
    std::uint32_t dwUpgradeVnum;
    std::uint32_t dwUpgradeLimitTime;
    std::int32_t lLife;
    std::int32_t lRegion[4];
    std::uint32_t dwNPCVnum;
    std::int32_t lNPCX;
    std::int32_t lNPCY;
    std::uint32_t dwGroupVnum;
    std::uint32_t dwDependOnGroupVnum;
} TObjectProto;
static_assert(sizeof(TObjectProto) == 96, "2009 building record");

typedef struct SObject {
    std::uint32_t dwID;
    std::uint32_t dwLandID;
    std::uint32_t dwVnum;
    std::int32_t lMapIndex;
    std::int32_t x, y;
    float xRot, yRot, zRot;
    std::int32_t lLife;
} TObject;
static_assert(sizeof(TObject) == 40, "2009 building record");
}

#pragma pack(push, 1)
struct TPacketGDCreateObject
{
    std::uint32_t dwVnum;
    std::uint32_t dwLandID;
    std::int32_t lMapIndex;
    std::int32_t x;
    std::int32_t y;
    float xRot;
    float yRot;
    float zRot;
};
#pragma pack(pop)
static_assert(sizeof(TPacketGDCreateObject) == 32, "2009 DB object create request");
#endif
