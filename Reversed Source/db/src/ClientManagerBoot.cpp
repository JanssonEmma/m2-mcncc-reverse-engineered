#include "stdafx.h"
#include "NumericInput.h"
#include "SqlFormat.h"
#include "SqlInput.h"
#include "ClientManager.h"
#include "DBManager.h"
#include "Main.h"
#include "../../common/building.h"

#include <memory>
#include <cctype>
#include <utility>

extern std::string g_stLocaleNameColumn;

namespace
{
template <std::size_t Size>
void CopyBootText(char (&destination)[Size], const char* source)
{
    strncpy(destination, source ? source : "", Size - 1);
    destination[Size - 1] = '\0';
}
}

bool CClientManager::InitializeTables()
{
	if (!InitializeMobTable())
	{
		sys_err("InitializeMobTable FAILED");
		return false;
	}

	if (!InitializeItemTable())
	{
		sys_err("InitializeItemTable FAILED");
		return false;
	}

	if (!InitializeShopTable())
	{
		sys_err("InitializeShopTable FAILED");
		return false;
	}

	if (!InitializeSkillTable())
	{
		sys_err("InitializeSkillTable FAILED");
		return false;
	}

	if (!InitializeRefineTable())
	{
		sys_err("InitializeRefineTable FAILED");
		return false;
	}

	if (!InitializeItemAttrTable())
	{
		sys_err("InitializeItemAttrTable FAILED");
		return false;
	}

	if (!InitializeItemRareTable())
	{
		sys_err("InitializeItemRareTable FAILED");
		return false;
	}

	if (!InitializeBanwordTable())
	{
		sys_err("InitializeBanwordTable FAILED");
		return false;
	}

	if (!InitializeLandTable())
	{
		sys_err("InitializeLandTable FAILED");
		return false;
	}

	if (!InitializeObjectProto())
	{
		sys_err("InitializeObjectProto FAILED");
		return false;
	}

	if (!InitializeObjectTable())
	{
		sys_err("InitializeObjectTable FAILED");
		return false;
	}

	return true;
}

bool CClientManager::InitializeRefineTable()
{
	char query[2048];

	dbsql::snprintf(query, sizeof(query),
			"SELECT id, cost, prob, vnum0, count0, vnum1, count1, vnum2, count2,  vnum3, count3, vnum4, count4 FROM refine_proto%s",
			GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->iNumRows)
		return true;

	if (m_pRefineTable)
	{
		delete [] m_pRefineTable;
		m_pRefineTable = NULL;
	}

	m_iRefineTableSize = pRes->iNumRows;

	m_pRefineTable	= new TRefineTable[m_iRefineTableSize];
	memset(m_pRefineTable, 0, sizeof(TRefineTable) * m_iRefineTableSize);

	TRefineTable* prt = m_pRefineTable;
	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{

		int col = 0;
		prt->id = atoi(data[col++]);
		prt->cost = atoi(data[col++]);
		prt->prob = atoi(data[col++]);

		for (int i = 0; i < static_cast<int>(sizeof(prt->materials) / sizeof(prt->materials[0])); i++)
		{
			prt->materials[i].vnum = atoi(data[col++]);
			prt->materials[i].count = atoi(data[col++]);
			if (prt->materials[i].vnum == 0)
			{
				prt->material_count = static_cast<BYTE>(i);
				break;
			}
		}

		prt++;
	}
	return true;
}

