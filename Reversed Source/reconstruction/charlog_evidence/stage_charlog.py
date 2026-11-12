# Resolve portable paths from the repository root, regardless of current working directory.
import sys as _portable_sys
from pathlib import Path as _portable_Path
for _portable_parent in (_portable_Path(__file__).resolve().parent, *_portable_Path(__file__).resolve().parents):
    if (_portable_parent / 'project_paths.py').is_file():
        _portable_sys.path.insert(0, str(_portable_parent))
        break
else:
    raise RuntimeError('project_paths.py not found in repository ancestors')
from project_paths import resolve_path as _resolve_portable_path
from pathlib import Path
import json,hashlib
p=Path(__file__).parent
src=Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source'))
body=r"""
//080785a0: exact old six-argument CHARACTER audit path.
void LogManager::CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint) {
    EscapeString(__escape_hint,hint,std::strlen(hint));
    const char* format="INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), %lu, %lu, %lu, %lu, '%s', '%s')";
    sys_log(0,format,get_table_postfix(),pid,x,y,value,text,hint);
    Query(format,get_table_postfix(),pid,x,y,value,text,__escape_hint);
}
"""
oldh=src.joinpath('game/src/log.h').read_text()
oldcpp=src.joinpath('game/src/log.cpp').read_text()
assert 'void CharLog(' not in oldh and 'void LogManager::CharLog(' not in oldcpp
h=oldh.replace('private:\n    void Query(const char* format,...);','    void Query(const char* format,...);\n    void CharLog(DWORD pid,DWORD x,DWORD y,DWORD value,const char* text,const char* hint);\nprivate:')
assert h!=oldh
assert h[h.index('    CAsyncSQL m_sql;'):]==oldh[oldh.index('    CAsyncSQL m_sql;'):]
p.joinpath('candidate-log.h').write_text(h)
p.joinpath('candidate-log.cpp').write_text(oldcpp+body)
p.joinpath('charlog-body.inc').write_text(body)
for n in ('logger-quest-dwarf.json',):
 p.joinpath(n).write_bytes(Path(_resolve_portable_path('USER_HOME\\Documents\\Codex\\2026-10-23\\i-h\\work\\point_change')).joinpath(n).read_bytes())
p.joinpath('logger-globals-dwarf.json').write_bytes(src.joinpath('reconstruction/logger-globals-dwarf.json').read_bytes())
for f in p.glob('*-decompile.json'):
 d=json.loads(f.read_text());f.with_suffix('.c').write_text(d['code'])
fixture=src.joinpath('reconstruction/tests/item_audit_regression.cpp').read_text().split('int main() {')[0]
fixture=fixture.replace('static std::string actions,last_log,last_locale;','static std::string actions,last_log,last_locale;\nstatic bool change_postfix,reenter;\nstatic LogManager* current_logger;')
fixture=fixture.replace('assert(db==escaped_handle); unsigned long used=0;','assert(db==escaped_handle); actions+=\'E\'; unsigned long used=0;')
fixture=fixture.replace('last_log=text;','''last_log=text; actions+='S';
    if(change_postfix) { change_postfix=false; g_table_postfix="_after"; }
    if(reenter) { reenter=false; current_logger->ItemLog(9,8,7,6,"INNER","nested'\\\\\\\\hint"); }''')
