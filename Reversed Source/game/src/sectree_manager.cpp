#include "config.h"
#include "constants.h"
#include "buffer_manager.h"
#include "char.h"
#include "desc.h"
#include <cstring>
#include <limits>
#include "dev_log.h"
#include "lzo_manager.h"
#include "packet.h"
#include "regen.h"
#include "../../libgame/include/attribute.h"
#include "../../libgame/include/targa.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <strings.h>
#include <utility>
#include "sectree.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include "sectree_manager.h"

WORD SECTREE_MANAGER::current_sectree_version = 0x0300;

// 080b7ce0
bool SECTREE_MANAGER::SaveAttributeToImage(int mapIndex, const char* filename, SECTREE_MAP* source)
{
    SECTREE_MAP* map = GetMap(mapIndex);
    if (!map)
        map = source;
    if (!map) {
        _sys_err("SaveAttributeToImage", 0x4ca, "cannot find sectree_map %ld", static_cast<long>(mapIndex));
        return false;
    }

    const int mapWidth = map->m_setting.iWidth / 25600;
    const int mapHeight = map->m_setting.iHeight / 25600;
    const int width = mapWidth * 512;
    const int height = mapHeight * 512;
    if (width < 0 || height < 0 || width > 65535 || height > 65535) {
        _sys_err("SaveAttributeToImage", 0x4d4, "map size error w %d h %d", mapWidth, mapHeight);
        return false;
    }

    sys_log(0, "SaveAttributeToImage w %d h %d file %s", mapWidth, mapHeight, filename);
    CTargaImage image;
    image.Create(width, height);
    sys_log(0, "1");
    unsigned char* pixel = reinterpret_cast<unsigned char*>(image.GetBasePointer());
    sys_log(0, "2 %p", static_cast<void*>(pixel));
    if (!pixel)
        return false;

    constexpr int cellsPerSector = 128;
    DWORD attributes[cellsPerSector];
    std::uint64_t written = 0;
    const int baseX = map->m_setting.iBaseX / 6400;
    const int baseY = map->m_setting.iBaseY / 6400;
    for (int y = 0; y < height / cellsPerSector; ++y) {
        for (int row = 0; row < cellsPerSector; ++row) {
            for (int x = 0; x < width / cellsPerSector; ++x) {
                SECTREEID id{};
                id.coord.x = baseX + x;
                id.coord.y = baseY + y;
                SECTREE* sector = map->Find(id.package);
                if (!sector) {
                    _sys_err("SaveAttributeToImage", 0x4f9, "cannot get sectree for %d %d %d %d",
                             static_cast<int>(id.coord.x), static_cast<int>(id.coord.y),
                             map->m_setting.iBaseX, map->m_setting.iBaseY);
                    continue;
                }
                CAttribute* attribute = sector->GetAttributePtr();
                if (!attribute)
                    continue;

                attribute->CopyRow(row, attributes);
                for (DWORD flags : attributes) {
                    const bool water = (flags & ATTR_WATER) != 0;
                    const bool banPk = !water && (flags & ATTR_BANPK) != 0;
                    const bool blocked = !water && !banPk && (flags & ATTR_BLOCK) != 0;
                    *pixel++ = banPk || blocked ? 0 : 255;
                    *pixel++ = water || blocked ? 0 : 255;
                    *pixel++ = water || banPk ? 0 : 255;
                    *pixel++ = 255;
                    ++written;
                }
            }
        }
    }

    sys_log(0, "3");
    if (!image.Save(filename)) {
        _sys_err("SaveAttributeToImage", 0x524, "cannot save file, map_index %ld filename %s",
                 static_cast<long>(mapIndex), filename);
        return false;
    }
    sys_log(0, "SECTREE: map %ld attribute saved to %s (%llu bytes)", static_cast<long>(mapIndex), filename,
            static_cast<unsigned long long>(written));
    return true;
}

SECTREE_MANAGER::SECTREE_MANAGER()
{
}

SECTREE_MANAGER::~SECTREE_MANAGER()
{
}

SECTREE_MAP* SECTREE_MANAGER::GetMap(long index)
{
    const auto found = m_map_pkSectree.find(static_cast<std::uint32_t>(index));
    return found == m_map_pkSectree.end() ? 0 : found->second;
}

LPSECTREE SECTREE_MAP::Find(DWORD package)
{
    const std::map<DWORD, SECTREE*>::iterator found = find(static_cast<std::uint32_t>(package));
    return found == end() ? 0 : found->second;
}

LPSECTREE SECTREE_MAP::Find(DWORD x, DWORD y)
{
    const unsigned int package = ((static_cast<unsigned int>(x) / 6400U) & 0xffffU) |
                                 ((static_cast<unsigned int>(y) / 6400U) << 16);
    return Find(static_cast<DWORD>(package));
}