bool CClientManager::InitializeShopTable()
{
	MYSQL_ROW	data;
	int		col;

	static const char * s_szQuery =
		"SELECT "
		"shop.vnum, "
		"shop.npc_vnum, "
		"shop_item.item_vnum, "
		"shop_item.count "
		"FROM shop LEFT JOIN shop_item "
		"ON shop.vnum = shop_item.shop_vnum ORDER BY shop.vnum, shop_item.item_vnum";

	std::unique_ptr<SQLMsg> pkMsg2(CDBManager::instance().DirectQuery(s_szQuery));

	SQLResult * pRes2 = pkMsg2->Get();

	if (!pRes2->iNumRows)
	{
		sys_err("InitializeShopTable : Table count is zero.");
		return false;
	}

	std::map<int, TShopTable *> map_shop;

	if (m_pShopTable)
	{
		delete [] (m_pShopTable);
		m_pShopTable = NULL;
	}

	TShopTable * shop_table = m_pShopTable;

	while ((data = mysql_fetch_row(pRes2->pSQLResult)))
	{
		col = 0;

		int iShopVnum = 0;
		iShopVnum = atoi(data[col++]);

		if (map_shop.end() == map_shop.find(iShopVnum))
		{
			shop_table = new TShopTable;
			memset(shop_table, 0, sizeof(TShopTable));
			shop_table->dwVnum	= iShopVnum;

			map_shop[iShopVnum] = shop_table;
		}
		else
			shop_table = map_shop[iShopVnum];

		shop_table->dwNPCVnum = atoi(data[col++]);

		if (!data[col])
			continue;

		TShopItemTable * pItem = &shop_table->items[shop_table->byItemCount];

		pItem->vnum = atoi(data[col++]);
		pItem->count = dbinput::ReadInteger<decltype(pItem->count)>(data[col++]);

		++shop_table->byItemCount;
	}

	m_pShopTable = new TShopTable[map_shop.size()];
	m_iShopTableSize = map_shop.size();

	std::map<int, TShopTable*>::iterator it = map_shop.begin();

	int i = 0;

	while (it != map_shop.end())
	{
		thecore_memcpy((m_pShopTable + i), (it++)->second, sizeof(TShopTable));
		++i;
	}

	return true;
}

namespace
{
struct FCompareVnum
{
	bool operator()(const TItemTable& left, const TItemTable& right) const
	{
		return left.dwVnum < right.dwVnum;
	}
};
}

bool CClientManager::InitializeQuestItemTable()
{
	if (!dbsql::IsIdentifier(g_stLocaleNameColumn))
	{
		sys_err("Invalid locale column name");
		return false;
	}
	using namespace std;

	static const char * s_szQuery = "SELECT vnum, name, `%s` FROM quest_item_proto ORDER BY vnum";

	char query[1024];
	dbsql::snprintf(query, sizeof(query), s_szQuery, g_stLocaleNameColumn.c_str());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->iNumRows)
	{
		sys_err("query error or no rows: %s", query);
		return false;
	}

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;

		TItemTable tbl;
		memset(&tbl, 0, sizeof(tbl));

		tbl.dwVnum = atoi(row[col++]);

		if (row[col])
			strncpy(tbl.szName, row[col], sizeof(tbl.szName) - 1);

		col++;

		if (row[col])
			strncpy(tbl.szLocaleName, row[col], sizeof(tbl.szLocaleName) - 1);

		col++;

		if (m_map_itemTableByVnum.find(tbl.dwVnum) != m_map_itemTableByVnum.end())
		{
			sys_err("QUEST_ITEM_ERROR! %lu vnum already exist! (name %s)", static_cast<unsigned long>(tbl.dwVnum), tbl.szLocaleName);
			continue;
		}

		tbl.bType = ITEM_QUEST;
		tbl.bSize = 1;

		m_vec_itemTable.push_back(tbl);
	}

	std::sort(m_vec_itemTable.begin(), m_vec_itemTable.end(), FCompareVnum());
	return true;
}

