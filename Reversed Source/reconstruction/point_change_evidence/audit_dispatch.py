import pathlib,json,re
p=pathlib.Path('work/point_change');source=p.joinpath('PointChange.cpp.candidate').read_text().split('    goto epilogue;')[0]
actual=sorted({int(x) for x in re.findall(r'\bcase\s+(\d+)\s*:',source)})
targets=json.loads(p.joinpath('switch-targets.json').read_text());expected=sorted(x['type'] for x in targets if x['target']!='0x812f5d0')
result={'audit':'source-explicit-byte-cases vs original127-entry jumptable','explicit_source':actual,'explicit_original':expected,'missing':sorted(set(expected)-set(actual)),'unexpected':sorted(set(actual)-set(expected)),'pass':actual==expected,'unknown_original':[x['type'] for x in targets if x['target']=='0x812f5d0'],'out_of_table_bytes':'127..255 -> originaldefault'}
p.joinpath('source-dispatch-audit.json').write_text(json.dumps(result,indent=2));print({'pass':result['pass'],'source_count':len(actual),'missing':result['missing'],'unexpected':result['unexpected']})
