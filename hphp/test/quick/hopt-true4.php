<?hh

function foo(mixed $x) :mixed{
  if (!HH\legacy_is_truthy($x)) { return true; }
  else { return false; }
}
<<__EntryPoint>> function main(): void {
var_dump(foo(true));
var_dump(foo(1));
}
