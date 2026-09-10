<?hh
// Copyright 2004-present Facebook. All Rights Reserved.

function do_print(mixed $f, mixed $v, mixed $k, mixed $d) :void{
  $print = HH\FIXME\UNSAFE_CAST<mixed, (function(mixed): mixed)>(
    $f,
    'All supplied callbacks accept one value',
  );
  $print($v); print "\n";
  $print($k); print "\n";
  $print($d); print "\n";
}

function run(mixed $v, mixed $k, mixed $d) :void{
  do_print(var_dump<>, $v, $k, $d);
  do_print(var_export<>, $v, $k, $d);
  do_print(print_r<>, $v, $k, $d);
}
<<__EntryPoint>> function main(): void {
run(vec['a', 'b', 'c'],
    keyset['a', 'b', 'c'],
    dict[0 => 'a', 1 => 'b', 2 => 'c']);
run(vec[], keyset[], dict[]);
}
