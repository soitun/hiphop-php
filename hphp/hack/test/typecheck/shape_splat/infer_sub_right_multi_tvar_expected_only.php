<?hh
<<file:__EnableUnstableFeatures('shape_splat_concrete', 'shape_splat_type_parameters', 'shape_splat_expression')>>

function sink(shape('x' => int, 'y' => string) $_): void {}

function make<T1 as shape(...), T2 as shape(...)>(
  ?T1 $_a = null,
  ?T2 $_b = null,
): shape(...T1, ...T2) {
  throw new Exception();
}

function test(): void {
  sink(make());
}
