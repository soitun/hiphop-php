<?hh

function f() :mixed{
  $m = Map {
1 => 'a', '2' => 'b'}
;
  var_dump(array_key_exists(1, $m));
  var_dump(array_key_exists('1', $m));
  var_dump(array_key_exists(2, $m));
  var_dump(array_key_exists('2', $m));
  var_dump(array_key_exists(3, $m));
  var_dump(array_key_exists('3', $m));
  echo "========\n";
  $x = 'array_key_exists';
  $x[0] = 'a';
  var_dump(HH\dynamic_fun($x)(1, $m));
  var_dump(HH\dynamic_fun($x)('1', $m));
  var_dump(HH\dynamic_fun($x)(2, $m));
  var_dump(HH\dynamic_fun($x)('2', $m));
  var_dump(HH\dynamic_fun($x)(3, $m));
  var_dump(HH\dynamic_fun($x)('3', $m));
}

<<__EntryPoint>>
function main_841() :mixed{
f();
}