LPSECTREE SECTREE_MANAGER::Get(DWORD index, DWORD package)
{
    SECTREE_MAP* map = GetMap(static_cast<long>(index));
    return map ? map->Find(package) : 0;
}

LPSECTREE SECTREE_MANAGER::Get(DWORD index, DWORD x, DWORD y)
{
    const unsigned int package = ((static_cast<unsigned int>(x) / 6400U) & 0xffffU) |
                                 ((static_cast<unsigned int>(y) / 6400U) << 16);
    return Get(index, static_cast<DWORD>(package));
}

bool SECTREE_MANAGER::IsMovablePosition(long index, long x, long y)
{
    LPSECTREE tree = SECTREE_MANAGER::instance().Get(index, x, y);
    return tree && !tree->IsAttr(x, y, ATTR_BLOCK | ATTR_OBJECT);
}

// 080b6170: search the original offsets in order, including the center.
bool SECTREE_MANAGER::GetMovablePosition(long index, long x, long y, PIXEL_POSITION& position)
{
    for (const Coord& offset : aArroundCoords) {
        const long candidateX = static_cast<std::int32_t>(static_cast<std::uint32_t>(x) + offset.x);
        const long candidateY = static_cast<std::int32_t>(static_cast<std::uint32_t>(y) + offset.y);
        LPSECTREE tree = SECTREE_MANAGER::instance().Get(index, candidateX, candidateY);
        if (tree && !tree->IsAttr(candidateX, candidateY, ATTR_BLOCK | ATTR_OBJECT)) {
            position.x = candidateX;
            position.y = candidateY;
            return true;
        }
    }
    position.x = x;
    position.y = y;
    return false;
}

bool SECTREE_MANAGER::GetMapBasePositionByMapIndex(long index, PIXEL_POSITION& position)
{
    if (index > 10000)
        index /= 10000;

    for (std::vector<SMapRegion>::iterator it = m_vec_mapRegion.begin();
         it != m_vec_mapRegion.end(); ++it)
    {
        if (index == it->index)
        {
            position.z = 0;
            position.x = it->sx;
            position.y = it->sy;
            return true;
        }
    }
    return false;
}

BYTE SECTREE_MANAGER::GetEmpireFromMapIndex(long index)
{
    const unsigned int mapIndex = static_cast<unsigned int>(index);
    if (mapIndex - 1U <= 19U)
        return 1;
    if (mapIndex - 21U <= 19U)
        return 2;
    if (mapIndex - 41U <= 19U)
        return 3;
    if (mapIndex - 184U <= 1U)
        return 1;
    if (mapIndex - 186U <= 1U)
        return 2;
    if (mapIndex - 188U <= 1U)
        return 3;
    return 0;
}

int SECTREE_MANAGER::GetMapIndex(long x, long y)
{
    for (std::vector<SMapRegion>::iterator it=m_vec_mapRegion.begin();it!=m_vec_mapRegion.end();++it) {
        if (x>=it->sx && y>=it->sy && x<it->ex && y<it->ey) return it->index;
    }
    sys_log(0,"SECTREE_MANAGER::GetMapIndex(%d, %d)",x,y);
    for (std::vector<SMapRegion>::iterator it=m_vec_mapRegion.begin();it!=m_vec_mapRegion.end();++it)
        sys_log(0,"%d: (%d, %d) ~ (%d, %d)",it->index,it->sx,it->sy,it->ex,it->ey);
    return 0;
}

SECTREE_MAP::~SECTREE_MAP()
{
    for (iterator it=begin();it!=end();++it)
        delete it->second;
    clear();
}

void SECTREE_MANAGER::DestroyPrivateMap(long index)
{
    if (index<=9999) return;
    SECTREE_MAP* map=GetMap(index);
    if (!map) return;
    sys_log(0,"PRIVATE_MAP: %d destroyed",index);
    m_map_pkSectree.erase(static_cast<std::uint32_t>(index));
    delete map;
}

SECTREE_MAP::SECTREE_MAP() {}

SECTREE_MAP::SECTREE_MAP(SECTREE_MAP& source)
    : m_setting(source.m_setting)
{
    for (iterator it=source.begin();it!=source.end();++it) {
        SECTREE* sector=new SECTREE;
        sector->m_id=it->second->m_id;
        sector->CloneAttribute(it->second);
        insert(std::make_pair(it->first,sector));
    }
    Build();
}

void SECTREE_MAP::Build()
{
    static const int offsets[8][2]={
        {-6400,0},{6400,0},{0,-6400},{0,6400},
        {-6400,6400},{6400,-6400},{-6400,-6400},{6400,6400}
    };
    for (iterator it=begin();it!=end();++it) {
        SECTREE* sector=it->second;
        sector->m_neighbor_list.push_back(sector);
        const SECTREEID id=sector->GetID();
        sys_log(3,"%dx%d",id.coord.x,id.coord.y);
        for (int i=0;i<8;++i) {
            SECTREE* neighbor=Find(id.coord.x*6400+offsets[i][0],id.coord.y*6400+offsets[i][1]);
            if (neighbor) {
                const SECTREEID neighborId=neighbor->GetID();
                sys_log(3,"   %d %dx%d",i,neighborId.coord.x,neighborId.coord.y);
                sector->m_neighbor_list.push_back(neighbor);
            }
        }
    }
}

