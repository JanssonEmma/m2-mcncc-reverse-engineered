"""Run focused/full reconstruction checks and record evidence from actual processes."""
import argparse
import datetime
import json
import hashlib
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile


def save(path, value):
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.NamedTemporaryFile(mode='w', encoding='utf-8', dir=path.parent,
                                     delete=False) as out:
        json.dump(value, out, indent=2)
        out.write('\n')
        temporary = out.name
    os.replace(temporary, path)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('mode', choices=['plan', 'run'])
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--make', default=shutil.which('gmake') or shutil.which('make'))
    parser.add_argument('--cc', default=shutil.which('gcc') or 'cc')
    parser.add_argument('--cxx', default=shutil.which('g++') or 'c++')
    parser.add_argument('--suite', action='append', default=[])
    parser.add_argument('--full', action='store_true')
    parser.add_argument('--abi', action='store_true')
    parser.add_argument('--jobs', type=int, default=3)
    parser.add_argument('--force', action='store_true', help='Rebuild requested targets regardless of timestamps')
    parser.add_argument('--log', type=Path)
    args = parser.parse_args()
    tests = args.root / 'tests'
    text = (tests / 'Makefile').read_text(encoding='utf-8')
    match = re.search(r'^RUNTIME_TESTS = (.+)$', text, re.M)
    if not match:
        parser.error('RUNTIME_TESTS declaration missing')
    expected = match.group(1).split()
    if len(expected) != len(set(expected)):
        parser.error('Duplicate runtime target')
    if set(args.suite) - set(expected):
        parser.error('Unknown runtime target')
    if args.full and args.suite:
        parser.error('--full and --suite are mutually exclusive')
    if not args.full and not args.suite and not args.abi:
        parser.error('Choose --full, --suite NAME, or --abi')
    if not args.make or args.jobs < 1:
        parser.error('Working make executable and positive jobs count required')
    project = args.root.parent
    headers = sorted(set(project.rglob('*.h')) | set(project.rglob('*.hpp')))
    header_records = {str(p.relative_to(project)).replace('\\', '/'): hashlib.sha256(p.read_bytes()).hexdigest() for p in headers}
    header_fingerprint = hashlib.sha256(json.dumps(header_records, sort_keys=True).encode('utf-8')).hexdigest()
    header_state = args.root / 'validated-header-state.json'
    previous = json.loads(header_state.read_text(encoding='utf-8')) if header_state.exists() else {}
    headers_changed = previous.get('fingerprint') != header_fingerprint
    command = [args.make, '-j' + str(args.jobs), 'CC=' + args.cc.replace('\\', '/'),
               'CXX=' + args.cxx.replace('\\', '/')]
    if args.force or headers_changed:
        command.append('-B')
    if os.name == 'nt':
        command.append('SHELL=cmd.exe')
    targets = ['check', 'check-abi'] if args.full else args.suite + (['check-abi'] if args.abi else [])
    if args.mode == 'plan':
        plan = subprocess.run(command + ['-n'] + targets, cwd=tests,
                              capture_output=True, text=True)
        if plan.returncode:
            sys.stderr.write(plan.stderr)
            return plan.returncode
        compiles = []
        for line in plan.stdout.splitlines():
            parts = shlex.split(line)
            if parts and '-o' in parts:
                compiles.append(parts[parts.index('-o') + 1])
        print(json.dumps({'runtime_targets': expected if args.full else args.suite,
                          'scheduled_compiler_outputs': compiles,
                          'scheduled_compiler_output_count': len(compiles), 'headers_changed_since_last_full_validation': headers_changed, 'forced_rebuild': args.force or headers_changed}, indent=2))
        return 0
    now = datetime.datetime.now(datetime.timezone.utc).isoformat()
    log = (args.log or args.root / 'validation_runs' / (now[:19].replace(':', '-') + '.log')).resolve()
    log.parent.mkdir(parents=True, exist_ok=True)
    receipt = {'started_utc': now, 'status': 'starting', 'scope': 'full' if args.full else 'focused',
               'suites': expected if args.full else args.suite, 'abi_requested': args.full or args.abi,
               'header_fingerprint': header_fingerprint, 'headers_changed': headers_changed, 'forced_rebuild': args.force or headers_changed,
               'command': command + targets, 'cwd': str(tests.resolve()), 'log': str(log),
               'limitation': 'Scripted external APIs and test-only lifecycle boundaries remain; passing does not prove full server linking.'}
    receipt_path = log.with_suffix('.receipt.json')
    operation = args.root / 'OPERATION_STATE.json'

    def checkpoint():
        save(receipt_path, receipt)
        save(operation, receipt)
        (args.root / 'OPERATION_STATE.md').write_text(
            '# Current validation operation\n\n' + '\n'.join(
                '- **' + key + '**: ' + str(receipt[key]) for key in
                ('started_utc', 'status', 'scope', 'cwd', 'log')) +
            '\n\nInspect the JSON receipt for PID, exact arguments and final exit codes. '
            'A running receipt alone does not prove its process is still alive.\n', encoding='utf-8')

    checkpoint()
    with log.open('w', encoding='utf-8') as output:
        process = subprocess.Popen(command + targets, cwd=tests, stdout=output, stderr=subprocess.STDOUT)
        receipt.update(status='running', pid=process.pid)
        checkpoint()
        code = process.wait()
        receipt['make_exit_code'] = code
        if not code and not args.full:
            for suite in args.suite:
                executable = tests / (suite + ('.exe' if os.name == 'nt' else ''))
                output.write('./' + executable.name + '\n')
                output.flush()
                process = subprocess.Popen([str(executable.resolve())], cwd=tests,
                                           stdout=output, stderr=subprocess.STDOUT)
                receipt.update(pid=process.pid, running_suite=suite)
                checkpoint()
                code = process.wait()
                if code:
                    break
    content = log.read_text(encoding='utf-8', errors='replace')
    actual = re.findall(r'^\./([A-Za-z0-9_]+?)(?:\.exe)?\s*$', content, re.M)
    receipt.update(status='passed' if code == 0 else 'failed', exit_code=code,
                   finished_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                   executed_runtime_commands=actual)
    receipt.pop('running_suite', None)
    if code == 0 and actual != (expected if args.full else args.suite):
        receipt.update(status='evidence_mismatch', exit_code=2)
        code = 2
    checkpoint()
    if code == 0 and args.full:
        manifest_path = args.root / 'validation-status.json'
        manifest = json.loads(manifest_path.read_text(encoding='utf-8'))
        manifest.update(runtime_suites=actual, runtime_commands_verified=actual,
                        runtime_suite_count=len(actual), runtime_command_count_verified=len(actual),
                        combined_make_check_exit_code=0, make_check_abi_exit_code=0,
                        full_validation_log=str(log), latest_validation_receipt=str(receipt_path),
                        runtime_count_note='Exact executed commands, matched against RUNTIME_TESTS; compiler diagnostics are excluded.')
        save(manifest_path, manifest)
        save(header_state, {'fingerprint': header_fingerprint, 'headers': header_records, 'validated_by': str(receipt_path)})
    print(json.dumps(receipt, indent=2))
    return code


if __name__ == '__main__':
    sys.exit(main())