bool CClientManager::InitializeSkillTable()
{
	static const char* s_szQuery =
		"SELECT dwVnum, szName, bType, bMaxLevel, dwSplashRange, "
		"szPointOn, szPointPoly, szSPCostPoly, szDurationPoly, szDurationSPCostPoly, "
		"szCooldownPoly, szMasterBonusPoly, setFlag+0, setAffectFlag+0, "
		"szPointOn2, szPointPoly2, szDurationPoly2, setAffectFlag2+0, "
		"szPointOn3, szPointPoly3, szDurationPoly3, szGrandMasterAddSPCostPoly, "
		"bLevelStep, bLevelLimit, prerequisiteSkillVnum, prerequisiteSkillLevel, iMaxHit, szSplashAroundDamageAdjustPoly, eSkillType+0, dwTargetRange "
		"FROM skill_proto%s ORDER BY dwVnum";
	char query[4096];
	dbsql::snprintf(query, sizeof(query), s_szQuery, GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->iNumRows)
	{
		sys_err("no result: %s", s_szQuery);
		return false;
	}

	if (!m_vec_skillTable.empty())
		m_vec_skillTable.clear();

	m_vec_skillTable.reserve(pRes->iNumRows);

	MYSQL_ROW	data;
	int		col;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TSkillTable t;
		memset(&t, 0, sizeof(t));

		col = 0;

		t.dwVnum = atoi(data[col++]);
		CopyBootText(t.szName, data[col++]);
		t.bType = dbinput::ReadInteger<decltype(t.bType)>(data[col++]);
		t.bMaxLevel = dbinput::ReadInteger<decltype(t.bMaxLevel)>(data[col++]);
		t.dwSplashRange = atoi(data[col++]);

		CopyBootText(t.szPointOn, data[col++]);
		CopyBootText(t.szPointPoly, data[col++]);
		CopyBootText(t.szSPCostPoly, data[col++]);
		CopyBootText(t.szDurationPoly, data[col++]);
		CopyBootText(t.szDurationSPCostPoly, data[col++]);
		CopyBootText(t.szCooldownPoly, data[col++]);
		CopyBootText(t.szMasterBonusPoly, data[col++]);

		t.dwFlag = atoi(data[col++]);
		t.dwAffectFlag = atoi(data[col++]);

		CopyBootText(t.szPointOn2, data[col++]);
		CopyBootText(t.szPointPoly2, data[col++]);
		CopyBootText(t.szDurationPoly2, data[col++]);
		t.dwAffectFlag2 = atoi(data[col++]);

		CopyBootText(t.szPointOn3, data[col++]);
		CopyBootText(t.szPointPoly3, data[col++]);
		CopyBootText(t.szDurationPoly3, data[col++]);

		CopyBootText(t.szGrandMasterAddSPCostPoly, data[col++]);

		t.bLevelStep = dbinput::ReadInteger<decltype(t.bLevelStep)>(data[col++]);
		t.bLevelLimit = dbinput::ReadInteger<decltype(t.bLevelLimit)>(data[col++]);
		t.preSkillVnum = atoi(data[col++]);
		t.preSkillLevel = dbinput::ReadInteger<decltype(t.preSkillLevel)>(data[col++]);

		t.lMaxHit = atoi(data[col++]);

		CopyBootText(t.szSplashAroundDamageAdjustPoly, data[col++]);

		t.bSkillAttrType = dbinput::ReadInteger<decltype(t.bSkillAttrType)>(data[col++]);
		t.dwTargetRange = atoi(data[col++]);

		m_vec_skillTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeBanwordTable()
{
	m_vec_banwordTable.clear();

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery("SELECT word FROM banword"));

	SQLResult * pRes = pkMsg->Get();

	if (pRes->iNumRows <= 0)
		return true;

	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TBanwordTable t;

		if (data[0])
		{
			strncpy(t.szWord, data[0], sizeof(t.szWord) - 1);
			t.szWord[sizeof(t.szWord) - 1] = '\0';
			m_vec_banwordTable.push_back(t);
		}
	}

	return true;
}

