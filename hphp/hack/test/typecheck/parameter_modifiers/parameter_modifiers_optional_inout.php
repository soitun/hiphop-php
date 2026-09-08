<?hh
// Optional and inout modifiers are incompatible in either order.

function f1((function(optional inout bool): void) $_): void {}
function f2((function(inout optional bool): void) $_): void {}
