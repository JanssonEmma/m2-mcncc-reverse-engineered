# Repository Development Instructions

## Path Handling

This repository must remain portable and must not expose local developer information.

- Never hardcode absolute paths containing Windows usernames, drive letters or personal home directories.
- Resolve `PROJECT_ROOT` dynamically from the repository root or from the `PROJECT_ROOT` environment variable.
- Resolve `USER_HOME` using `Path.home()` in Python.
- Resolve `REFERENCE_PROJECT` using the environment variable of the same name.
- Read local configuration from `.env.local` where appropriate.
- Never commit `.env.local` or any private machine configuration.

## Script Compatibility

- Existing Python scripts and Makefiles must continue to work.
- Replace literal placeholders such as `PROJECT_ROOT`, `USER_HOME` and `REFERENCE_PROJECT` with dynamically resolved paths.
- Use `pathlib.Path` for filesystem operations.
- For Makefiles, use environment variables or paths relative to the Makefile.
- Do not modify historical evidence, hashes or generated metadata unless necessary.
- Never assume that the current working directory is the repository root.
- Verify modified scripts using syntax checks and relevant execution tests.

## Privacy

Before committing, scan modified files for personal usernames, absolute paths, credentials and other sensitive information.