bool CClientManager::InitializeItemAttrTable()
{
	static const char* s_szQuery = "SELECT apply, apply+0, prob, lv1, lv2, lv3, lv4, lv5, weapon, body, wrist, foots, neck, head, shield, ear FROM item_attr%s ORDER BY apply";
	char query[4096];
	dbsql::snprintf(query, sizeof(query), s_szQuery, GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->iNumRows)
	{
		sys_err("no result: %s", s_szQuery);
		return false;
	}

	if (!m_vec_itemAttrTable.empty())
		m_vec_itemAttrTable.clear();

	m_vec_itemAttrTable.reserve(pRes->iNumRows);

	MYSQL_ROW	data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TItemAttrTable t;

		memset(&t, 0, sizeof(TItemAttrTable));

		int col = 0;

		strncpy(t.szApply, data[col++], sizeof(t.szApply) - 1);
		t.szApply[sizeof(t.szApply) - 1] = '\0';
		t.dwApplyIndex = atoi(data[col++]);
		t.dwProb = atoi(data[col++]);
		t.lValues[0] = atoi(data[col++]);
		t.lValues[1] = atoi(data[col++]);
		t.lValues[2] = atoi(data[col++]);
		t.lValues[3] = atoi(data[col++]);
		t.lValues[4] = atoi(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_BODY] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_BODY])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_NECK] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_NECK])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_EAR] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_EAR])>>(data[col++]);

		m_vec_itemAttrTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeItemRareTable()
{
	static const char* s_szQuery = "SELECT apply, apply+0, prob, lv1, lv2, lv3, lv4, lv5, weapon, body, wrist, foots, neck, head, shield, ear FROM item_attr_rare%s ORDER BY apply";
	char query[4096];
	dbsql::snprintf(query, sizeof(query), s_szQuery, GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->iNumRows)
	{
		sys_err("no result: %s", s_szQuery);
		return false;
	}

	if (!m_vec_itemRareTable.empty())
		m_vec_itemRareTable.clear();

	m_vec_itemRareTable.reserve(pRes->iNumRows);

	MYSQL_ROW	data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TItemAttrTable t;

		memset(&t, 0, sizeof(TItemAttrTable));

		int col = 0;

		strncpy(t.szApply, data[col++], sizeof(t.szApply) - 1);
		t.szApply[sizeof(t.szApply) - 1] = '\0';
		t.dwApplyIndex = atoi(data[col++]);
		t.dwProb = atoi(data[col++]);
		t.lValues[0] = atoi(data[col++]);
		t.lValues[1] = atoi(data[col++]);
		t.lValues[2] = atoi(data[col++]);
		t.lValues[3] = atoi(data[col++]);
		t.lValues[4] = atoi(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_BODY] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_BODY])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_NECK] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_NECK])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD])>>(data[col++]);
		t.bMaxLevelBySet[ATTRIBUTE_SET_EAR] = dbinput::ReadInteger<std::remove_reference_t<decltype(t.bMaxLevelBySet[ATTRIBUTE_SET_EAR])>>(data[col++]);

		m_vec_itemRareTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeLandTable()
{
	using namespace building;

	char query[4096];

	dbsql::snprintf(query, sizeof(query),
		"SELECT id, map_index, x, y, width, height, guild_id, guild_level_limit, price "
		"FROM land%s WHERE enable='YES' ORDER BY id",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_vec_kLandTable.empty())
		m_vec_kLandTable.clear();

	m_vec_kLandTable.reserve(pRes->iNumRows);

	MYSQL_ROW	data;

	if (pRes->iNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TLand t;

			memset(&t, 0, sizeof(t));

			int col = 0;

			t.dwID = atoi(data[col++]);
			t.lMapIndex = atoi(data[col++]);
			t.x = atoi(data[col++]);
			t.y = atoi(data[col++]);
			t.width = atoi(data[col++]);
			t.height = atoi(data[col++]);
			t.dwGuildID = atoi(data[col++]);
			t.bGuildLevelLimit = dbinput::ReadInteger<decltype(t.bGuildLevelLimit)>(data[col++]);
			t.dwPrice = atoi(data[col++]);

			m_vec_kLandTable.push_back(t);
		}

	return true;
}

