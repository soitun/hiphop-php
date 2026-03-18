<?hh
// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

// Test that the Tany_found warning (Warn[12036]) does NOT fire when the Tany
// type was elaborated from MISSING_TYPE_IN_HIERARCHY, including when the type
// flows through unions, intersections, join points, and other flows.
//
// Also test that the warning DOES fire for genuine Tany that does not come
// from MISSING_TYPE_IN_HIERARCHY.

namespace HH_FIXME {
  type MISSING_TYPE_IN_HIERARCHY = mixed;
}

// ════════════════════════════════════════════════════════════════════════════
// Setup for genuine Tany (NOT from MISSING_TYPE_IN_HIERARCHY)
// ════════════════════════════════════════════════════════════════════════════

namespace GenuineTany {
  interface IWithoutTB {}

  abstract class DefinesTB implements IWithoutTB {
    abstract const type TB;
  }

  abstract class UsesTB {
    abstract const type TA as DefinesTB;
    const type TB = this::TA::TB;
    public function __construct(protected this::TB $tb): void {}
  }

  // Redeclares TA with an upper bound that doesn't define TB, causing
  // this::TB to resolve to genuine Tany
  trait TRedeclareTA {
    abstract const type TA as IWithoutTB;
  }

  trait TGenuineTany {
    require extends UsesTB;
    use TRedeclareTA;
  }
}

namespace Test {

  function return_missing(): \HH_FIXME\MISSING_TYPE_IN_HIERARCHY {
    return 1;
  }

  // ── Basic: direct use of MISSING_TYPE_IN_HIERARCHY ──────────────────────────
  function test_direct(\HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x): void {
    // No Warn[12036] expected: $x is Tany from MISSING_TYPE_IN_HIERARCHY
    $y = $x;
  }

  // ── Union: MISSING_TYPE_IN_HIERARCHY merged with another Tany via union ─────
  function test_union(
    bool $b,
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $y,
  ): void {
    // Both branches produce Tany from MISSING_TYPE_IN_HIERARCHY; the union
    // should preserve the reason.
    $z = $b ? $x : $y;
    // No Warn[12036] expected
    $_ = $z;
  }

  // ── Union with a concrete type ──────────────────────────────────────────────
  function test_union_with_concrete(
    bool $b,
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
    int $y,
  ): void {
    // Union of Tany (from MISSING_TYPE_IN_HIERARCHY) and int collapses to Tany
    // since Tany is a supertype of everything. The reason should be preserved.
    $z = $b ? $x : $y;
    // No Warn[12036] expected
    $_ = $z;
  }

  // ── Intersection: MISSING_TYPE_IN_HIERARCHY through an intersection ─────────
  function test_intersection(
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    // Refinement via `is` creates an intersection. Since Tany & T = Tany,
    // the reason should be preserved.
    if ($x is int) {
      // No Warn[12036] expected
      $_ = $x;
    }
  }

  // ── Join point: MISSING_TYPE_IN_HIERARCHY across control flow branches ──────
  function test_join_point(
    bool $b,
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    if ($b) {
      $z = $x;
    } else {
      $z = $x;
    }
    // $z has gone through a join point; the reason should be preserved.
    // No Warn[12036] expected
    $_ = $z;
  }

  // ── Join point with concrete type ───────────────────────────────────────────
  function test_join_point_with_concrete(
    bool $b,
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    if ($b) {
      $z = $x;
    } else {
      $z = 42;
    }
    // $z is union of Tany (MISSING_TYPE_IN_HIERARCHY) and int, collapsed to Tany.
    // No Warn[12036] expected
    $_ = $z;
  }

  // ── Method call on MISSING_TYPE_IN_HIERARCHY ────────────────────────────────
  function test_method_call(
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    // The method call result should preserve the MISSING_TYPE_IN_HIERARCHY reason
    // since the receiver's Tany flows to the result via flow_prop_access.
    $result = $x->someMethod();
    // No Warn[12036] expected
    $_ = $result;
  }

  // ── Property access on MISSING_TYPE_IN_HIERARCHY ───────────────────────────
  function test_property_access(
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    $prop = $x->someProp;
    // No Warn[12036] expected
    $_ = $prop;
  }

  // ── Return from function ───────────────────────────────────────────────────
  function test_return_value(): void {
    $x = return_missing();
    // No Warn[12036] expected
    $_ = $x;
  }

  // ── Assignment: result of assigning Tany ───────────────────────────────────
  function test_assignment(\HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x): void {
    $a = $x;
    $b = $a;
    $c = $b;
    // No Warn[12036] expected after multiple assignments
    $_ = $c;
  }

  // ── Shape access (array_get flow) ─────────────────────────────────────────
  function test_shape_access(\HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x): void {
    // $x['key'] goes through flow_array_get
    $val = $x['key'];
    // No Warn[12036] expected
    $_ = $val;
  }

  // ── Nested shape access ───────────────────────────────────────────────────
  function test_nested_shape_access(
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    $val = $x['a']['b']['c'];
    // No Warn[12036] expected
    $_ = $val;
  }

  // ── Dict/vec index access ─────────────────────────────────────────────────
  function test_index_access(\HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x): void {
    $val = $x[0];
    // No Warn[12036] expected
    $_ = $val;
  }

  // ── Method call with arguments ────────────────────────────────────────────
  function test_method_call_with_args(
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    // The result of calling a method with arguments on Tany should preserve
    // the MISSING_TYPE_IN_HIERARCHY reason
    $result = $x->doSomething(1, 'foo', true);
    // No Warn[12036] expected
    $_ = $result;
  }

  // ── Chained method calls ──────────────────────────────────────────────────
  function test_chained_method_calls(
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    $result = $x->first()->second()->third();
    // No Warn[12036] expected
    $_ = $result;
  }

  // ── Passing as function argument ──────────────────────────────────────────
  function accept_int(int $_): void {}
  function test_as_argument(\HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x): void {
    // Tany is silently compatible with int, no error expected
    accept_int($x);
  }

  // ── Return expression flow ────────────────────────────────────────────────
  function test_return_flow(
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): \HH_FIXME\MISSING_TYPE_IN_HIERARCHY {
    return $x;
  }

  // ── Nullsafe method call ──────────────────────────────────────────────────
  function test_nullsafe_method_call(
    \HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x,
  ): void {
    $result = $x?->maybeMethod();
    // No Warn[12036] expected
    $_ = $result;
  }

  // ── Arithmetic operations ─────────────────────────────────────────────────
  function test_arithmetic(\HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x): void {
    $sum = $x + 1;
    // No Warn[12036] expected
    $_ = $sum;
  }

  // ── String concatenation ──────────────────────────────────────────────────
  function test_string_concat(\HH_FIXME\MISSING_TYPE_IN_HIERARCHY $x): void {
    $s = $x.'suffix';
    // No Warn[12036] expected
    $_ = $s;
  }
}
