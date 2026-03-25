---
oncalls: ['hack']
description: 'Debug Hack typechecker performance issues: find slow files via Scuba, profile with hh_single_type_check, build A/B comparison binaries, create reduced test cases, and test against www.'
apply_to_user_prompt: '(?i)(hack|hh|type.?check).*(slow|perf|performance|profil|optimi|speed|latenc|regress|bottleneck|scaling)|slow.*(type.?check|hack|hh_single)|profil.*(type.?check|hack)|hh_single.*(profil|perf|slow|bench)'
---

# Hack Typechecker Performance Debugging

Methodology for investigating and fixing performance issues in the Hack typechecker.

## When to Use

Use this skill when the user:
- Reports slow type checking for specific files or patterns
- Wants to profile the Hack typechecker on a file
- Needs to compare performance before/after a code change (A/B testing)
- Wants to find which files are slowest in production
- Needs to test typechecker changes against www

## 1. Finding Slow Files via Scuba

Query the `hh_process_file_history` Scuba table to find files with the highest second-typecheck CPU time (which excludes cold-cache overhead):

Run via CLI:
```bash
scuba -e "$(cat <<'EOF'
SELECT filename, AVG(end_second_cpu_time_ms - end_cpu_time_ms) / 1000 AS cpu_time_sec_avg
FROM hh_process_file_history
WHERE time >= NOW() - (24 * 60 * 60)
  AND end_second_cpu_time_ms - end_cpu_time_ms >= 500
  AND sandcastle_is_dry_run = '0'
GROUP BY filename ORDER BY cpu_time_sec_avg DESC LIMIT 20;
EOF
)"
```

## 2. Profiling Individual Files

```bash
# Profile a file
buck run @fbcode//mode/opt fbcode//hphp/hack/src:hh_single_type_check -- \
  --root ~/fbsource/www \
  --profile-type-check-twice <file.php>
```

Use `--profile-type-check-twice` only when trying to obtain overall
measurements. The other options are for digging deeper into issues.

- `--root <PATH>`: The root of the repo to typecheck. You can only drop this option if the <file.php> is not inside `www`.
- `--profile-type-check-twice`: Typechecks the file twice. **Always use the second run timing** — the first run warms caches.
- `--profile-top-level-definitions`: To find out which method has the performance issue
- `--skip-hierarchy-checks`: To test if skipping hierarchy checks improve performance
- `--skip-tast-checks`: To test if skipping TAST checks improve performance

## 3. A/B Binary Comparison

Build two binaries to compare performance before and after a change:

```bash
# 1. Build the optimized binary with your changes
buck build @fbcode//mode/opt fbcode//hphp/hack/src:hh_single_type_check
cp $(buck2 targets --show-output @fbcode//mode/opt fbcode//hphp/hack/src:hh_single_type_check 2>/dev/null | awk '{print $2}') /tmp/hh_stc_optimized

# 2. Revert your changes, build baseline
buck build @fbcode//mode/opt fbcode//hphp/hack/src:hh_single_type_check
cp $(buck2 targets --show-output @fbcode//mode/opt fbcode//hphp/hack/src:hh_single_type_check 2>/dev/null | awk '{print $2}') /tmp/hh_stc_baseline

# 3. Compare on target files
/tmp/hh_stc_baseline --profile-type-check-twice <file.php>
/tmp/hh_stc_optimized --profile-type-check-twice <file.php>
```

Use `@fbcode//mode/opt` for production-representative numbers when reporting final results.

## 4. Using `perf` to Find Where Time Is Spent

Use Linux `perf` to collect CPU samples and identify hot functions:

### Recording a Profile

```bash
# Build in opt mode for meaningful symbols
buck build @fbcode//mode/opt fbcode//hphp/hack/src:hh_single_type_check

# Record CPU samples while typechecking a slow file
perf record -g -- \
  $(buck2 targets --show-output @fbcode//mode/opt fbcode//hphp/hack/src:hh_single_type_check 2>/dev/null | awk '{print $2}') \
  <file.php>
```

- `-g`: Capture call graphs (stack traces) for understanding call chains
- The output is saved to `perf.data` in the current directory

### Analyzing the Profile

```bash
# Interactive TUI — navigate with arrow keys, Enter to drill into functions
perf report

# Show flat profile sorted by overhead
perf report --no-children --sort=dso,symbol

# Filter to just OCaml/Hack symbols (skip libc, kernel, etc.)
perf report --no-children --dso=hh_single_type_check.opt
```

In `perf report`:
- **Overhead**: percentage of total samples in this function
- **Children**: cumulative overhead including callees
- Press `+` to expand call chains, `a` to annotate source lines

### Tips

- **Always use `@fbcode//mode/opt`** — dev builds have different inlining and optimization behavior, making profiles misleading
- **Focus on the second typecheck** — if using `--profile-type-check-twice`, the first typecheck warms caches. Look at the portion of the profile after the first typecheck completes
- **OCaml symbol names** — look for `caml` prefixed symbols (e.g., `camlTyping_subtype__...`). The function name after `caml` maps back to OCaml module and function
- **Common hot spots** — `Typing_subtype`, `Typing_phase`

## 5. Testing optimisations

After making a performance optimization, verify correctness:

```bash
# Run typecheck tests
buck test @fbcode//mode/opt-clang fbcode//hphp/hack/test/typecheck:typecheck

# If error messages changed, update .exp files
buck test @fbcode//mode/opt-clang fbcode//hphp/hack/test/typecheck:typecheck \
  --config hack.update=always

# Run targeted tests only
buck test @fbcode//mode/opt-clang fbcode//hphp/hack/test/typecheck:typecheck \
  --config hack.test_filter=<pattern>

# Run NAST tests if AST-level changes were made
buck test @fbcode//mode/opt-clang fbcode//hphp/hack/test/nast/...
```
