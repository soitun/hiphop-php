# Hack Language Specification

This directory contains the unified Hack language specification, combining parser grammar, typing rules, and testable examples in a single authoritative source.

## Purpose

The specification serves two audiences:

1. **Language Implementers**: Provides formal grammar and typing rules needed to implement or modify the Hack type checker and runtime.
2. **New Engineers**: Offers clear explanations with tested examples to learn Hack's type system and semantics.

## Directory Structure

The specification is organized into numbered chapters:

```text
spec/
  00-introduction/       # Overview, notation, conventions
  01-lexical-structure/  # Tokens, comments, literals
  02-types/              # Type system fundamentals
  03-expressions/        # Expressions and operators
  ...
```

Each chapter may contain multiple markdown files for subtopics.

## Content Format

### Grammar Blocks

Grammar rules use fenced code blocks with the `grammar` language tag:

```grammar
function-definition:
  function-declaration-header compound-statement

function-declaration-header:
  attribute-specification? async? function name generic-type-parameter-list?
    ( parameter-list? ) : return-type where-clause?
```

### Typing Rules

Typing rules use mathematical notation in fenced code blocks:

```text
G |- e1 : T1    G |- e2 : T2    T1 <: num    T2 <: num
------------------------------------------------------
                G |- e1 + e2 : num
```

### Code Examples

Code examples follow the same format as the [Hack manual](../manual/hack/90-contributing/10-code-samples.md).

For code that should typecheck without errors:
```filename.hack
function greet(string $name): string {
  return "Hello, ".$name;
}
```

For code that demonstrates type errors:
```filename.hack.type-errors
function bad(): int {
  return "not an int";  // Type error
}
```

The `hh_manual` tool extracts these examples and runs them through the type checker as part of CI.

## Running Tests

To extract and test all code examples:

```bash
# Extract code blocks from spec markdown files
buck run //hphp/hack/src/hh_manual:hh_manual extract hphp/hack/spec/

# Run the extracted tests
buck test //hphp/hack/spec:extracted_from_spec
buck test //hphp/hack/spec:extracted_from_spec_error
```

## Adding New Content

1. Create or edit a markdown file in the appropriate chapter directory.
2. Add code examples using the filename convention (see above).
3. Run the extraction and test commands to verify examples are correct.
4. Submit your changes for review.

## Relationship to Other Documentation

- **Hack Manual** (`../manual/`): User-facing documentation with tutorials and guides. The spec provides more formal definitions.
- **HIPs (Hack Improvement Proposals)**: Describe proposed language changes. The spec documents accepted features.
- **HHVM Documentation**: Covers runtime behavior. The spec focuses on static typing and language semantics.
