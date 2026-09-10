<?hh

function same(mixed $left, mixed $right) :void{
  echo ($left === $right) ? "true\n" : "false\n";
}

function eq(mixed $left, mixed $right) :void{
  echo (HH\Lib\Legacy_FIXME\eq($left, $right)) ? "true\n" : "false\n";
}

function neq(mixed $left, mixed $right) :void{
  echo (HH\Lib\Legacy_FIXME\neq($left, $right)) ? "true\n" : "false\n";
}
<<__EntryPoint>> function main(): void {
same(false, 0);
neq(0, "b");
eq(true, -1);
}
