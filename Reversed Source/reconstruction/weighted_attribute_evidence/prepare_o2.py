from pathlib import Path
s=Path('work/item_attribute_o2.py').read_text()
s=s.replace('item_attribute_regression','weighted_attribute_regression').replace('item_attribute_o2','weighted_attribute_o2').replace('item-attribute','weighted-attribute')
s=s.replace("sources=['../../game/src/item.cpp','../../game/src/item_attribute.cpp','../../game/src/event.cpp','../../game/src/event_queue.cpp','weighted_attribute_regression.cpp']","sources=['../../common/tables.h','../../game/src/constants.h','../../game/src/constants.cpp','../../game/src/item.h','../../game/src/item.cpp','../../game/src/item_attribute.cpp','../../game/src/event.cpp','../../game/src/event_queue.cpp','weighted_attribute_regression.cpp']")
s=s.replace('Optimized actual item attribute/socket graph passed','Optimized actual weighted selection/magic/maps graph passed')
Path('work/weighted_attribute_o2.py').write_text(s)
