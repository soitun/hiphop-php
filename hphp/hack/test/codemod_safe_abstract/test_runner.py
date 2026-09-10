#!/usr/bin/env python3

# pyre-unsafe

"""
Generates snapshots of the codemod safe abstract orchestrator output for testing.
See ./BUCK for how to run this.
Example output: ./test_simple_abstraxt/.hhconfig.exp

"""

import argparse
import os
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def run_command(cmd, cwd=None):
    """Run a command and return stdout, stderr, and return code."""
    try:
        result = subprocess.run(
            cmd, cwd=cwd, capture_output=True, text=True, shell=True
        )
        return result.stdout, result.stderr, result.returncode
    except Exception as e:
        return "", str(e), 1


def get_changes_diff(working_dir, initial_commit):
    """Get diff showing changes made by the orchestrator using sapling."""
    # Use sapling diff to show changes from the initial commit to current state
    cmd = (
        f"sl diff -r {initial_commit} "
        "--reason 'verify Safe Abstract codemod output - sl help diff'"
    )
    stdout, stderr, returncode = run_command(cmd, cwd=working_dir)

    if returncode == 0:
        return stdout.strip()
    else:
        # If sapling diff fails, return empty (no changes)
        return ""


def main():
    parser = argparse.ArgumentParser(
        description="Test runner for codemod safe abstract orchestrator"
    )
    parser.add_argument("config_filename", help="Config filename (e.g., .hhconfig)")
    parser.add_argument(
        "orchestrator_command", help="command to orchestrate the codemod"
    )

    args = parser.parse_args()

    # The verify.py script runs us from the test directory, so we can use the current working directory
    test_dir = Path.cwd()
    config_path = test_dir / args.config_filename

    if not config_path.exists():
        print(f"Config file {config_path} not found", file=sys.stderr)
        sys.exit(1)

    if not test_dir.exists():
        print(f"Test directory {test_dir} does not exist", file=sys.stderr)
        sys.exit(1)

    # Create temporary directories
    with tempfile.TemporaryDirectory() as tmp_base:
        original_dir = os.path.join(tmp_base, "original")
        working_dir = os.path.join(tmp_base, "working")

        # Copy test directory to both original and working directories
        shutil.copytree(test_dir, original_dir)
        shutil.copytree(test_dir, working_dir)

        # Initialize sapling repository in working directory (required for the orchestrator)
        stdout, stderr, returncode = run_command(
            "sl init --reason 'create Safe Abstract test repo - sl help init'",
            cwd=working_dir,
        )
        if returncode != 0:
            print(f"Failed to init sl repo: {stderr}", file=sys.stderr)
            sys.exit(1)

        # Add all files to sapling
        stdout, stderr, returncode = run_command(
            "sl add . --reason 'track Safe Abstract test files - sl help add'",
            cwd=working_dir,
        )
        if returncode != 0:
            print(f"Failed to add files to sl: {stderr}", file=sys.stderr)
            sys.exit(1)

        # Make initial commit and get its hash
        stdout, stderr, returncode = run_command(
            "sl commit -m 'Initial commit' "
            "--reason 'checkpoint Safe Abstract test input - sl help commit'",
            cwd=working_dir,
        )
        if returncode != 0:
            print(f"Failed to make initial commit: {stderr}", file=sys.stderr)
            sys.exit(1)

        # Get the initial commit hash
        initial_commit_stdout, _, returncode = run_command(
            "sl id --reason 'record Safe Abstract test base - sl help id'",
            cwd=working_dir,
        )
        if returncode != 0:
            print("Failed to get initial commit hash", file=sys.stderr)
            sys.exit(1)
        initial_commit = initial_commit_stdout.strip()

        artifacts_dir = os.path.join(tmp_base, "artifacts")
        cmd = (
            f"{args.orchestrator_command} --root {shlex.quote(working_dir)} "
            f"--artifacts-dir {shlex.quote(artifacts_dir)}"
        )
        stdout, stderr, returncode = run_command(cmd, cwd=working_dir)

        diff_output = get_changes_diff(working_dir, initial_commit)

        if returncode != 0:
            if diff_output.strip():
                print(diff_output)
            if stdout.strip():
                print(stdout)
            if stderr.strip():
                print(stderr, file=sys.stderr)
            if not (diff_output.strip() or stdout.strip() or stderr.strip()):
                print(f"Command failed with exit code {returncode}")
            sys.exit(returncode)

        status_stdout, status_stderr, status_returncode = run_command(
            "sl status --reason 'verify Safe Abstract test cleanliness - sl help status'",
            cwd=working_dir,
        )
        if status_returncode != 0 or status_stdout.strip():
            print(status_stdout)
            print(status_stderr, file=sys.stderr)
            print("Codemod left a non-clean working directory", file=sys.stderr)
            sys.exit(1)

        if stdout.count("SAFE_ABSTRACT_RUN_SUCCESS") != 1:
            print("Codemod did not emit exactly one success sentinel", file=sys.stderr)
            sys.exit(1)

        summaries = re.findall(r"^SAFE_ABSTRACT_SUMMARY\t.*$", stdout, re.MULTILINE)
        if not summaries or "\trewrites=0" not in summaries[-1]:
            print("Codemod did not finish with a zero-rewrite pass", file=sys.stderr)
            sys.exit(1)
        rewrite_counts = [
            int(re.search(r"\trewrites=(\d+)$", summary).group(1))
            for summary in summaries
        ]
        if any(rewrites == 0 for rewrites in rewrite_counts[:-1]):
            print("Codemod continued after a zero-rewrite pass", file=sys.stderr)
            sys.exit(1)

        round_headers = re.findall(
            r"^round (\d+) \(commit ([0-9a-f]+)\)$", stdout, re.MULTILINE
        )
        expected_rounds = [str(index) for index in range(len(summaries))]
        if [
            round_number for round_number, _commit in round_headers
        ] != expected_rounds or len(
            {commit for _round_number, commit in round_headers}
        ) != len(round_headers):
            print(
                "Every positive rewrite pass must be checkpointed before the next round",
                file=sys.stderr,
            )
            sys.exit(1)

        if diff_output.strip():
            print(diff_output)
        else:
            print("")


if __name__ == "__main__":
    main()
