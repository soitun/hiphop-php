<?hh

function test(mixed $a, mixed $b) :void{
  $a = HH\FIXME\UNSAFE_CAST<mixed, dynamic>(
    $a,
    'The test intentionally increments values with legacy coercions',
  );
  $a++;
  var_dump($a,$b);
  }
<<__EntryPoint>> function main(): void {
$a = vec[];
$a[] = 1;
test(false, $a);
test(true, $a);
test(1, $a);
test(1.0, $a);
}