long SECTREE_MANAGER::CreatePrivateMap(long index)
{
    if (index>9999) return 0;
    SECTREE_MAP* source=GetMap(index);
    if (!source) {
        _sys_err("CreatePrivateMap",0x368,"Cannot find map index %d",index);
        return 0;
    }
    long base=index*10000;
    int slot=0;
    while (slot<10000 && GetMap(base+slot)) ++slot;
    if (test_server) sys_log(0,"Create Dungeon : OrginalMapindex %d NewMapindex %d",index,slot);
    if (index>=107 && index<=109 && slot>(test_server ? 0 : 50)) return 0;
    if (slot==10000) {
        _sys_err("CreatePrivateMap",0x389,"not enough private map index (map_index %d)",index);
        return 0;
    }
    long privateIndex=base+slot;
    SECTREE_MAP* map=new SECTREE_MAP(*source);
    m_map_pkSectree.insert(std::make_pair(static_cast<std::uint32_t>(privateIndex),map));
    sys_log(0,"PRIVATE_MAP: %d created",privateIndex);
    sys_log(0,"Create Dungeon : OrginalMapindex %d NewMapindex %d",index,privateIndex);
    return privateIndex;
}

TAreaMap& SECTREE_MANAGER::GetDungeonArea(long map)
{
    std::map<int, TAreaMap>::iterator it = m_map_pkArea.find(map);
    if (it == m_map_pkArea.end())
        return m_map_pkArea[-1];
    return it->second;
}

npc_info::npc_info(BYTE type, const char* npcName, long posX, long posY)
    : bType(type), name(npcName), x(posX), y(posY)
{
}

void SECTREE_MANAGER::SendNPCPosition(CHARACTER* ch)
{
    DESC* desc = ch->GetDesc();
    if (!desc)
        return;

    const auto& positions = m_mapNPCPosition[static_cast<std::uint32_t>(ch->GetMapIndex())];
    if (positions.empty())
        return;
    const auto maxEntries = (std::numeric_limits<WORD>::max() - sizeof(TPacketGCNPCPosition)) / sizeof(TNPCPosition);
    if (positions.size() > maxEntries) {
        sys_err("SendNPCPosition: too many NPC positions on map %ld", ch->GetMapIndex());
        return;
    }

    TEMP_BUFFER buffer;
    for (const npc_info& npc : positions) {
        TNPCPosition entry = {npc.bType, {}, static_cast<std::int32_t>(npc.x), static_cast<std::int32_t>(npc.y)};
        std::strncpy(entry.name, npc.name, sizeof(entry.name) - 1);
        buffer.write(&entry, sizeof(entry));
    }
    const TPacketGCNPCPosition packet = {0x73, static_cast<WORD>(sizeof(packet) + buffer.size()),
                                        static_cast<WORD>(positions.size())};
    desc->BufferedPacket(&packet, sizeof(packet));
    desc->Packet(buffer.read_peek(), buffer.size());
}

void SECTREE_MANAGER::InsertNPCPosition(long map, BYTE type, const char* name, long x, long y)
{
    m_mapNPCPosition[static_cast<std::uint32_t>(map)].push_back(npc_info(type, name, x, y));
}

int SECTREE_MANAGER::LoadSettingFile(long map, const char* filename, TMapSetting& setting)
{
    setting = TMapSetting();
    FILE* file = std::fopen(filename, "r");
    if (!file) {
        _sys_err("LoadSettingFile", 0xb1, "cannot open file: %s", filename);
        return 0;
    }

    int width = 0, height = 0;
    char line[256];
    char command[256];
    while (std::fgets(line, sizeof(line), file)) {
        if (std::sscanf(line, "%255s", command) != 1)
            continue;
        if (!strcasecmp(command, "MapSize"))
            std::sscanf(line, "%*s %d %d", &width, &height);
        else if (!strcasecmp(command, "BasePosition"))
            std::sscanf(line, "%*s %d %d", &setting.iBaseX, &setting.iBaseY);
        else if (!strcasecmp(command, "CellScale"))
            std::sscanf(line, "%*s %d", &setting.iCellScale);
    }
    std::fclose(file);

    if ((!width && !height) || !setting.iCellScale) {
        _sys_err("LoadSettingFile", 0xce, "Invalid Settings file: %s", filename);
        return 0;
    }
    setting.iIndex = map;
    const auto sectorScale = static_cast<std::uint32_t>(setting.iCellScale) * 128U;
    setting.iWidth = static_cast<std::int32_t>(sectorScale * static_cast<std::uint32_t>(width));
    setting.iHeight = static_cast<std::int32_t>(sectorScale * static_cast<std::uint32_t>(height));
    return 1;
}