void parse_pair_number_string(const char* text, std::vector<std::pair<int, int> >& pairs)
{
	const char* separator = strchr(text, '/');
	char number[33];

	while (separator)
	{
		if ((*text & 0xe0) <= 0x90 && isdigit(static_cast<unsigned char>(*text)))
		{
			int length = MIN(32, static_cast<int>(separator - text));
			strncpy(number, text, length);
			number[length] = '\0';

			char* comma = strchr(number, ',');
			int count = 0;
			if (comma)
			{
				*comma = '\0';
				count = atoi(comma + 1);
			}
			pairs.push_back(std::make_pair(atoi(number), count));
		}

		text = separator + 1;
		separator = strchr(text, '/');
	}

	if ((*text & 0xe0) <= 0x90 && isdigit(static_cast<unsigned char>(*text)))
	{
		strncpy(number, text, sizeof(number) - 1);
		number[sizeof(number) - 1] = '\0';

		char* comma = strchr(const_cast<char*>(text), ',');
		int count = 0;
		if (comma)
		{
			*comma = '\0';
			count = atoi(comma + 1);
		}
		pairs.push_back(std::make_pair(atoi(number), count));
	}
}

bool CClientManager::InitializeObjectProto()
{
	using namespace building;

	char query[4096];
	dbsql::snprintf(query, sizeof(query),
			"SELECT vnum, price, materials, upgrade_vnum, upgrade_limit_time, life, reg_1, reg_2, reg_3, reg_4, npc, group_vnum, dependent_group "
			"FROM object_proto%s ORDER BY vnum",
			GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_vec_kObjectProto.empty())
		m_vec_kObjectProto.clear();

	m_vec_kObjectProto.reserve(MAX(0, pRes->iNumRows));

	MYSQL_ROW	data;

	if (pRes->iNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TObjectProto t;

			memset(&t, 0, sizeof(t));

			int col = 0;

			t.dwVnum = atoi(data[col++]);
			t.dwPrice = atoi(data[col++]);

			std::vector<std::pair<int, int> > vec;
			parse_pair_number_string(data[col++], vec);

			for (unsigned int i = 0; i < sizeof(t.kMaterials) / sizeof(t.kMaterials[0]) && i < vec.size(); ++i)
			{
				std::pair<int, int> & r = vec[i];

				t.kMaterials[i].dwItemVnum = r.first;
				t.kMaterials[i].dwCount = r.second;
			}

			t.dwUpgradeVnum = atoi(data[col++]);
			t.dwUpgradeLimitTime = atoi(data[col++]);
			t.lLife = atoi(data[col++]);
			t.lRegion[0] = atoi(data[col++]);
			t.lRegion[1] = atoi(data[col++]);
			t.lRegion[2] = atoi(data[col++]);
			t.lRegion[3] = atoi(data[col++]);

			t.dwNPCVnum = atoi(data[col++]);
			t.dwGroupVnum = atoi(data[col++]);
			t.dwDependOnGroupVnum = atoi(data[col++]);

			t.lNPCX = 0;
			t.lNPCY = MAX(t.lRegion[1], t.lRegion[3])+300;

			m_vec_kObjectProto.push_back(t);
		}

	return true;
}

bool CClientManager::InitializeObjectTable()
{
	using namespace building;

	char query[4096];
	dbsql::snprintf(query, sizeof(query), "SELECT id, land_id, vnum, map_index, x, y, x_rot, y_rot, z_rot, life FROM object%s ORDER BY id", GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_map_pkObjectTable.empty())
		m_map_pkObjectTable.clear();

	MYSQL_ROW data;

	if (pRes->iNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TObject * k = new TObject;

			memset(k, 0, sizeof(TObject));

			int col = 0;

			k->dwID = atoi(data[col++]);
			k->dwLandID = atoi(data[col++]);
			k->dwVnum = atoi(data[col++]);
			k->lMapIndex = atoi(data[col++]);
			k->x = atoi(data[col++]);
			k->y = atoi(data[col++]);
			k->xRot = static_cast<float>(std::atof(data[col++]));
			k->yRot = static_cast<float>(std::atof(data[col++]));
			k->zRot = static_cast<float>(std::atof(data[col++]));
			k->lLife = atoi(data[col++]);

			m_map_pkObjectTable.insert(std::make_pair(k->dwID, k));
		}

	return true;
}

