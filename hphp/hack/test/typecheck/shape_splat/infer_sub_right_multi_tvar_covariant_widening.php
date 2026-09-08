<?hh
<<file:__EnableUnstableFeatures('shape_splat_concrete', 'shape_splat_type_parameters', 'shape_splat_expression')>>

interface Box<+T> {}

function sink(Box<shape('x' => num, 'y' => string)> $_): void {}

function combine<T1 as shape(...), T2 as shape(...)>(
  T1 $a,
  T2 $b,
): Box<shape(...T1, ...T2)> {
  throw new Exception();
}

function test(): void {
  sink(combine(shape('x' => 1), shape('y' => 'hi')));
}