namespace {
std::int32_t AddMapCoordinate(std::int32_t base, std::int32_t offset)
{
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(base) + static_cast<std::uint32_t>(offset));
}
}

bool SECTREE_MANAGER::LoadMapRegion(const char* filename, TMapSetting& setting, const char* mapName)
{
    FILE* file = std::fopen(filename, "r");
    if (test_server)
        sys_log(0, "[LoadMapRegion] file(%s)", filename);
    if (!file)
        return false;

    int x = 0, y = 0;
    PIXEL_POSITION empire[3] = {};
    std::fscanf(file, " %d %d ", &x, &y);
    const int coordinates = std::fscanf(file, " %ld %ld %ld %ld %ld %ld ",
        &empire[0].x, &empire[0].y, &empire[1].x, &empire[1].y, &empire[2].x, &empire[2].y);
    std::fclose(file);

    for (int i = 0; i < 3; ++i)
        sys_log(0, "LoadMapRegion %d %d ", static_cast<int>(empire[i].x), static_cast<int>(empire[i].y));

    TMapRegion region = {};
    region.index = setting.iIndex;
    region.sx = setting.iBaseX;
    region.sy = setting.iBaseY;
    region.ex = AddMapCoordinate(setting.iBaseX, setting.iWidth);
    region.ey = AddMapCoordinate(setting.iBaseY, setting.iHeight);
    region.strMapName = mapName;
    const auto worldCoordinate = [](int base, long local) {
        return static_cast<std::int32_t>(
            static_cast<std::uint32_t>(base) + static_cast<std::uint32_t>(local) * 100U);
    };
    region.posSpawn.x = worldCoordinate(setting.iBaseX, x);
    region.posSpawn.y = worldCoordinate(setting.iBaseY, y);
    setting.posSpawn = region.posSpawn;
    sys_log(0, "LoadMapRegion %d x %d ~ %d y %d ~ %d, town %d %d", region.index,
            region.sx, region.ex, region.sy, region.ey, static_cast<int>(region.posSpawn.x),
            static_cast<int>(region.posSpawn.y));

    region.bEmpireSpawnDifferent = coordinates == 6;
    if (region.bEmpireSpawnDifferent) {
        for (int i = 0; i < 3; ++i) {
            region.posEmpire[i].x = worldCoordinate(setting.iBaseX, empire[i].x);
            region.posEmpire[i].y = worldCoordinate(setting.iBaseY, empire[i].y);
        }
    }
    m_vec_mapRegion.push_back(region);
    sys_log(0, "LoadMapRegion %d End", region.index);
    return true;
}

SECTREE_MAP* SECTREE_MANAGER::BuildSectreeFromSetting(TMapSetting& setting)
{
    const int sectorSize = 6400;
    const int endX = AddMapCoordinate(setting.iBaseX, setting.iWidth);
    const int endY = AddMapCoordinate(setting.iBaseY, setting.iHeight);
    SECTREE_MAP* map = new SECTREE_MAP;
    map->m_setting = setting;
    const auto insertSector = [map](int x, int y) {
        SECTREE* sector = new SECTREE;
        sector->m_id.coord.x = x;
        sector->m_id.coord.y = y;
        if (!map->insert(std::make_pair(sector->m_id.package, sector)).second)
            delete sector;
    };
    for (int x = setting.iBaseX; x < endX; x += sectorSize) {
        for (int y = setting.iBaseY; y < endY; y += sectorSize) {
            insertSector(x / sectorSize, y / sectorSize);
            sys_log(3, "new sectree %d x %d", static_cast<int>(static_cast<std::uint16_t>(x / sectorSize)),
                    static_cast<int>(static_cast<std::uint16_t>(y / sectorSize)));
        }
    }
    if (endX % sectorSize)
        insertSector(endX / sectorSize + 1, endY / sectorSize);
    if (endY % sectorSize)
        insertSector(endX / sectorSize, AddMapCoordinate(setting.iBaseX, setting.iHeight) / sectorSize + 1);
    return map;
}

void SECTREE_MANAGER::LoadDungeon(int map, const char* filename)
{
    FILE* file = std::fopen(filename, "r");
    if (!file)
        return;

    int count = 0;
    char line[1024];
    while (std::fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || (line[0] == '/' && line[1] == '/'))
            continue;
        std::istringstream input(line);
        std::string name;
        int x, y, sx, sy, dir;
        if (input >> name >> x >> y >> sx >> sy >> dir) {
            const auto centerX = static_cast<std::uint32_t>(x);
            const auto centerY = static_cast<std::uint32_t>(y);
            const auto radiusX = static_cast<std::uint32_t>(sx);
            const auto radiusY = static_cast<std::uint32_t>(sy);
            const TAreaInfo bounds(
                static_cast<std::int32_t>(centerX - radiusX),
                static_cast<std::int32_t>(centerY - radiusY),
                static_cast<std::int32_t>(centerX + radiusX),
                static_cast<std::int32_t>(centerY + radiusY), dir);
            m_map_pkArea[map].insert(std::make_pair(name, bounds));
            ++count;
        }
    }
    std::fclose(file);
    sys_log(0, "Dungeon Position Load [%3d]%s count %d", map, filename, count);
}

