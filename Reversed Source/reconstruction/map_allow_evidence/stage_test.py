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

original = Path(_resolve_portable_path('PROJECT_ROOT\\Reversed Source\\reconstruction\\tests\\locale_regression.cpp'))
output = Path('work/map_allow/locale_regression.cpp')
text = original.read_bytes()
eol = b'\r\n' if b'\r\n' in text else b'\n'

old_log = b'extern "C" void sys_log(int,const char*,...) { assert(!"unused locale diagnostic path"); }'
new_log = '''static std::vector<int> logged_map_allow;
extern "C" void sys_log(int level,const char* format,...)
{
    assert(level == 0 && std::strcmp(format, "MAP_ALLOW: %d") == 0);
    va_list args;
    va_start(args, format);
    logged_map_allow.push_back(va_arg(args, int));
    va_end(args);
}'''
assert text.count(old_log) == 1
text = text.replace(old_log, new_log.encode().replace(b'\n', eol))
assert text.count(b'#include <string>') == 1
text = text.replace(b'#include <string>', b'#include <string>' + eol + b'#include <vector>')
extra = '''static void CheckMapAllow()
{
    const BYTE guard = 0xA5;
    BYTE output[] = { guard, guard, guard, guard, guard, guard };
    assert(!g_bAuthServer);
    assert(!map_allow_find(0));
    map_allow_copy(output, 1);
    map_allow_log();
    assert(logged_map_allow.empty());
    assert(output[0] == guard);

    map_allow_add(513);
    map_allow_add(-2);
    map_allow_add(300);
    assert(map_allow_find(-2) && map_allow_find(300) && map_allow_find(513));
    assert(!map_allow_find(0) && !map_allow_find(44));

    // Nonempty copies store before testing size, with signed sorting and BYTE narrowing.
    map_allow_copy(output + 1, -1);
    assert(output[0] == guard && output[1] == 254 && output[2] == guard);
    output[1] = guard;
    map_allow_copy(output + 1, 0);
    assert(output[0] == guard && output[1] == 254 && output[2] == guard);
    output[1] = guard;
    map_allow_copy(output + 1, 1);
    assert(output[0] == guard && output[1] == 254 && output[2] == 44 && output[3] == guard);

    g_bAuthServer = true;
    assert(!map_allow_find(-2) && !map_allow_find(513));
    map_allow_add(513); // Auth mode never detects duplicates in find; set insertion stays unique.
    map_allow_add(9);
    assert(!map_allow_find(9));
    map_allow_copy(output + 1, 20);
    assert(output[0] == guard && output[1] == 254 && output[2] == 9 &&
           output[3] == 44 && output[4] == 1 && output[5] == guard);
    map_allow_log();
    assert(logged_map_allow.size() == 4 && logged_map_allow[0] == -2 &&
           logged_map_allow[1] == 9 && logged_map_allow[2] == 300 && logged_map_allow[3] == 513);
    g_bAuthServer = false;
    assert(map_allow_find(9) && map_allow_find(513));
}

int main() {
    CheckMapAllow();'''
assert text.count(b'int main() {') == 1
output.write_bytes(text.replace(b'int main() {', extra.encode().replace(b'\n', eol)))
