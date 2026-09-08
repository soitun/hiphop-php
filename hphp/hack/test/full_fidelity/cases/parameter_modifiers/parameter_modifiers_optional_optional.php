<?hh
// Repeating the optional modifier is invalid.

function f1((function(optional optional bool): void) $_): void {}