bool SECTREE_MANAGER::LoadAttribute(SECTREE_MAP* map, const char* filename, TMapSetting& setting)
{
    std::unique_ptr<FILE, decltype(&std::fclose)> file(std::fopen(filename, "rb"), std::fclose);
    if (!file) {
        _sys_err("LoadAttribute", 0x172, "SECTREE_MANAGER::LoadAttribute : cannot open %s", filename);
        return false;
    }

    std::int32_t width, height;
    if (std::fread(&width, sizeof(width), 1, file.get()) != 1 ||
        std::fread(&height, sizeof(height), 1, file.get()) != 1)
        return false;
    const unsigned side = 128;
    const unsigned cells = side * side;
    const unsigned bytes = cells * sizeof(std::uint32_t);
    LZOManager& lzo = LZOManager::instance();
    std::vector<BYTE> compressed(lzo.GetMaxCompressedSize(bytes));
    std::vector<std::uint32_t> decoded(cells);
    std::vector<DWORD> attributes(cells);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            SECTREEID id{};
            id.coord.x = setting.iBaseX / 6400 + x;
            id.coord.y = setting.iBaseY / 6400 + y;
            SECTREE* sector = map->Find(id.package);
            if (!sector) {
                _sys_err("LoadAttribute", 0x191,
                         "FATAL ERROR! LoadAttribute(%s) - cannot find sectree(package=%x, coord=(%u, %u), map_index=%u, map_base=(%u, %u))",
                         filename, static_cast<unsigned>(id.package), id.coord.x, id.coord.y,
                         setting.iIndex, setting.iBaseX, setting.iBaseY);
                _sys_err("LoadAttribute", 0x192, "ERROR_ATTR_POS(%d, %d) attr_size(%d, %d)", x, y, width, height);
                _sys_err("LoadAttribute", 0x193, "CHECK! 'Setting.txt' and 'server_attr' MAP_SIZE!!");
                for (SECTREE_MAP::iterator it = map->begin(); it != map->end(); ++it) {
                    const unsigned package = it->first;
                    _sys_err("LoadAttribute", 0x198, "SECTREE %x(%u, %u)", package, package & 0xffff, package >> 16);
                }
                std::abort();
            }
            if (sector->GetID().package != id.package) {
                _sys_err("LoadAttribute", 0x1a6, "returned tree id mismatch! return %lu, request %lu",
                         static_cast<unsigned long>(sector->GetID().package),
                         static_cast<unsigned long>(id.package));
                return false;
            }

            std::uint32_t size;
            if (std::fread(&size, sizeof(size), 1, file.get()) != 1 || size > compressed.size() ||
                std::fread(compressed.data(), 1, size, file.get()) != size)
                return false;
            unsigned outputSize = bytes;
            if (!lzo.Decompress(compressed.data(), size, reinterpret_cast<BYTE*>(decoded.data()), &outputSize) ||
                outputSize != bytes) {
                _sys_err("LoadAttribute", 0x1b7, "SECTREE_MANAGER::LoadAttribte : %s : %d %d size mismatch! %d",
                         filename, id.coord.x, id.coord.y, outputSize);
                return false;
            }
            std::copy(decoded.begin(), decoded.end(), attributes.begin());
            sector->BindAttribute(new CAttribute(attributes.data(), side, side));
        }
    }
    return true;
}

