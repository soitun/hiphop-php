<?hh
function main(mixed $a, int $i) :mixed{
  $a = HH\FIXME\UNSAFE_CAST<mixed, dynamic>(
    $a,
    'The test intentionally removes an element from a vec',
  );
  unset($a[$i]);
  $a[] = 'foo';
  return $a;
}
<<__EntryPoint>> function main_entry(): void {
var_dump(main(vec['a', 'b'], 1));
}
