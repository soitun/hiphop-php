<?hh

function foo1(int $x, int $y) :mixed{
  return $x + (- $y);
}

function foo2(int $x, int $y) :mixed{
  return $x - (- $y);
}

function foo3(int $x) :mixed{
  return -$x;
}

function foo4(int $x) :mixed{
  return $x - $x + 3;
}

function foo5(int $x) :mixed{
  return $x + 3 - $x;
}

function foo6(int $x) :mixed{
  return 0 - $x;
}

function foo7(int $x) :mixed{
  return $x - 0;
}

function foo8(int $x) :mixed{
  return $x - 1;
}

function foo9(int $x) :mixed{
  return 1 - $x;
}
<<__EntryPoint>> function main(): void {
var_dump(foo1(5, 2));
var_dump(foo2(1, 2));
var_dump(foo3(-3));
var_dump(foo4(5));
var_dump(foo5(5));
var_dump(foo6(-3));
var_dump(foo7(3));
var_dump(foo8(4));
var_dump(foo9(-2));
}