bool CClientManager::InitializeMobTable()
{
	if (!dbsql::IsIdentifier(g_stLocaleNameColumn))
	{
		sys_err("Invalid locale column name");
		return false;
	}
	char query[4096];
	dbsql::snprintf(query, sizeof(query),
			"SELECT vnum, name, `%s`, type, `rank`, battle_type, level, size+0, ai_flag+0, setRaceFlag+0, setImmuneFlag+0, on_click, empire, drop_item, resurrection_vnum, folder, st, dx, ht, iq, damage_min, damage_max, max_hp, regen_cycle, regen_percent, exp, gold_min, gold_max, def, attack_speed, move_speed, aggressive_hp_pct, aggressive_sight, attack_range, polymorph_item, enchant_curse, enchant_slow, enchant_poison, enchant_stun, enchant_critical, enchant_penetrate, resist_sword, resist_twohand, resist_dagger, resist_bell, resist_fan, resist_bow, resist_fire, resist_elect, resist_magic, resist_wind, resist_poison, dam_multiply, summon, drain_sp, skill_vnum0, skill_level0, skill_vnum1, skill_level1 , sp_berserk, sp_stoneskin, sp_godspeed, sp_deathblow, sp_revive  FROM mob_proto%s ORDER BY vnum",
			g_stLocaleNameColumn.c_str(), GetTablePostfix());

	std::unique_ptr<SQLMsg> message(CDBManager::instance().DirectQuery(query));
	SQLResult* result = message->Get();
	if (!result->iNumRows)
		return true;

	if (!m_vec_mobTable.empty())
		m_vec_mobTable.clear();

	m_vec_mobTable.resize(result->iNumRows);
	TMobTable* mob = &m_vec_mobTable[0];
	memset(mob, 0, sizeof(*mob) * m_vec_mobTable.size());

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result->pSQLResult)))
	{
		int col = 0;
		mob->dwVnum = atoi(row[col++]);
		strncpy(mob->szName, row[col++], sizeof(mob->szName) - 1);
		mob->szName[sizeof(mob->szName) - 1] = '\0';
		strncpy(mob->szLocaleName, row[col++], sizeof(mob->szLocaleName) - 1);
		mob->szLocaleName[sizeof(mob->szLocaleName) - 1] = '\0';
		mob->bType = dbinput::ReadInteger<decltype(mob->bType)>(row[col++]);
		mob->bRank = dbinput::ReadInteger<decltype(mob->bRank)>(row[col++]);
		mob->bBattleType = dbinput::ReadInteger<decltype(mob->bBattleType)>(row[col++]);
		mob->bLevel = dbinput::ReadInteger<decltype(mob->bLevel)>(row[col++]);
		mob->bSize = dbinput::ReadInteger<decltype(mob->bSize)>(row[col++]);
		mob->dwAIFlag = atoi(row[col++]);
		mob->dwRaceFlag = atoi(row[col++]);
		mob->dwImmuneFlag = atoi(row[col++]);
		mob->bOnClickType = dbinput::ReadInteger<decltype(mob->bOnClickType)>(row[col++]);
		mob->bEmpire = dbinput::ReadInteger<decltype(mob->bEmpire)>(row[col++]);
		mob->dwDropItemVnum = atoi(row[col++]);
		mob->dwResurrectionVnum = atoi(row[col++]);

		if (row[col])
			CopyBootText(mob->szFolder, row[col]);
		++col;

		mob->bStr = dbinput::ReadInteger<decltype(mob->bStr)>(row[col++]);
		mob->bDex = dbinput::ReadInteger<decltype(mob->bDex)>(row[col++]);
		mob->bCon = dbinput::ReadInteger<decltype(mob->bCon)>(row[col++]);
		mob->bInt = dbinput::ReadInteger<decltype(mob->bInt)>(row[col++]);
		mob->dwDamageRange[0] = atoi(row[col++]);
		mob->dwDamageRange[1] = atoi(row[col++]);
		mob->dwMaxHP = atoi(row[col++]);
		mob->bRegenCycle = dbinput::ReadInteger<decltype(mob->bRegenCycle)>(row[col++]);
		mob->bRegenPercent = dbinput::ReadInteger<decltype(mob->bRegenPercent)>(row[col++]);
		mob->dwExp = atoi(row[col++]);
		mob->dwGoldMin = atoi(row[col++]);
		mob->dwGoldMax = atoi(row[col++]);
		mob->wDef = dbinput::ReadInteger<decltype(mob->wDef)>(row[col++]);
		mob->sAttackSpeed = dbinput::ReadInteger<decltype(mob->sAttackSpeed)>(row[col++]);
		mob->sMovingSpeed = dbinput::ReadInteger<decltype(mob->sMovingSpeed)>(row[col++]);
		mob->bAggresiveHPPct = dbinput::ReadInteger<decltype(mob->bAggresiveHPPct)>(row[col++]);
		mob->wAggressiveSight = dbinput::ReadInteger<decltype(mob->wAggressiveSight)>(row[col++]);
		mob->wAttackRange = dbinput::ReadInteger<decltype(mob->wAttackRange)>(row[col++]);
		mob->dwPolymorphItemVnum = atoi(row[col++]);

		for (int i = 0; i < 6; ++i)
			mob->cEnchants[i] = dbinput::ReadInteger<std::remove_reference_t<decltype(mob->cEnchants[i])>>(row[col++]);
		for (int i = 0; i < 11; ++i)
			mob->cResists[i] = dbinput::ReadInteger<std::remove_reference_t<decltype(mob->cResists[i])>>(row[col++]);

		mob->fDamMultiply = static_cast<float>(std::atof(row[col++]));
		mob->dwSummonVnum = atoi(row[col++]);
		mob->dwDrainSP = atoi(row[col++]);
		mob->Skills[0].dwVnum = atoi(row[col++]);
		mob->Skills[0].bLevel = dbinput::ReadInteger<std::remove_reference_t<decltype(mob->Skills[0].bLevel)>>(row[col++]);
		mob->Skills[1].dwVnum = atoi(row[col++]);
		mob->Skills[1].bLevel = dbinput::ReadInteger<std::remove_reference_t<decltype(mob->Skills[1].bLevel)>>(row[col++]);
		mob->bBerserkPoint = dbinput::ReadInteger<decltype(mob->bBerserkPoint)>(row[col++]);
		mob->bStoneSkinPoint = dbinput::ReadInteger<decltype(mob->bStoneSkinPoint)>(row[col++]);
		mob->bGodSpeedPoint = dbinput::ReadInteger<decltype(mob->bGodSpeedPoint)>(row[col++]);
		mob->bDeathBlowPoint = dbinput::ReadInteger<decltype(mob->bDeathBlowPoint)>(row[col++]);
		mob->bRevivePoint = dbinput::ReadInteger<decltype(mob->bRevivePoint)>(row[col++]);

		++mob;
	}

	return true;
}

