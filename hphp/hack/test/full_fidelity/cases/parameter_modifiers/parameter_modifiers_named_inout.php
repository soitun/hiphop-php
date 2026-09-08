<?hh
// Named and inout modifiers are incompatible in either order.
<<file:__EnableUnstableFeatures('named_parameters')>>

function f1(named inout bool $x): void {}
function f2(inout named bool $y): void {}