int SECTREE_MANAGER::Build(const char* indexFilename, const char* mapBase)
{
    if (test_server) {
        sys_log(0, "[BUILD] Build %s %s ", indexFilename, mapBase);
        dev_log("sectree_manager.cpp", 0x27f, "Build", 0x800, const_cast<char*>("[BUILD] Build %s %s "),
                indexFilename, mapBase);
    }
    std::unique_ptr<FILE, decltype(&std::fclose)> index(std::fopen(indexFilename, "r"), std::fclose);
    if (!index)
        return 0;

    char line[256];
    while (std::fgets(line, sizeof(line), index.get())) {
        if (line[0] == '#' || (line[0] == '/' && line[1] == '/'))
            continue;
        int mapIndex;
        char mapName[256];
        if (std::sscanf(line, "%d %255s", &mapIndex, mapName) != 2)
            continue;
        const std::string directory = std::string(mapBase) + "/" + mapName + "/";
        TMapSetting setting;
        if (!LoadSettingFile(mapIndex, (directory + "Setting.txt").c_str(), setting)) {
            _sys_err("Build", 0x29a, "can't load file %s in LoadSettingFile", (directory + "Setting.txt").c_str());
            return 0;
        }
        if (!LoadMapRegion((directory + "Town.txt").c_str(), setting, mapName)) {
            _sys_err("Build", 0x2a2, "can't load file %s in LoadMapRegion", (directory + "Town.txt").c_str());
            return 0;
        }
        if (test_server)
            sys_log(0, "[BUILD] Build %s %s %d ", mapBase, mapName, mapIndex);
        if (!map_allow_find(mapIndex))
            continue;

        SECTREE_MAP* map = BuildSectreeFromSetting(setting);
        m_map_pkSectree.insert(std::make_pair(static_cast<std::uint32_t>(mapIndex), map));
        LoadAttribute(map, (directory + "server_attr").c_str(), setting);
        const char* spawnFiles[] = { "regen.txt", "npc.txt", "boss.txt", "stone.txt" };
        for (unsigned i = 0; i < sizeof(spawnFiles) / sizeof(spawnFiles[0]); ++i)
            regen_load((directory + spawnFiles[i]).c_str(), setting.iIndex, setting.iBaseX, setting.iBaseY);
        LoadDungeon(mapIndex, (directory + "dungeon.txt").c_str());
        map->Build();
    }
    return 1;
}

TAreaInfo::TAreaInfo(int minX, int minY, int maxX, int maxY, int direction)
    : sx(minX), sy(minY), ex(maxX), ey(maxY), dir(direction)
{
}

bool SECTREE_MANAGER::GetRandomLocation(long index, PIXEL_POSITION& position,
                                       DWORD currentX, DWORD currentY, int maxDistance)
{
    SECTREE_MAP* map = GetMap(index);
    if (!map)
        return false;

    for (const TMapRegion& region : m_vec_mapRegion) {
        if (region.index != index)
            continue;

        const auto limitCoordinate = [maxDistance](std::uint32_t sample, std::uint32_t current) {
            if (maxDistance == 0)
                return sample;
            const auto difference = static_cast<std::int32_t>(current - sample);
            const auto distance = difference < 0
                ? static_cast<std::int32_t>(0U - static_cast<std::uint32_t>(difference)) : difference;
            if (maxDistance >= distance)
                return sample;
            const auto limit = static_cast<std::uint32_t>(maxDistance);
            return sample < current ? current - limit : current + limit;
        };
        for (int attempt = 0; attempt < 100; ++attempt) {
            const auto sampleX = static_cast<std::uint32_t>(number_ex(region.sx + 50, region.ex - 50,
                const_cast<char*>("sectree_manager.cpp"), 0x332));
            const auto sampleY = static_cast<std::uint32_t>(number_ex(region.sy + 50, region.ey - 50,
                const_cast<char*>("sectree_manager.cpp"), 0x333));
            const auto x = limitCoordinate(sampleX, static_cast<std::uint32_t>(currentX));
            const auto y = limitCoordinate(sampleY, static_cast<std::uint32_t>(currentY));
            const auto worldX = static_cast<std::int32_t>(x);
            const auto worldY = static_cast<std::int32_t>(y);

            LPSECTREE tree = map->Find(x, y);
            if (tree && !tree->IsAttr(worldX, worldY, ATTR_BLOCK | ATTR_OBJECT)) {
                position.x = worldX;
                position.y = worldY;
                return true;
            }
        }
    }
    return false;
}

const TMapRegion* SECTREE_MANAGER::FindRegionByPartialName(const char* name)
{
    for (const TMapRegion& region : m_vec_mapRegion)
        if (region.strMapName.find(name) != 0)
            return &region;
    return nullptr;
}

bool SECTREE_MANAGER::GetCenterPositionOfMap(BYTE map, PIXEL_POSITION& position)
{
    const TMapRegion* region = GetMapRegion(map);
    if (!region)
        return false;
    const auto midpoint = [](std::int32_t start, std::int32_t end) {
        const auto span = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(end) - static_cast<std::uint32_t>(start));
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(start) + span / 2);
    };
    position.x = midpoint(region->sx, region->ex);
    position.y = midpoint(region->sy, region->ey);
    position.z = 0;
    return true;
}

const TMapRegion* SECTREE_MANAGER::GetMapRegion(long index)
{
    for (const TMapRegion& region : m_vec_mapRegion) {
        if (region.index == index)
            return &region;
    }
    return 0;
}

bool SECTREE_MANAGER::GetSpawnPosition(long x, long y, PIXEL_POSITION& position)
{
    for (const TMapRegion& region : m_vec_mapRegion) {
        if (region.sx <= x && x < region.ex && region.sy <= y && y < region.ey) {
            position = region.posSpawn;
            return true;
        }
    }
    return false;
}

bool SECTREE_MANAGER::GetSpawnPositionByMapIndex(long index, PIXEL_POSITION& position)
{
    if (index > 10000)
        index /= 10000;
    const TMapRegion* region = GetMapRegion(index);
    if (!region)
        return false;
    position = region->posSpawn;
    return true;
}