bool CClientManager::InitializeItemTable()
{
	if (!dbsql::IsIdentifier(g_stLocaleNameColumn))
	{
		sys_err("Invalid locale column name");
		return false;
	}
	static const char* s_szQuery =
			"SELECT vnum, type, subtype, name, `%s`, gold, shop_buy_price, weight, size, flag, wearflag, "
			"antiflag, immuneflag+0, refined_vnum, refine_set, magic_pct, socket_pct, addon_type, "
			"limittype0, limitvalue0, limittype1, limitvalue1, applytype0, applyvalue0, applytype1, "
			"applyvalue1, applytype2, applyvalue2, value0, value1, value2, value3, value4, value5 "
			"FROM item_proto%s ORDER BY vnum";
	char query[4096];
	dbsql::snprintf(query, sizeof(query), s_szQuery, g_stLocaleNameColumn.c_str(), GetTablePostfix());

	std::unique_ptr<SQLMsg> message(CDBManager::instance().DirectQuery(query));
	SQLResult* result = message->Get();

	if (!result->iNumRows)
	{
		sys_err("query error: %s", query);
		return false;
	}

	if (!m_vec_itemTable.empty())
	{
		m_vec_itemTable.clear();
		m_map_itemTableByVnum.clear();
	}

	m_vec_itemTable.resize(result->iNumRows);
	TItemTable* item = &m_vec_itemTable[0];
	memset(item, 0, sizeof(*item) * m_vec_itemTable.size());

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result->pSQLResult)))
	{
		int col = 0;
		item->dwVnum = atoi(row[col++]);
		item->bType = dbinput::ReadInteger<decltype(item->bType)>(row[col++]);
		item->bSubType = dbinput::ReadInteger<decltype(item->bSubType)>(row[col++]);
		strncpy(item->szName, row[col++], sizeof(item->szName) - 1);
		item->szName[sizeof(item->szName) - 1] = '\0';
		strncpy(item->szLocaleName, row[col++], sizeof(item->szLocaleName) - 1);
		item->szLocaleName[sizeof(item->szLocaleName) - 1] = '\0';
		item->dwGold = atoi(row[col++]);
		item->dwShopBuyPrice = atoi(row[col++]);
		item->bWeight = dbinput::ReadInteger<decltype(item->bWeight)>(row[col++]);
		item->bSize = dbinput::ReadInteger<decltype(item->bSize)>(row[col++]);
		item->dwFlags = atoi(row[col++]);
		item->dwWearFlags = atoi(row[col++]);
		item->dwAntiFlags = atoi(row[col++]);
		item->dwImmuneFlag = atoi(row[col++]);
		item->dwRefinedVnum = atoi(row[col++]);
		item->wRefineSet = dbinput::ReadInteger<decltype(item->wRefineSet)>(row[col++]);
		item->bAlterToMagicItemPct = dbinput::ReadInteger<decltype(item->bAlterToMagicItemPct)>(row[col++]);
		item->bGainSocketPct = dbinput::ReadInteger<decltype(item->bGainSocketPct)>(row[col++]);
		item->sAddonType = dbinput::ReadInteger<decltype(item->sAddonType)>(row[col++]);

		for (int i = 0; i < 2; ++i)
		{
			item->aLimits[i].bType = dbinput::ReadInteger<std::remove_reference_t<decltype(item->aLimits[i].bType)>>(row[col++]);
			item->aLimits[i].lValue = atoi(row[col++]);
		}
		for (int i = 0; i < 3; ++i)
		{
			item->aApplies[i].bType = dbinput::ReadInteger<std::remove_reference_t<decltype(item->aApplies[i].bType)>>(row[col++]);
			item->aApplies[i].lValue = atoi(row[col++]);
		}
		for (int i = 0; i < 6; ++i)
			item->alValues[i] = atoi(row[col++]);

		m_map_itemTableByVnum.insert(std::make_pair(item->dwVnum, item));
		++item;
	}

	m_map_itemTableByVnum.clear();
	for (std::vector<TItemTable>::iterator it = m_vec_itemTable.begin(); it != m_vec_itemTable.end(); ++it)
	{
		TItemTable& entry = *it;
		m_map_itemTableByVnum.insert(std::make_pair(entry.dwVnum, &entry));
	}

	return true;
}
