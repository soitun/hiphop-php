<?hh
<<file:__EnableUnstableFeatures('shape_splat_concrete', 'shape_splat_type_parameters', 'shape_splat_expression')>>

function sink(shape('x' => int, 'y' => string) $_): void {}

function combine<T1 as shape(...), T2 as shape(...)>(
  T1 $a,
  (function(int): T2) $f,
): shape(...T1, ...T2) {
  throw new Exception();
}

function test(): void {
  sink(combine(
    shape('x' => 1),
    $_ ==> shape('y' => 'hi'),
  ));
}
