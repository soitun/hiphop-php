<?hh

function f(inout num $x) :void{ var_dump($x); }
function test(int $b, int $c) :void{
  $x = false && HH\legacy_is_truthy($b);
  $x = HH\Lib\Legacy_FIXME\cast_for_arithmetic($x);
  $x += HH\Lib\Legacy_FIXME\cast_for_arithmetic(true && HH\legacy_is_truthy($b));
  $x += HH\Lib\Legacy_FIXME\cast_for_arithmetic(false || HH\legacy_is_truthy($b));
  $x += HH\Lib\Legacy_FIXME\cast_for_arithmetic(true || HH\legacy_is_truthy($b));

  $x += false ? $b : $c;
  $x += true ? $b : $c;
  f(inout $x);
}
<<__EntryPoint>> function main(): void {
test(2, 3);
}