main=r"""
int main() {
    AuditRuntime runtime; sql_test_runtime=&runtime;
    {
        LogManager logger; current_logger=&logger;
        escaped_handle=logger.m_sql.GetSQLHandle(); active=&logger.m_sql;
        assert(logger.Connect("host","user","password","database"));
        // Real Setup initializes mutexes; host thread-create boundary fails as in existing fixture.
        logger.m_bIsConnect=false;
        g_table_postfix="_old"; actions.clear();
        logger.CharLog(0xffffffffUL,0x80000000UL,0xffffffffUL,0xffffffffUL,"GOLD_OVERFLOW","a'b\\c");
        assert(actions=="ESLPU" && !locked && logger.m_sql.m_queue_query.size()==1);
        SQLMsg* message=logger.m_sql.m_queue_query.front();
        const std::string prefix="INSERT DELAYED INTO log_old (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 4294967295, 2147483648, 4294967295, 4294967295, 'GOLD_OVERFLOW', '";
        assert(last_log==prefix+"a'b\\c')");
        assert(message->stQuery==prefix+"a\\'b\\\\c')");
        assert(message->iID==1 && !message->bReturn && !message->pvUserData && message->m_pkSQL==escaped_handle);
        assert(!logger.IsConnected());

        // The original does not cache postfix across sys_log.
        change_postfix=true; g_table_postfix="_before"; actions.clear();
        logger.CharLog(1,2,3,4,"T","h");
        assert(actions=="ESLPU");
        assert(last_log.find("INTO log_before ")!=std::string::npos);
        assert(logger.m_sql.m_queue_query.back()->stQuery.find("INTO log_after ")!=std::string::npos);

        // Empty hints replace prior escaped contents; text is not escaped by CharLog.
        actions.clear(); g_table_postfix.clear();
        logger.CharLog(0,0,0,0,"raw'text","");
        assert(actions=="ESLPU");
        assert(logger.m_sql.m_queue_query.back()->stQuery==
            "INSERT DELAYED INTO log (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 0, 0, 0, 0, 'raw'text', '')");

        // The1024-byte buffer is shared with ItemLog. Preserve real callback ordering.
        // Inner ItemLog queues first and overwrites the escaped buffer used by outer CharLog.
        actions.clear(); reenter=true;
        logger.CharLog(1,2,3,4,"OUTER","outer'hint");
        assert(actions=="ESESLPULPU");
        assert(logger.m_sql.m_queue_query.size()==5);
        assert(logger.m_sql.m_queue_query.back()->stQuery==
            "INSERT DELAYED INTO log (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 1, 2, 3, 4, 'OUTER', 'nested\\'\\\\hint')");

        // Safe boundary size:511 quotes expand to1022 bytes plus terminator.
        actions.clear();
        std::string quotes(511,'\''); logger.CharLog(1,2,3,4,"BOUNDARY",quotes.c_str());
        assert(actions=="ESLPU");
        std::string escaped; for(unsigned i=0;i<511;++i) escaped+="\\'";
        assert(logger.m_sql.m_queue_query.back()->stQuery==
            "INSERT DELAYED INTO log (type, time, who, x, y, what, how, hint) VALUES('CHARACTER', NOW(), 1, 2, 3, 4, 'BOUNDARY', '"+escaped+"')");
        // No app lifecycle/query/escape wrapper is stubbed. Drain real SQLMsg queue.
        while(!logger.m_sql.m_queue_query.empty()) {
            delete logger.m_sql.m_queue_query.front();logger.m_sql.m_queue_query.pop();
        }
        active=0;current_logger=0;
    }
    assert(!LogManager::instance_ptr() && initialized==1 && destroyed==1);
    std::puts("CharLog passed: exact CHARACTER SQL, raw/escaped hints, uint32, postfix reread, shared buffer and real async queue");
}
"""
p.joinpath('charlog_regression.cpp').write_text(fixture+main)
p.joinpath('charlog_abi.cpp').write_text(r"""
#include "../../game/src/log.h"
#include <type_traits>
typedef void(LogManager::*CharLogSignature)(DWORD,DWORD,DWORD,DWORD,const char*,const char*);
static_assert(std::is_same<decltype(&LogManager::CharLog),CharLogSignature>::value,"old6arg nonconst CharLog");
static_assert(sizeof(LogManager)==1160,"complete old logger ABI");
static_assert(sizeof(DWORD)==4,"target DWORD");
static_assert(std::is_same<decltype(&LogManager::Query),void(LogManager::*)(const char*,...)>::value,"Query public original API");
""")
print('Full logger data preserved; actual existing methods retained; CharLog appended; Query access corrected. Tests staged only.')
