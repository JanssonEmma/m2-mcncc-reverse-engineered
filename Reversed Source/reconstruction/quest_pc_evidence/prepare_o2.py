from pathlib import Path
s=Path('work/item_attribute_o2.py').read_text()
s=s.replace('item_attribute_regression','quest_pc_lookup_regression').replace('item_attribute_o2','quest_pc_lookup_o2').replace('item-attribute','quest-pc-lookup')
s=s.replace("assert '../../game/src/item_attribute.cpp' in args and '../../game/src/item.cpp' in args","assert '../../game/src/questpc.cpp' in args and '../../game/src/questevent.cpp' in args and '../../game/src/questmanager.cpp' in args")
a=s.index('sources=[');b=s.index('\nreceipt=',a)
s=s[:a]+"sources=['../../game/src/questpc.h','../../game/src/questpc.cpp','../../game/src/questevent.h','../../game/src/questevent.cpp','../../game/src/questmanager.h','../../game/src/questmanager.cpp','../../game/src/event.cpp','../../game/src/event_queue.cpp','quest_flag_fixture.cpp','desc_fixture.cpp','quest_pc_lookup_regression.cpp','quest_pc_lookup_desc_output_syscalls.c']"+s[b:]
s=s.replace('Optimized actual item attribute/socket graph passed','Optimized actual PC lifecycle/lookup/real event graph passed')
Path('work/quest_pc_lookup_o2.py').write_text(s)
