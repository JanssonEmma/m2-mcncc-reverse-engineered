"""Record a passed focused batch. Input plans explicitly name reviewed source and test anchors."""
import hashlib
import json
import re
import shutil
import subprocess
import sys
from pathlib import Path

def load(path):
    return json.loads(path.read_text(encoding="utf-8-sig"))

def save(path, data):
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")

def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main(plan_path):
    plan_path = plan_path.resolve()
    plan = load(plan_path)
    root = Path(plan["project"])
    rec = root / "reconstruction"
    checkpoint = plan["checkpoint"]
    archive = rec / plan["archive"]
    assert not archive.exists(), archive
    proof_path = Path(plan["receipt"]).resolve()
    proof = load(proof_path)
    assert proof["status"] == "passed" and proof["scope"] == "focused" and proof["exit_code"] == 0
    assert proof["abi_requested"] == plan["abi"]
    suites = proof["suites"]
    assert suites == plan["suites"] == proof["executed_runtime_commands"]
    rows = load(Path(plan["source_guards"]))
    for row in rows:
        path = root / row["path"]
        assert (digest(path) if path.is_file() else None) == row["after"], path

    status_path = rec / "validation-status.json"
    status = load(status_path)
    status_key = plan["status_key"]
    assert status_key not in status
    assert status[plan["baseline_key"]]["status"] == "passed_focused"
    registry = next(line.split()[2:] for line in (rec/"tests/Makefile").read_text().splitlines()
                    if line.startswith("RUNTIME_TESTS = "))
    assert len(registry) == len(set(registry)) and set(suites) <= set(registry)
    assert registry == plan.get("new_suites", []) + status["registered_runtime_suites"]
    overrides_path = rec / "function-coverage-overrides.json"
    overrides = load(overrides_path)
    originals = []
    recorded = {row["address"] for row in overrides["reviews"]}
    for reviewed in plan["reviews"]:
        row = dict(reviewed)
        original_path = Path(row.pop("export"))
        export = load(original_path)
        if "functions" in export:
            assert export["target_sha256"] == overrides["target_sha256"]
            export = next(f for f in export["functions"] if f["function"]["address"] == row["address"])
        assert export["function"]["address"] == row["address"] and not export["truncated"]
        assert row["address"] not in recorded
        recorded.add(row["address"])
        for ref in row["source_refs"] + row.get("test_refs", []):
            assert ref["anchor"] in (root/ref["path"]).read_text(encoding="utf-8-sig"), ref
        originals.append(export)
        overrides["reviews"].append(row)

    archive.mkdir()
    shutil.copy2(plan_path, archive / "record-plan.json")
    shutil.copy2(Path(__file__), archive / "record_focused.py")
    for source in map(Path, plan["evidence_packages"]):
        shutil.copytree(source, archive/source.name, ignore=shutil.ignore_patterns("*.exe","*.o","__pycache__"))
    for source in [proof_path, Path(proof["log"])] + list(map(Path, plan.get("extra_proofs", []))):
        shutil.copy2(source, archive/source.name)
    for name in ("CURRENT_STATUS.md","DEPENDENCY_FRONTIER.md","RESUME_CHECKPOINT.md",
                 "validation-status.json","function-coverage-overrides.json"):
        shutil.copy2(rec/name, archive/(name+".before"))
    save(rec/(plan["archive"]+"-ghidra.json"), dict(target_sha256=overrides["target_sha256"],functions=originals))
    save(overrides_path, overrides)
    subprocess.run([sys.executable, str(rec/"tools/update_coverage.py"), "--inventory",
                    str(rec/"target-function-inventory.json")], check=True)
    ledger = load(rec/"function-coverage-ledger.json")
    assert not ledger["warnings"]

    full = status["last_full_validation"]
    manifest = dict(checkpoint=checkpoint,status="passed_focused",validation_scope="focused_only",
                    registered_runtime_commands=len(registry),focused_runtime_commands=suites,abi=proof["abi_requested"],
                    source_hashes=rows,receipt_sha256=digest(proof_path),last_full_validation=full)
    save(archive/"RECORD_MANIFEST.json",manifest)
    status.update(registered_runtime_suite_count=len(registry),registered_runtime_suites=registry,
                  latest_validation_scope="focused_only",latest_validation_receipt=str(proof_path),
                  latest_focused_receipt=str(proof_path),latest_focused_suites=suites,latest_focused_check_exit_code=0)
    status[status_key] = manifest
    save(status_path,status)
    summary = (f'Checkpoint {checkpoint} {plan["summary"]} {len(suites)} focused runtime suites'
               + (" and ABI checks passed." if proof["abi_requested"] else " passed.")
               + f' The registry has {len(registry)} suites. The last full run remains checkpoint {full["checkpoint"]}.')
    for name in ("CURRENT_STATUS.md","DEPENDENCY_FRONTIER.md"):
        p = rec/name
        p.write_text(f"# Checkpoint {checkpoint}\n\n{summary}\n\n"+p.read_text(encoding="utf-8-sig"),encoding="utf-8")
    resume = (f"# Metin2 reconstruction restart — checkpoint {checkpoint}\n\n{summary}\n\n"
              f'Evidence, source guards and focused proof are in reconstruction/{plan["archive"]}.\n\n'
              + plan["next"] + "\n\nCurrent scope excludes Monarch. Batch related focused tests. "
              "Keep the original ELF unexecuted and unmodified.\n")
    (rec/"RESUME_CHECKPOINT.md").write_text(resume,encoding="utf-8")
    Path(plan["workspace_resume"]).write_text(resume,encoding="utf-8")
    readme = root.parent/"README.md"
    text = readme.read_text(encoding="utf-8-sig")
    text = re.sub(r"checkpoint \d+ \(focused validation\)", f"checkpoint {checkpoint} (focused validation)", text, count=1)
    text = re.sub(r"- \*\*\d+ regression programs[^\n]*", "- "+summary, text, count=1)
    text = re.sub(r"\*\*\d+ entries with structured address-backed evidence\*\*",
                  f'**{ledger["evidence_available"]} entries with structured address-backed evidence**',text)
    text = re.sub(r"\*\*\d+ bounded",f'**{ledger["summary"]["reviewed_implementation_test_mapping"]} bounded',text)
    readme.write_text(text,encoding="utf-8")
    print(summary)

if __name__ == "__main__":
    main(Path(sys.argv[1]))
