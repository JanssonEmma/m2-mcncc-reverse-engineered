from pathlib import Path
p=Path('work/party_dependency/party_dependency_regression.cpp');s=p.read_text().replace('"PARTY send info source[1000] to member[%lu]",obs.recipients[obs.sent]','"PARTY send info source[1000] to %s[%lu]",obs.recipients[obs.sent]==1000?"source":"member",obs.recipients[obs.sent]');p.write_text(s)
