<?hh

function foo(int $a, inout ?int $b, inout ?int $c, int $d) :void{
  $a = 10;
  $b = 20;
  $c = HH\FIXME\UNSAFE_CAST<num, int>(
    HH\Lib\Legacy_FIXME\cast_for_arithmetic($c),
    'The test only supplies integers and null',
  );
  $c *= 10;
  $d *= 10;
  echo (__METHOD__."(): a: ".$a.", b: ".$b.", c: ".$c.", d: ".$d."\n");
}

<<__EntryPoint>> function main(): void {
  $a = 1;
  $b = 2;
  $c = 3;
  $d = 4;
  echo (__METHOD__."(): a: ".$a.", b: ".$b.", c: ".$c.", d: ".$d."\n");
  foo($a, inout $b, inout $c, $d);
  $b_int = HH\FIXME\UNSAFE_CAST<?int, int>(
    $b,
    'foo assigns an integer before returning',
  );
  $c_int = HH\FIXME\UNSAFE_CAST<?int, int>(
    $c,
    'foo assigns an integer before returning',
  );
  echo (__METHOD__."(): a: ".$a.", b: ".$b_int.", c: ".$c_int.", d: ".$d."\n");

  $foo = null;
  $bar = null;
  foo(123, inout $foo, inout $bar, 456);  // should not warn that $foo/$bar are undefined
  var_dump($foo, $bar);
}
