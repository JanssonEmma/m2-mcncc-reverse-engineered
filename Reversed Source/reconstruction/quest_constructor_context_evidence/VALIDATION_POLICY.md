# Validation policy

Use risk-based validation to avoid rebuilding the full regression collection after every bounded change.

- For each change, run the new suites and affected existing suites, relevant ABI checks, strict target32 compilation, and an optimized build/runtime of the actual dependency graph. Reuse unchanged SDK artifacts only after source/object/archive hashes pass.
- Choose affected suites from actual callers, shared headers, ownership, packet layout and global build recipes. Record the choice and any excluded unsafe/static-only domains. Broaden checks when a failure or unresolved dependency justifies it.
- Run the full registered suite at meaningful batch milestones, before release/readiness claims, and when broad ownership, ABI/layout, shared-library or global recipe changes make focused coverage insufficient. A bounded inline-body addition without layout changes does not automatically require a full run.
- Preserve the latest full receipt, ordered suite list, source-state scope and date. A later focused pass does not refresh that full result.
- Report registered suites, executed focused suites and the latest full run separately. Never infer a full pass from successful compilation, a registry count or a helper/index filename.

Checkpoint102:116 suites are registered;4 focused runtime suites plus relevant ABI/strict/O2 checks were selected. The last full validation remains checkpoint101's114 suites on its earlier source state. Files named `combined116-PROOF_INDEX` identify the registered group, not execution of all116 runtimes. No full116 run is claimed.

Preserve failed receipts and exact reviewed fixes. Coverage mappings remain bounded source/test evidence, not whole-server equivalence or a completion percentage.
