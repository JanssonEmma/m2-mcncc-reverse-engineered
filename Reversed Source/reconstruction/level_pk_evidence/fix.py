from pathlib import Path
p=Path('work/level_pk/level_pk_regression.cpp');s=p.read_text().replace('#include <climits>','#include <climits>\n#include <cstdlib>').replace(';fdwatch_delete(watch);event_destroy();',';fdwatch_delete(watch);');p.write_text(s)
