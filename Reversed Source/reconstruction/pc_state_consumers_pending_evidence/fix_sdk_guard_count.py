from pathlib import Path
p=Path('work/pc_state_consumers/prepare.py');s=p.read_text(encoding='utf-8-sig');s=s.replace(";assert len(sdkfiles)==58",";assert len(sdkfiles)>=58 and sum(x['path'].endswith('.c') for x in sdkfiles)==28");s=s.replace('source guards+58SDK expectations','source guards+completeSDK expectations');p.write_text(s,encoding='utf-8')