bool SECTREE_MANAGER::GetRecallPositionByEmpire(int index, BYTE empire, PIXEL_POSITION& position)
{
    const TMapRegion* region = GetMapRegion(index);
    if (!region)
        return false;
    position = region->bEmpireSpawnDifferent && empire > 0 && empire <= 3
        ? region->posEmpire[empire - 1] : region->posSpawn;
    return true;
}

bool SECTREE_MANAGER::GetValidLocation(long index, long x, long y, long& validIndex, PIXEL_POSITION& position)
{
    SECTREE_MAP* map = GetMap(index);
    const long baseIndex = index > 9999 ? index / 10000 : index;
    if (!map) {
        if (index > 9999)
            return GetValidLocation(baseIndex, x, y, validIndex, position);
        _sys_err("GetValidLocation", 0x2f7, "cannot find sectree_map by map index %d", static_cast<int>(index));
        return false;
    }
    if (!GetMapRegion(baseIndex)) {
        _sys_err("GetValidLocation", 0x318, "invalid location (map index %d %d x %d)",
                 static_cast<int>(baseIndex), static_cast<int>(x), static_cast<int>(y));
        return false;
    }
    if (!map->Find(x, y)) {
        _sys_err("GetValidLocation", 0x30d, "cannot find tree by %d %d (map index %d)",
                 static_cast<int>(x), static_cast<int>(y), static_cast<int>(index));
        return false;
    }
    validIndex = index;
    position.x = x;
    position.y = y;
    return true;
}

bool SECTREE_MANAGER::ForAttrRegionCell(long map, long x, long y, DWORD attribute, EAttrRegionMode mode)
{
    const long cellsPerSector = 128;
    SECTREEID id = {};
    id.coord.x = x / cellsPerSector;
    id.coord.y = y / cellsPerSector;
    LPSECTREE sector = Get(map, id.package);
    if (!sector)
        return false;

    const long localX = x - id.coord.x * cellsPerSector;
    const long localY = y - id.coord.y * cellsPerSector;
    switch (mode) {
        case ATTR_REGION_MODE_SET:
            sector->SetAttribute(localX, localY, attribute);
            break;
        case ATTR_REGION_MODE_REMOVE:
            sector->RemoveAttribute(localX, localY, attribute);
            break;
        case ATTR_REGION_MODE_CHECK:
            return sector->IsAttr(x * 50, y * 50, ATTR_OBJECT);
        default:
            _sys_err("ForAttrRegionCell", 0x43a, "Unknown region mode %u", static_cast<unsigned>(mode));
            break;
    }
    return false;
}

bool SECTREE_MANAGER::ForAttrRegionRightAngle(long map, long x, long y, long width, long height,
                                             long rotation, DWORD attribute, EAttrRegionMode mode)
{
    const long turn = rotation / 90;
    if (turn == 1 || turn == 3)
        std::swap(width, height);
    else if (turn != 0 && turn != 2)
        return mode != ATTR_REGION_MODE_CHECK;

    for (long dx = 0; dx < width; ++dx) {
        for (long dy = 0; dy < height; ++dy) {
            if (ForAttrRegionCell(map, x + dx, y + dy, attribute, mode))
                return true;
        }
    }
    return mode != ATTR_REGION_MODE_CHECK;
}

