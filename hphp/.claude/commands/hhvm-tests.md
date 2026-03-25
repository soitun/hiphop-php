---
description: "Run HHVM tests for changed files in a given mode. Argument: one of {interp, jit, interp-repo, jit-repo}. Defaults to jit. Usage: /hhvm-tests jit-repo"
---

# HHVM Tests

Run HHVM tests for files changed in the current commit, using the specified execution mode.

**Argument:** `$ARGUMENTS`

## Step 1: Parse the mode

The argument must be one of: `interp`, `jit`, `interp-repo`, `jit-repo`. If no argument is provided, default to `jit`.

If the argument is not one of the valid modes, stop and tell the user the valid options.

Map the mode to test runner flags:

| Mode | Flags |
|------|-------|
| `interp` | `-m interp` |
| `jit` | `-m jit` |
| `interp-repo` | `-m interp -r` |
| `jit-repo` | `-m jit -r` |

## Step 2: Identify changed files

Run `sl status` to find files changed in the working copy. Also run `sl diff --stat -r '.^' -r '.'` to find files changed in the current commit.

Combine both lists and filter to only files under `hphp/` that are relevant source files (`.cpp`, `.h`, `.php`, `.hack`, `.hhas`).

## Step 3: Find related tests

For each changed file, determine which tests to run:

1. **If the changed file is itself a test file** (under `hphp/test/`): run that test directly.
2. **If the changed file is a runtime/extension source file** (under `hphp/runtime/`, `hphp/hhbbc/`, `hphp/jit/`, etc.): run `hphp/test/quick` as a baseline. Also look for tests in `hphp/test/slow/` that are specifically related to the changed component if identifiable.

If no changed files are found under `hphp/`, inform the user and ask what tests they'd like to run.

## Step 4: Build HHVM

Check if HHVM is built already. The canonical way to do so is by running the test command and seeing if it finds an HHVM binary. If not, build the binary from the fbsource/fbcode directory using this exact command:

```bash
cd ~/fbsource/fbcode && ls hphp/hhvm/hhvm || buck build @//mode/dbgo -c hhvm.use_lowptr=1 -c fbcode.split-dwarf5=True //hphp/hhvm:hhvm
```

If the build fails, report the error to the user and stop.

## Step 5: Run the tests

Run the test runner from the fbsource/fbcode directory:

```bash
hphp/test/run <test_paths> <flags>
```

Where `<flags>` are the mapped flags from Step 1.

Report the results to the user, including any failures with their diff output.
