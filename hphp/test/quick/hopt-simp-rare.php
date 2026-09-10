<?hh

function f1(mixed $x) :mixed{
  return HH\Lib\Legacy_FIXME\lt(true, $x);
}

function f2(mixed $x) :mixed{
  return "0.0" == $x;
}

function f3(mixed $x) :mixed{
  $y = vec[1];
  return $x == $y;
}

<<__EntryPoint>> function main(): void {
var_dump(f1(4));
var_dump(f2(false));
var_dump(f3(3));
}
