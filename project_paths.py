"""Portable filesystem paths for reconstruction scripts."""
import os
from pathlib import Path

def _load_env():
    env_file = Path(__file__).resolve().parent / '.env.local'
    if not env_file.is_file():
        return
    for line in env_file.read_text(encoding='utf-8-sig').splitlines():
        line = line.strip()
        if not line or line.startswith('#') or '=' not in line:
            continue
        key, value = line.split('=', 1)
        os.environ.setdefault(key.strip(), value.strip().strip('"').strip("'"))

_load_env()
PROJECT_ROOT = Path(os.environ.get('PROJECT_ROOT') or Path(__file__).resolve().parent).expanduser().resolve()
USER_HOME = Path.home()
REFERENCE_PROJECT = Path(os.environ.get('REFERENCE_PROJECT') or PROJECT_ROOT / 'reference').expanduser().resolve()

def resolve_path(value):
    """Expand sanitized path prefixes while retaining subdirectories."""
    value = os.fspath(value)
    normalized = value.replace('\\', '/')
    for prefix, base in [('PROJECT_ROOT', PROJECT_ROOT), ('USER_HOME', USER_HOME), ('REFERENCE_PROJECT', REFERENCE_PROJECT)]:
        if normalized == prefix:
            return str(base)
        if normalized.startswith(prefix + '/'):
            return str(base.joinpath(*normalized[len(prefix)+1:].split('/')))
    return value
