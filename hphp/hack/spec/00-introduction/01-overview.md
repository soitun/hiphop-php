# Introduction

## What is Hack?

Hack is a programming language for HHVM that provides static typing with instantaneous type checking. It interoperates seamlessly with PHP while adding features like generics, async/await, and a powerful type system.

## Scope of This Specification

This specification defines the Hack programming language, including:

- Lexical structure (tokens, comments, literals)
- Types and the type system
- Expressions and operators
- Statements and control flow
- Functions and closures
- Classes, interfaces, and traits
- Generics and type parameters

The specification covers the static semantics (type checking) of Hack. Runtime behavior is documented separately in the HHVM documentation.

## Notation Conventions

This specification uses several notational conventions to describe grammar, typing rules, and code examples.

### Grammar Notation

Grammar rules are written in a BNF-like notation. Non-terminals are written in lowercase with hyphens. Terminals are written in `monospace`. Optional elements are followed by `?`. Repetition is indicated by `*` (zero or more) or `+` (one or more).

```grammar
function-definition:
  function-declaration-header compound-statement

function-declaration-header:
  attribute-specification? async? function name generic-type-parameter-list?
    ( parameter-list? ) : return-type where-clause?
```

### Typing Rules

Typing rules use mathematical notation with a horizontal line separating premises (above) from conclusions (below). The turnstile symbol `|-` reads as "entails" or "proves".

```text
G |- e : T
```

This reads: "In type environment G, expression e has type T."

A complete typing rule:

```text
G |- e1 : T1    G |- e2 : T2    T1 <: num    T2 <: num
------------------------------------------------------
                G |- e1 + e2 : num
```

This reads: "If e1 has type T1 and e2 has type T2, and both T1 and T2 are subtypes of num, then e1 + e2 has type num."

### Code Examples

Code examples are shown in fenced code blocks with a filename. Examples that should typecheck successfully use the `.hack` extension:

```example_valid.hack
function add(int $x, int $y): int {
  return $x + $y;
}
```

Examples that demonstrate type errors use the `.hack.type-errors` extension:

```example_error.hack.type-errors
function bad_add(int $x, string $y): int {
  return $x + $y;  // Error: string is not compatible with int
}
```

### Sharing Code Between Examples

Examples can share code by using a common prefix in their filenames. Files with the same prefix before the first `.` share a namespace:

```shared_example.definition.hack
class Counter {
  private int $count = 0;
  public function increment(): void {
    $this->count++;
  }
  public function get(): int {
    return $this->count;
  }
}
```

```shared_example.usage.hack
$c = new Counter();
$c->increment();
echo $c->get();  // 1
```

## How to Read This Specification

Each chapter focuses on a specific aspect of the language. Within chapters:

1. **Grammar** sections define the syntactic structure
2. **Semantics** sections explain meaning and behavior
3. **Typing Rules** sections provide formal type checking rules
4. **Examples** demonstrate usage and edge cases

Start with the types chapter if you want to understand Hack's type system. Start with expressions if you want to understand how code is evaluated.

## Relationship to Other Documentation

- **Hack Manual**: Provides tutorials and user-friendly explanations. Use it to learn Hack.
- **HIPs**: Document proposed and accepted language changes. The spec reflects accepted HIPs.
- **This Specification**: Provides authoritative definitions for language implementers and those needing precise semantics.