bool SECTREE_MANAGER::ForAttrRegionFreeAngle(long map, long x, long y, long width, long height,
                                            long rotation, DWORD attribute, EAttrRegionMode mode)
{
    // The original keeps both operations in x87 until the sinf/cosf argument is stored.
    const float radians = static_cast<float>(static_cast<long double>(static_cast<std::int32_t>(rotation)) / 180.0L * 3.14f);
    const float sinAngle = std::sin(radians);
    const float cosAngle = std::cos(radians);
    const long halfWidth = width / 2;
    const long halfHeight = height / 2;
    const long double wideHalfHeight = halfHeight;
    const float widthSin = static_cast<float>(static_cast<long double>(halfWidth) * sinAngle);
    const float widthCos = static_cast<float>(static_cast<long double>(halfWidth) * cosAngle);
    // The original spills the width term before computing and adding the height term.
    const float x1 = static_cast<float>(-wideHalfHeight * cosAngle - widthSin);
    const float y1 = static_cast<float>(wideHalfHeight * sinAngle - widthCos);
    const float x2 = static_cast<float>(-wideHalfHeight * cosAngle + widthSin);
    const float y2 = static_cast<float>(wideHalfHeight * sinAngle + widthCos);
    const float x3 = static_cast<float>(wideHalfHeight * cosAngle - widthSin);
    const float y3 = static_cast<float>(-wideHalfHeight * sinAngle - widthCos);
    const float x4 = static_cast<float>(wideHalfHeight * cosAngle + widthSin);
    const long double y4 = widthCos - wideHalfHeight * sinAngle;

    const long double dx1 = static_cast<long double>(x2) - x1;
    const long double dx2 = static_cast<long double>(x1) - x3;
    if (dx1 == 0.0f || dx2 == 0.0f) {
        _sys_err("ForAttrRegionFreeAngle", 0x471,
                 "SECTREE_MANAGER::ForAttrRegion - Unhandled exception. MapIndex: %d",
                 static_cast<int>(map));
        return false;
    }

    long double slope1 = (static_cast<long double>(y2) - y1) / dx1;
    const long double wideSlope2 = (static_cast<long double>(y1) - y3) / dx2;
    const float slope2 = static_cast<float>(wideSlope2);
    const float intercept1 = static_cast<float>(y1 - slope1 * x1);
    const float intercept2 = static_cast<float>(y1 - wideSlope2 * x1);
    const float intercept3 = static_cast<float>(y4 - slope1 * x4);
    long double intercept4 = y4 - wideSlope2 * x4;

    const int beginX = static_cast<int>(std::min(std::min(x1, x2), std::min(x3, x4)));
    const int endX = static_cast<int>(std::max(std::max(x1, x2), std::max(x3, x4)));
    for (int dx = beginX; dx < endX; ++dx) {
        const long double offset1 = slope1 * dx;
        const long double offset2 = static_cast<long double>(slope2) * dx;
        const long double low1 = offset1 + std::min(intercept1, intercept3);
        const long double low2 = offset2 + std::min<long double>(intercept2, intercept4);
        const long double high1 = offset1 + std::max(intercept1, intercept3);
        const long double high2 = offset2 + std::max<long double>(intercept2, intercept4);
        const long double a = std::abs(static_cast<int>(low1)) < std::abs(static_cast<int>(low2)) ? low1 : low2;
        const long double b = std::abs(static_cast<int>(high1)) < std::abs(static_cast<int>(high2)) ? high1 : high2;

        const int beginY = static_cast<int>(std::min(a, b));
        const long double upper = std::max(a, b);
        int endY = static_cast<int>(upper);
        for (int dy = beginY; dy < endY; ++dy) {
            if (ForAttrRegionCell(map, x + halfWidth + dx, y + halfHeight + dy, attribute, mode))
                return true;
            // The original spills these x87 registers to float across the cell call.
            endY = static_cast<int>(static_cast<float>(upper));
            slope1 = static_cast<float>(slope1);
            intercept4 = static_cast<float>(intercept4);
        }
    }
    return mode != ATTR_REGION_MODE_CHECK;
}

bool SECTREE_MANAGER::ForAttrRegion(long map, long startX, long startY, long endX, long endY,
                                   long rotation, DWORD attribute, EAttrRegionMode mode)
{
    if (!GetMap(map)) {
        _sys_err("ForAttrRegion", 0x4a0, "Cannot find SECTREE_MAP by map index %ld", map);
        return mode == ATTR_REGION_MODE_CHECK;
    }

    constexpr std::int32_t cellSize = 50;
    const auto alignStart = [=](long value) {
        const auto coordinate = static_cast<std::int32_t>(value);
        return coordinate - coordinate % cellSize;
    };
    const auto alignEnd = [=](long value) {
        const auto coordinate = static_cast<std::int32_t>(value);
        return static_cast<std::int32_t>(
            static_cast<std::uint32_t>(coordinate) + static_cast<std::uint32_t>(coordinate % cellSize));
    };
    const auto cellSpan = [=](std::int32_t start, std::int32_t end) {
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(end) - static_cast<std::uint32_t>(start)) / cellSize;
    };
    const auto alignedStartX = alignStart(startX);
    const auto alignedStartY = alignStart(startY);
    const auto alignedEndX = alignEnd(endX);
    const auto alignedEndY = alignEnd(endY);
    const auto x = alignedStartX / cellSize;
    const auto y = alignedStartY / cellSize;
    const auto width = cellSpan(alignedStartX, alignedEndX);
    const auto height = cellSpan(alignedStartY, alignedEndY);
    sys_log(0, "ForAttrRegion %d %d ~ %d %d", alignedStartX, alignedStartY, alignedEndX, alignedEndY);

    rotation = static_cast<std::int32_t>(rotation) % 360;
    if (rotation % 90 == 0)
        return ForAttrRegionRightAngle(map, x, y, width, height, rotation, attribute, mode);
    return ForAttrRegionFreeAngle(map, x, y, width, height, rotation, attribute, mode);
}

bool SECTREE_MANAGER::GetMapBasePosition(long x, long y, PIXEL_POSITION& position)
{
    for (const TMapRegion& region : m_vec_mapRegion) {
        if (x >= region.sx && y >= region.sy && x < region.ex && y < region.ey) {
            position.x = region.sx;
            position.y = region.sy;
            position.z = 0;
            return true;
        }
    }
    return false;
}
