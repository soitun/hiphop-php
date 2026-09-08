<?hh
<<file:__EnableUnstableFeatures('shape_splat_concrete', 'shape_splat_type_parameters', 'shape_splat_expression')>>

function sink(shape('x' => int, 'y' => string) $_): void {}

function duplicate<T as shape(...)>(T $a): shape(...T, ...T) {
  throw new Exception();
}

function test(): void {
  sink(duplicate(shape('x' => 1)));
}
