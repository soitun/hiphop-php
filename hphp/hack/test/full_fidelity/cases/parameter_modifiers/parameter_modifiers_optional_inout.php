<?hh
// Optional and inout modifiers are incompatible in either order.

function f1(optional inout bool $x): void {}
function f2(inout optional bool $y): void {}
