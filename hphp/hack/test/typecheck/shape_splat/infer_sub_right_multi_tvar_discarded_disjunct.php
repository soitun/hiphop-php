<?hh
<<file:
  __EnableUnstableFeatures(
    'shape_splat_concrete',
    'shape_splat_type_parameters',
    'shape_splat_expression',
    'union_intersection_type_hints',
  )>>

interface ShapeBox<+T> {}

interface ValueBox<+T> {}

interface Both<+TShape, +TValue> extends ShapeBox<TShape>, ValueBox<TValue> {}

function sink(
  (
    ShapeBox<shape('x' => int, 'y' => string)>
    | ValueBox<(function(int): void)>
  ) $_,
): void {}

function combine<T1 as shape(...), T2 as shape(...), TF>(
  TF $value,
  (function(): T1) $g,
  (function(): T2) $h,
): Both<shape(...T1, ...T2), TF> {
  throw new Exception();
}

function test(): void {
  sink(combine(
    'not an int',
    () ==> shape('x' => 1),
    () ==> shape('y' => 'hi'),
  ));
}
