#!/usr/bin/env python3
"""Build conservative address-keyed coverage; never infer recovery from names/tests.

Usage: python reconstruction/tools/update_coverage.py --inventory PATH
Overrides are explicit human/agent review records, not automatic implementation proof.
"""
import argparse
import collections
import hashlib
import json
from pathlib import Path


def address(value):
    try:
        return f'{int(str(value), 16):08x}'
    except (ValueError, TypeError):
        return None


def walk(value, pointer=''):
    if isinstance(value, dict):
        f = value.get('function')
        if isinstance(f, dict) and f.get('address') and value.get('code') and not value.get('error'):
            yield address(f['address']), pointer, not value.get('truncated', False)
        for key, item in value.items():
            yield from walk(item, pointer + '/' + str(key).replace('~', '~0').replace('/', '~1'))
    elif isinstance(value, list):
        for i, item in enumerate(value):
            yield from walk(item, pointer + '/' + str(i))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--inventory', type=Path, required=True)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--overrides', type=Path)
    parser.add_argument('--output-directory', type=Path, help='Defaults to root/reconstruction; supports staging when root is read-only')
    args = parser.parse_args()
    root = args.root.resolve()
    rec = root / 'reconstruction'
    output = args.output_directory or rec
    output.mkdir(parents=True, exist_ok=True)
    raw = args.inventory.read_bytes()
    inv = json.loads(raw)
    if inv['function_count'] != 7238 or len(inv['functions']) != 7238:
        raise ValueError('Expected the exhaustively paginated 7,238-function inventory')
    entries = {}
    for f in inv['functions']:
        a = address(f['address'])
        if a in entries or a is None:
            raise ValueError('Invalid or duplicate inventory address')
        entries[a] = dict(address=a, name=f['name'], evidence_status='not_indexed', evidence_refs=[],
                          implementation_status='unreviewed', review_status='unreviewed',
                          test_mapping_status='unreviewed', coverage_status='unreviewed')
    warnings = []
    for path in sorted(rec.glob('*ghidra*.json')):
        try:
            data = json.loads(path.read_text(encoding='utf-8-sig'))
        except (ValueError, OSError) as error:
            warnings.append(f'{path.name}: {error}')
            continue
        if isinstance(data, dict) and data.get('target_sha256', inv['target_sha256']) != inv['target_sha256']:
            warnings.append(f'{path.name}: target hash mismatch; excluded')
            continue
        for a, pointer, complete in walk(data):
            if a in entries:
                entries[a]['evidence_refs'].append(dict(path=path.relative_to(root).as_posix(),
                    json_pointer=pointer, complete_export=complete,
                    provenance='declared_target_hash' if isinstance(data, dict) and data.get('target_sha256') else 'context_only_no_embedded_hash'))
                entries[a]['evidence_status'] = 'available'
                entries[a]['coverage_status'] = 'evidence_available'
    overrides_path = args.overrides or rec / 'function-coverage-overrides.json'
    overrides = json.loads(overrides_path.read_text(encoding='utf-8-sig'))
    if overrides['target_sha256'] != inv['target_sha256']:
        raise ValueError('Override target hash mismatch')
    for override in overrides['reviews']:
        entry = entries[address(override['address'])]
        if not entry['evidence_refs']:
            raise ValueError('Reviewed entries require address-backed evidence')
        for ref in override['source_refs'] + override.get('test_refs', []):
            text = (root / ref['path']).read_text(encoding='utf-8-sig')
            if ref['anchor'] not in text:
                raise ValueError(f'Stale anchor: {ref}')
            ref['line'] = text[:text.index(ref['anchor'])].count('\n') + 1
            ref['file_sha256'] = hashlib.sha256((root / ref['path']).read_bytes()).hexdigest()
        entry.update(override)
        entry['implementation_status'] = 'candidate_implementation'
        entry['review_status'] = 'explicitly_reviewed_mapping'
        entry['test_mapping_status'] = 'explicit_module_suite_mapping' if override.get('test_refs') else 'unreviewed'
        entry['coverage_status'] = 'reviewed_implementation_test_mapping' if override.get('test_refs') else 'candidate_implementation'
    counts = dict(collections.Counter(e['coverage_status'] for e in entries.values()))
    out = dict(schema_version=1, target_sha256=inv['target_sha256'], function_count=len(entries),
        inventory_sha256=hashlib.sha256(raw).hexdigest(), inventory_origin=inv.get('source'),
        policy='Reviewed mapping means a bounded source/evidence/test association, not complete function recovery, ABI equivalence, all-branch coverage or target execution. Unreviewed never means unrecovered.',
        evidence_scope='Only structured function/address/code objects in reconstruction/*ghidra*.json are indexed. Metadata, assembly and notes may contain additional evidence. Context-only exports have no embedded target hash.',
        test_execution='This updater does not execute tests or the original ELF. Test references describe reviewed suite associations only.',
        summary=counts, evidence_available=sum(e['evidence_status']=='available' for e in entries.values()),
        warnings=warnings, functions=list(entries.values()))
    (output / 'function-coverage-ledger.json').write_text(json.dumps(out, indent=2) + '\n', encoding='utf-8')
    lines = ['# Conservative target function coverage ledger', '', f"Target SHA-256: `{inv['target_sha256']}`", '',
             'Inventory: exactly 7,238 internal Ghidra function entries. Address identity, not name matching, defines the denominator.', '',
             '| Status | Functions |', '| --- | ---: |']
    lines += [f'| {k} | {v} |' for k,v in sorted(counts.items())]
    lines += ['', f"Structured Ghidra evidence indexed for {out['evidence_available']} entries (overlaps reviewed entries).", '', out['policy'], '', out['evidence_scope'], '', out['test_execution'], '',
              'All remaining entries retain review_status=unreviewed. No module-suite pass is promoted to whole-function recovery. Constructor/destructor variants remain separate target entries. The full reconstruction goal is unfinished.', '',
              '## Explicit review mappings', '', '| Target address | Function | Source | Suite association | Review scope |', '| --- | --- | --- | --- | --- |']
    for e in entries.values():
        if e['review_status'] != 'unreviewed':
            refs = lambda key: ', '.join(f"{r['path']}:{r['line']}" for r in e.get(key, []))
            lines.append(f"| {e['address']} | {e['name']} | {refs('source_refs')} | {refs('test_refs')} | {e['review_scope']} |")
    lines += ['', '## Refresh', '', 'Run `python reconstruction/tools/update_coverage.py --inventory <exhaustive-inventory.json>`. Add explicit address-keyed records to `reconstruction/function-coverage-overrides.json`; source and test anchors must still exist. File hashes and current line numbers are captured on every refresh. Automatic indexing never reviews implementations. Review changes in source bodies manually even when anchors still exist.', '']
    (output / 'function-coverage-ledger.md').write_text('\n'.join(lines), encoding='utf-8')
    print(json.dumps(dict(function_count=len(entries), summary=counts, evidence_available=out['evidence_available'], warnings=len(warnings))))


if __name__ == '__main__':
    main()
